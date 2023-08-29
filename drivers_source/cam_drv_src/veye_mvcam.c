// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022, www.veye.cc
 *
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/lcm.h>
#include <linux/crc32.h>
#include <linux/version.h>
//#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
//#include <media/v4l2-fwnode.h>
//#include <media/v4l2-mediabus.h>
#include <media/v4l2-event.h>
#include <linux/of_graph.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0)
#include <media/v4l2-fwnode.h>
#else
#include <media/v4l2-of.h>
#endif

#include <media/tegra-v4l2-camera.h>
#include <media/camera_common.h>
#include <media/mc_common.h>

#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>

#define DRIVER_VERSION			KERNEL_VERSION(1, 0x01, 0x03) 
#include "veye_mvcam.h"

//reserved
/* Embedded metadata stream structure */
#define VEYE_MV_EMBEDDED_LINE_WIDTH 16384
#define VEYE_MV_NUM_EMBEDDED_LINES 1

enum pad_types {
	IMAGE_PAD,
//	METADATA_PAD,//reserved
	NUM_PADS
};

static const struct of_device_id veye_mvcam_of_match[] = {
	{ .compatible = "veye,mvcam",},
	{ },
};
MODULE_DEVICE_TABLE(of, veye_mvcam_of_match);

//#define DEBUG_PRINTK
#ifndef DEBUG_PRINTK
static int debug = 0;
#define debug_printk(s , ... )
#define VEYE_TRACE 
#else
static int debug = 1;
#define debug_printk printk
#define VEYE_TRACE printk("[TRACE]%s %d \n",__FUNCTION__,__LINE__);
#endif

module_param(debug, int, 0644);

#define STARTUP_MIN_DELAY_US	500*1000//500ms
#define STARTUP_DELAY_RANGE_US	1000

struct reg_mv {
	u16 addr;
	u32 val;
};

struct mvcam_reg_list {
	unsigned int num_of_regs;
	const struct reg_mv *regs;
};

struct mvcam_format {
	u32 index;
	u32 mbus_code;//mbus format
	u32 data_type;//mv data format
};
struct mvcam_roi
{
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
};

struct mvcam {
	struct v4l2_subdev *subdev;
	struct media_pad pad[NUM_PADS];
    struct i2c_client *client;
    u32    model_id; 
    
    //data format 
	struct mvcam_format *supported_formats;
	int num_supported_formats;
	int current_format_idx;
    
    struct camera_common_data	*s_data;
    u32 reset_gpio;
    u32 max_width;
    u32 max_height;
    u32 min_width;
    u32 min_height;
    struct v4l2_rect roi;//the same as roi
    //max fps @ current roi format
    u32 max_fps;
    u32 cur_fps;
    u32 h_flip;
    u32 v_flip;
    
    u32 lane_num;
    u32 mipi_datarate;
    
	struct v4l2_ctrl_handler ctrl_handler;
    struct v4l2_ctrl *ctrls[MVCAM_MAX_CTRLS];
	/* V4L2 Controls */
    struct v4l2_ctrl *frmrate;

	/* Streaming on/off */
	bool streaming;
};

static struct mvcam *to_mvcam(struct v4l2_subdev *sd)
{
	struct i2c_client *client;
	struct camera_common_data *s_data;

	if (sd == NULL)
		return NULL;

	client = v4l2_get_subdevdata(sd);
	if (client == NULL)
		return NULL;

	s_data = to_camera_common_data(&client->dev);
	if (s_data == NULL)
		return NULL;

	return (struct mvcam *)s_data->priv;
}

static int mvcam_readl_reg(struct i2c_client *client,
								   u16 addr, u32 *val)
{
    u16 buf = htons(addr);
    u32 data;
    struct i2c_msg msgs[2] = {
		{
			.addr = client->addr,
			.flags= 0,
			.len = 2,
			.buf = (u8 *)&buf,
		},
		{
			.addr = client->addr,
			.flags= I2C_M_RD,
			.len = 4,
			.buf = (u8 *)&data,
		},
	};

	if(i2c_transfer(client->adapter, msgs, 2) != 2){
		return -1;
	}

	*val = ntohl(data);

	return 0;
}

static int mvcam_writel_reg(struct i2c_client *client,
									u16 addr, u32 val)
{
	u8 data[6];
	struct i2c_msg msgs[2] = {
		{
			.addr = client->addr,
			.flags= 0,
			.len = 6,
			.buf = data,
		},
	};
    debug_printk("mvcam write 0x%x val 0x%x\n",addr,val);
	addr = htons(addr);
	val = htonl(val);
	memcpy(data, &addr, 2);
	memcpy(data + 2, &val, 4);    
	if(i2c_transfer(client->adapter, msgs, 1) != 1)
		return -1;

	return 0;
}

static int mvcam_read(struct i2c_client *client, u16 addr, u32 *value)
{
	int ret;
	int count = 0;
	while (count++ < I2C_READ_RETRY_COUNT) {
		ret = mvcam_readl_reg(client, addr, value);
		if(!ret) {
			//v4l2_dbg(1, debug, client, "%s: 0x%02x 0x%04x\n",
			//	__func__, addr, *value);
			return ret;
		}
	}
    
	v4l2_err(client, "%s: Reading register 0x%02x failed\n",
			 __func__, addr);
	return ret;
}

static int mvcam_write(struct i2c_client *client, u16 addr, u32 value)
{
	int ret;
	int count = 0;
	while (count++ < I2C_WRITE_RETRY_COUNT) {
		ret = mvcam_writel_reg(client, addr, value);
		if(!ret)
			return ret;
	}
	v4l2_err(client, "%s: Write 0x%04x to register 0x%02x failed\n",
			 __func__, value, addr);
	return ret;
}

/* Write a list of registers */
static int __maybe_unused  mvcam_write_regs(struct i2c_client *client,
			     const struct reg_mv *regs, u32 len)
{
	unsigned int i;
	int ret;

	for (i = 0; i < len; i++) {
		ret = mvcam_write(client, regs[i].addr,regs[i].val);
		if (ret) {
			dev_err_ratelimited(&client->dev,
					    "Failed to write reg 0x%4.4x. error = %d\n",
					    regs[i].addr, ret);

			return ret;
		}
	}
	return 0;
}

static u32 bit_count(u32 n)
{
    n = (n &0x55555555) + ((n >>1) &0x55555555) ;
    n = (n &0x33333333) + ((n >>2) &0x33333333) ;
    n = (n &0x0f0f0f0f) + ((n >>4) &0x0f0f0f0f) ;
    n = (n &0x00ff00ff) + ((n >>8) &0x00ff00ff) ;
    n = (n &0x0000ffff) + ((n >>16) &0x0000ffff) ;

    return n ;
}

static int mvcam_getroi(struct mvcam *mvcam)
{
  //  int ret;
    struct i2c_client *client = mvcam->client;
    mvcam_read(client, ROI_Offset_X,&mvcam->roi.left);
    mvcam_read(client, ROI_Offset_Y,&mvcam->roi.top);
    mvcam_read(client, ROI_Width,&mvcam->roi.width);
    mvcam_read(client, ROI_Height,&mvcam->roi.height);
    v4l2_dbg(1, debug, mvcam->client, "%s:get roi(%d,%d,%d,%d)\n",
			 __func__, mvcam->roi.left,mvcam->roi.top,mvcam->roi.width,mvcam->roi.height);
    return 0;
}

static int mvcam_setroi(struct mvcam *mvcam)
{
  //  int ret;
    u32 fps_reg;
    struct i2c_client *client = mvcam->client;
    v4l2_dbg(1, debug, mvcam->client, "%s:set roi(%d,%d,%d,%d)\n",
			 __func__, mvcam->roi.left,mvcam->roi.top,mvcam->roi.width,mvcam->roi.height);
    mvcam_write(client, ROI_Offset_X,mvcam->roi.left);
    msleep(1);
    mvcam_write(client, ROI_Offset_Y,mvcam->roi.top);
    msleep(1);
    mvcam_write(client, ROI_Width,mvcam->roi.width);
    msleep(1);
    mvcam_write(client, ROI_Height,mvcam->roi.height);
    msleep(8);
    //get sensor max framerate 
    mvcam_read(client, MaxFrame_Rate,&fps_reg);
    mvcam->max_fps = fps_reg/100;
    mvcam_read(client, Framerate,&fps_reg);
    mvcam->cur_fps = fps_reg/100;
    v4l2_ctrl_modify_range(mvcam->frmrate, 1, mvcam->max_fps, 1, mvcam->cur_fps);
    
//    dev_info(&client->dev,
//			 "max fps is %d,cur fps %d\n",
//			 mvcam->max_fps,mvcam->cur_fps);
    return 0;
}

static int mvcam_g_volatile_ctrl(struct v4l2_ctrl *ctrl)
{
	int ret;
    struct mvcam *mvcam = 
		container_of(ctrl->handler, struct mvcam, ctrl_handler);
    struct i2c_client *client = mvcam->client;
    
	switch (ctrl->id) {
	case V4L2_CID_VEYE_MV_TRIGGER_MODE:
        ret = mvcam_read(client, Trigger_Mode,&ctrl->val);
		break;
	case V4L2_CID_VEYE_MV_TRIGGER_SRC:
        ret = mvcam_read(client, Trigger_Source,&ctrl->val);
		break;

	case V4L2_CID_VEYE_MV_FRAME_RATE:
        ret = mvcam_read(client, Framerate,&ctrl->val);
        ctrl->val = ctrl->val/100;
        mvcam->cur_fps = ctrl->val;
		break;
	default:
		dev_info(&client->dev,
			 "mvcam_g_volatile_ctrl ctrl(id:0x%x,val:0x%x) is not handled\n",
			 ctrl->id, ctrl->val);
		ret = -EINVAL;
		break;
	}
    
    v4l2_dbg(1, debug, mvcam->client, "%s: cid = (0x%X), value = (%d).\n",
                     __func__, ctrl->id, ctrl->val);

	return ret;
}

static int mvcam_s_ctrl(struct v4l2_ctrl *ctrl)
{
	int ret;
	struct mvcam *mvcam = 
		container_of(ctrl->handler, struct mvcam, ctrl_handler);
    struct i2c_client *client = mvcam->client;
    
	v4l2_dbg(1, debug, mvcam->client, "%s: cid = (0x%X), value = (%d).\n",
			 __func__, ctrl->id, ctrl->val);
	
    switch (ctrl->id) {
	case V4L2_CID_VEYE_MV_TRIGGER_MODE:
        ret = mvcam_write(client, Trigger_Mode,ctrl->val);
		break;
	case V4L2_CID_VEYE_MV_TRIGGER_SRC:
        ret = mvcam_write(client, Trigger_Source,ctrl->val);
		break;
	case V4L2_CID_VEYE_MV_SOFT_TRGONE:
        ret = mvcam_write(client, Trigger_Software,1);
		break;
	case V4L2_CID_VEYE_MV_FRAME_RATE:
        ret = mvcam_write(client, Framerate,ctrl->val*100);
        mvcam->cur_fps = ctrl->val;
		break;
    case V4L2_CID_VEYE_MV_ROI_X:
        mvcam->roi.left = rounddown(ctrl->val, MV_CAM_ROI_W_ALIGN);
        v4l2_dbg(1, debug, mvcam->client, "set roi_x %d round to %d.\n",
			 ctrl->val, mvcam->roi.left);
        ret = 0;
		break;
    case V4L2_CID_VEYE_MV_ROI_Y:
        mvcam->roi.top = rounddown(ctrl->val, MV_CAM_ROI_H_ALIGN);
        v4l2_dbg(1, debug, mvcam->client, "set roi_y %d round to %d.\n",
			 ctrl->val, mvcam->roi.top);
        ret = 0;
		break;
	default:
		dev_info(&client->dev,
			 "ctrl(id:0x%x,val:0x%x) is not handled\n",
			 ctrl->id, ctrl->val);
		ret = -EINVAL;
		break;
	}

	return ret;
}


static const struct v4l2_ctrl_ops mvcam_ctrl_ops = {
    .g_volatile_ctrl = mvcam_g_volatile_ctrl,
	.s_ctrl = mvcam_s_ctrl,
};

static struct v4l2_ctrl_config mvcam_v4l2_ctrls[] = {
    //standard v4l2_ctrls
	{
		.ops = &mvcam_ctrl_ops,
		.id = V4L2_CID_PIXEL_RATE,
		.name = NULL,//kernel fill fill it
		.type = V4L2_CTRL_TYPE_INTEGER,
		.def = MV_CAM_PIXEL_RATE,
		.min = MV_CAM_PIXEL_RATE,
		.max = MV_CAM_PIXEL_RATE,
		.step = 1,
		.flags = V4L2_CTRL_FLAG_READ_ONLY,
	},
	//custom v4l2-ctrls
	{
		.ops = &mvcam_ctrl_ops,
		.id = V4L2_CID_VEYE_MV_TRIGGER_MODE,
		.name = "trigger_mode",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.def = Image_Continues,
		.min = 0,
		.max = Image_trigger_mode_num-1,
		.step = 1,
		.flags = V4L2_CTRL_FLAG_VOLATILE|V4L2_CTRL_FLAG_EXECUTE_ON_WRITE,
	},
	{
		.ops = &mvcam_ctrl_ops,
		.id = V4L2_CID_VEYE_MV_TRIGGER_SRC,
		.name = "trigger_src",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.def = Trg_Hard,
		.min = 0,
		.max = Trg_Hard_src_num-1,
		.step = 1,
		.flags = V4L2_CTRL_FLAG_VOLATILE|V4L2_CTRL_FLAG_EXECUTE_ON_WRITE,
	},
	{
		.ops = &mvcam_ctrl_ops,
		.id = V4L2_CID_VEYE_MV_SOFT_TRGONE,
		.name = "soft_trgone",
		.type = V4L2_CTRL_TYPE_BUTTON,
		.def = 0,
		.min = 0,
		.max = 0,
		.step = 0,
	},
	{
		.ops = &mvcam_ctrl_ops,
		.id = V4L2_CID_VEYE_MV_FRAME_RATE,
		.name = "frame_rate",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.def = MV_CAM_DEF_FPS,
		.min = 0,
		.max = MV_CAM_DEF_FPS,
		.step = 1,
		.flags = V4L2_CTRL_FLAG_VOLATILE|V4L2_CTRL_FLAG_EXECUTE_ON_WRITE,
	},
	{
		.ops = &mvcam_ctrl_ops,
		.id = V4L2_CID_VEYE_MV_ROI_X,
		.name = "roi_x",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.def = 0,
		.min = 0,
		.max = 0,//to read from camera
		.step = MV_CAM_ROI_W_ALIGN,
		.flags = 0,
	},
	{
		.ops = &mvcam_ctrl_ops,
		.id = V4L2_CID_VEYE_MV_ROI_Y,
		.name = "roi_y",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.def = 0,
		.min = 0,
		.max = 0,//to read from camera
		.step = MV_CAM_ROI_H_ALIGN,
		.flags = 0,
	},
};
//grab some ctrls while streaming
static void mvcam_v4l2_ctrl_grab(struct mvcam *mvcam,bool grabbed)
{
    int i = 0;
    for (i = 0; i < ARRAY_SIZE(mvcam_v4l2_ctrls); ++i) {
		switch(mvcam->ctrls[i]->id)
        {
            case V4L2_CID_VEYE_MV_TRIGGER_MODE:
            case V4L2_CID_VEYE_MV_TRIGGER_SRC:
            case V4L2_CID_VEYE_MV_FRAME_RATE:
            case V4L2_CID_VEYE_MV_ROI_X:
            case V4L2_CID_VEYE_MV_ROI_Y:
                v4l2_ctrl_grab(mvcam->ctrls[i], grabbed);
            break;

            default:
            break;
        }
	}
}

static void mvcam_v4l2_ctrl_init(struct mvcam *mvcam)
{
    int i = 0;
    u32 value = 0;
    struct i2c_client *client = mvcam->client;
    for (i = 0; i < ARRAY_SIZE(mvcam_v4l2_ctrls); ++i) {
		switch(mvcam_v4l2_ctrls[i].id)
        {
            case V4L2_CID_VEYE_MV_TRIGGER_MODE:
                mvcam_read(client, Trigger_Mode,&value);
                mvcam_v4l2_ctrls[i].def = value;
                v4l2_dbg(1, debug, mvcam->client, "%s:default trigger mode %d\n", __func__, value);
            break;
            case V4L2_CID_VEYE_MV_TRIGGER_SRC:
                mvcam_read(client, Trigger_Source,&value);
                mvcam_v4l2_ctrls[i].def = value;
                v4l2_dbg(1, debug, mvcam->client, "%s:default trigger source %d\n", __func__, value);
            break;
            case V4L2_CID_VEYE_MV_FRAME_RATE:
                mvcam_read(client, Framerate,&value);
                mvcam_v4l2_ctrls[i].def = value/100;
                mvcam_read(client, MaxFrame_Rate,&value);
                mvcam_v4l2_ctrls[i].max = value/100;
                v4l2_dbg(1, debug, mvcam->client, "%s:default framerate %lld , max fps %lld \n", __func__, \
                    mvcam_v4l2_ctrls[i].def,mvcam_v4l2_ctrls[i].max);
            break;
            case V4L2_CID_VEYE_MV_ROI_X:
                //mvcam_read(client, ROI_Offset_X,value);
                //mvcam_v4l2_ctrls[i].def = value;
                mvcam_v4l2_ctrls[i].max = mvcam->max_width - mvcam->min_width;
            break;
            case V4L2_CID_VEYE_MV_ROI_Y:
                //mvcam_read(client, ROI_Offset_Y,value);
                //mvcam_v4l2_ctrls[i].def = value;
                mvcam_v4l2_ctrls[i].max = mvcam->max_height - mvcam->min_height;
            break;
            default:
            break;
        }
	}
}


#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
int mvcam_g_mbus_config(struct v4l2_subdev *sd,
				struct v4l2_mbus_config *cfg)
#else
int mvcam_get_mbus_config(struct v4l2_subdev *sd,
				unsigned int pad,
				struct v4l2_mbus_config *cfg)
#endif
{
    struct mvcam *mvcam = to_mvcam(sd);
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 4, 0)
	cfg->type = V4L2_MBUS_CSI2;
#else
	/*
	 * TODO Bug 200664694: If the sensor type is CPHY
	 *  then return an error
	 */
	cfg->type = V4L2_MBUS_CSI2_DPHY;
#endif
	cfg->flags = V4L2_MBUS_CSI2_NONCONTINUOUS_CLOCK;
    if(mvcam->lane_num == 4){
        cfg->flags |= V4L2_MBUS_CSI2_4_LANE; /* XXX wierd */
    }else{
        cfg->flags |= V4L2_MBUS_CSI2_2_LANE; /* XXX wierd */
    }
    debug_printk("mvcam_get_mbus_config lane num %d\n",mvcam->lane_num);
	return 0;
}

static int mvcam_csi2_enum_mbus_code(
			struct v4l2_subdev *sd,
			struct v4l2_subdev_pad_config *cfg,
			struct v4l2_subdev_mbus_code_enum *code)
{
	struct mvcam *mvcam = to_mvcam(sd);
	struct mvcam_format *supported_formats = mvcam->supported_formats;
	int num_supported_formats = mvcam->num_supported_formats;
	VEYE_TRACE

	if (code->index >= num_supported_formats)
		return -EINVAL;
	code->code = supported_formats[code->index].mbus_code;
    v4l2_dbg(1, debug, sd, "%s: index = (%d) mbus code (%x)\n", __func__, code->index,code->code);

	return 0;
}

static int mvcam_csi2_enum_framesizes(
			struct v4l2_subdev *sd,
			struct v4l2_subdev_pad_config *cfg,
			struct v4l2_subdev_frame_size_enum *fse)
{
	struct mvcam *mvcam = to_mvcam(sd);
    VEYE_TRACE

    if (fse->index != 0)
		return -EINVAL;
	fse->min_width = fse->max_width =
		mvcam->roi.width;
	fse->min_height = fse->max_height =
		mvcam->roi.height;
    v4l2_dbg(1, debug, sd, "%s: code = (0x%X), index = (%d) frm width %d height %d\n",
		 __func__, fse->code, fse->index,fse->max_width,fse->max_height);
	return 0;
}


static int mvcam_csi2_get_fmt(struct v4l2_subdev *sd,
								struct v4l2_subdev_pad_config *cfg,
								struct v4l2_subdev_format *format)
{
	struct mvcam *mvcam = to_mvcam(sd);
	struct mvcam_format *current_format;
	VEYE_TRACE

	current_format = &mvcam->supported_formats[mvcam->current_format_idx];
	format->format.width = mvcam->roi.width;
	format->format.height = mvcam->roi.height;
    
	format->format.code = current_format->mbus_code;
	format->format.field = V4L2_FIELD_NONE;
	format->format.colorspace = V4L2_COLORSPACE_SRGB;
	v4l2_dbg(1, debug, sd, "%s: width: (%d) height: (%d) code: (0x%X)\n",
		__func__, format->format.width,format->format.height,
			format->format.code);
	return 0;
}

static int mvcam_csi2_get_fmt_idx_by_code(struct mvcam *mvcam,
											u32 mbus_code)
{
	int i;
	struct mvcam_format *formats = mvcam->supported_formats;
	for (i = 0; i < mvcam->num_supported_formats; i++) {
		if (formats[i].mbus_code == mbus_code)
			return i; 
	}
	return -EINVAL;
}

static int mvcam_get_selection(struct v4l2_subdev *sd,
				struct v4l2_subdev_pad_config *cfg,
				struct v4l2_subdev_selection *sel)
{
	struct mvcam *mvcam = to_mvcam(sd);
    VEYE_TRACE
	switch (sel->target) {
	case V4L2_SEL_TGT_CROP: {
    		//sel->r = *__mvcam_get_pad_crop(mvcam, cfg, sel->pad,
    		//				sel->which);
            sel->r = mvcam->roi;
    		break;
	    }        
        //active area
        case V4L2_SEL_TGT_CROP_DEFAULT:
        case V4L2_SEL_TGT_NATIVE_SIZE:
        case V4L2_SEL_TGT_CROP_BOUNDS:
            sel->r.top = 0;
            sel->r.left = 0;
            sel->r.width = mvcam->max_width;
            sel->r.height = mvcam->max_height;
		break;
        default:
		return -EINVAL;
	}
    sel->flags = V4L2_SEL_FLAG_LE;
    v4l2_dbg(1, debug, sd, "%s: target %d\n", __func__,V4L2_SEL_TGT_CROP);
    return 0;
}
static int mvcam_set_selection(struct v4l2_subdev *sd,
		struct v4l2_subdev_pad_config *cfg,
		struct v4l2_subdev_selection *sel)
{
   //     struct i2c_client *client = v4l2_get_subdevdata(sd);
        struct mvcam *mvcam = to_mvcam(sd);
    
        switch (sel->target) {
        case V4L2_SEL_TGT_CROP:
            mvcam->roi.left  = clamp(rounddown(sel->r.left, MV_CAM_ROI_W_ALIGN), 0U, (mvcam->max_width-mvcam->min_width));
            mvcam->roi.top  = clamp(rounddown(sel->r.top, MV_CAM_ROI_H_ALIGN), 0U, (mvcam->max_height-mvcam->min_height));
            mvcam->roi.width = clamp(rounddown(sel->r.width, MV_CAM_ROI_W_ALIGN), mvcam->min_width, mvcam->max_width);
            mvcam->roi.height = clamp(rounddown(sel->r.height, MV_CAM_ROI_H_ALIGN), mvcam->min_height, mvcam->max_height);
            mvcam_setroi(mvcam);
    
            break;
        default:
            return -EINVAL;
        }
        v4l2_dbg(1, debug, sd, "%s: target %d\n", __func__,V4L2_SEL_TGT_CROP);
        return 0;
}
static int mvcam_frm_supported(int roi_x,int wmin, int wmax, int ws,
				int roi_y,int hmin, int hmax, int hs,
				int w, int h)
{
	if (
		(roi_x+w) > wmax || w < wmin ||
		(roi_y+h) > hmax || h < hmin ||
		(h) % hs != 0 ||
		(w) % ws != 0
	)
		return -EINVAL;

	return 0;
}

static int mvcam_csi2_try_fmt(struct v4l2_subdev *sd,
		struct v4l2_subdev_pad_config *cfg,
		struct v4l2_subdev_format *format)
{
	struct mvcam *mvcam = to_mvcam(sd);
	int ret = 0;

	ret = mvcam_frm_supported(
			mvcam->roi.left,mvcam->min_width, mvcam->max_width, MV_CAM_ROI_W_ALIGN,
			mvcam->roi.top,mvcam->min_height, mvcam->max_height, MV_CAM_ROI_H_ALIGN,
			format->format.width, format->format.height);

	if (ret < 0) {
		v4l2_err(sd, "Not supported size!\n");
		return ret;
	}

	return 0;
}

static int mvcam_csi2_set_fmt(struct v4l2_subdev *sd,
								struct v4l2_subdev_pad_config *cfg,
								struct v4l2_subdev_format *format)
{
	int i;
	struct mvcam *mvcam = to_mvcam(sd);
//    struct v4l2_mbus_framefmt *framefmt;
    struct v4l2_subdev_selection sel;
    VEYE_TRACE
  /*  if ((format->format.width != mvcam->roi.width ||
	 format->format.height != mvcam->roi.height))
	{
		v4l2_info(sd, "Changing the resolution is not supported with VIDIOC_S_FMT! \n Pls use VIDIOC_S_SELECTION.\n");
        v4l2_info(sd,"%d,%d,%d,%d\n",format->format.width,mvcam->roi.width,format->format.height,mvcam->roi.height);
         return -EINVAL;
	}*/

	format->format.colorspace =  V4L2_COLORSPACE_SRGB;
	format->format.field = V4L2_FIELD_NONE;

	v4l2_dbg(1, debug, sd, "%s: code: 0x%X",
			__func__, format->format.code);
    if (format->which == V4L2_SUBDEV_FORMAT_TRY) {
                //framefmt = v4l2_subdev_get_try_format(sd, cfg,
                //                      format->pad);
               // *framefmt = format->format;
               v4l2_info(sd, "csi2_try format\n");
                return mvcam_csi2_try_fmt(sd, cfg, format);
     } else {
		i = mvcam_csi2_get_fmt_idx_by_code(mvcam, format->format.code);
		if (i < 0){
            v4l2_info(sd, "mvcam_csi2_get_fmt_idx_by_code failed\n");
			return -EINVAL;
		}
        v4l2_info(sd, "mvcam_csi2_get_fmt_idx_by_code success\n");
        mvcam->current_format_idx = i;
        mvcam_write(mvcam->client,Pixel_Format,mvcam->supported_formats[i].data_type);

        mvcam->roi.width = format->format.width;
        mvcam->roi.height = format->format.height;
        
        sel.target = V4L2_SEL_TGT_CROP;
    	sel.r = mvcam->roi;
        mvcam_set_selection(sd, NULL, &sel);

        //format->format.width = mvcam->roi.width;
    }
	//update_controls(mvcam);
	
	return 0;
}

static int mvcam_g_input_status(struct v4l2_subdev *sd, u32 *status)
{
    VEYE_TRACE
	*status = 1;
	return 0;
}


static void mvcam_free_controls(struct mvcam *mvcam)
{
	v4l2_ctrl_handler_free(mvcam->subdev->ctrl_handler);
}

static int mvdatatype_to_mbus_code(int data_type)
{
   // debug_printk("%s: data type %d\n",
	//				__func__, data_type);
    switch(data_type) {
	case MV_DT_Mono8:
        return MEDIA_BUS_FMT_Y8_1X8;
	case MV_DT_Mono10:
		return MEDIA_BUS_FMT_Y10_1X10;
	case MV_DT_Mono12:
		return MEDIA_BUS_FMT_Y12_1X12;
  //  case MV_DT_Mono14:
	//	return MEDIA_BUS_FMT_Y14_1X14;
	case MV_DT_UYVY:
		//return MEDIA_BUS_FMT_UYVY8_2X8;//0x2006
		return MEDIA_BUS_FMT_UYVY8_1X16;//0x200f
	}
    //not supported
	return -1;
}

int get_fmt_index(struct mvcam *mvcam,u32 datatype)
{
    int i = 0;
    for(;i < mvcam->num_supported_formats;++i)
    {
        if((mvcam->supported_formats[i].data_type) == datatype)
            return i;
    }
    return -1;
}

static int mvcam_enum_pixformat(struct mvcam *mvcam)
{
	int ret = 0;
	u32 mbus_code = 0;
	int pixformat_type;
	int index = 0;
    int bitindex = 0;
	int num_pixformat = 0;
    u32 fmtcap = 0;
    u32 cur_fmt;
	struct i2c_client *client = mvcam->client;
    
    ret = mvcam_read(client, Format_cap, &fmtcap);
    if (ret < 0)
		goto err;
	num_pixformat = bit_count(fmtcap);
	if (num_pixformat < 0)
		goto err;
    
    v4l2_dbg(1, debug, mvcam->client, "%s: format count: %d; format cap 0x%x\n",
					__func__, num_pixformat,fmtcap);
    
	mvcam->supported_formats = devm_kzalloc(&client->dev,
		sizeof(struct mvcam_format) * (num_pixformat+1), GFP_KERNEL);
	while(fmtcap){
        if(fmtcap&1){
            //which bit is set?
            pixformat_type = bitindex;
            fmtcap >>= 1;
            bitindex++;
        }
        else{
            fmtcap >>= 1;
            bitindex++;
            continue;
        }
        mbus_code = mvdatatype_to_mbus_code(pixformat_type);
        if(mbus_code > 0){
    		mvcam->supported_formats[index].index = index;
    		mvcam->supported_formats[index].mbus_code = mbus_code;
    		mvcam->supported_formats[index].data_type = pixformat_type;
            v4l2_dbg(1, debug, mvcam->client, "%s support format index %d mbuscode 0x%x datatype: %d\n",
    					__func__, index,mbus_code,pixformat_type);
            index++;
        }
	}
	mvcam->num_supported_formats = num_pixformat;

    mvcam_read(client, Pixel_Format, &cur_fmt);
	mvcam->current_format_idx = get_fmt_index(mvcam,cur_fmt);
    v4l2_dbg(1, debug, mvcam->client, "%s: cur format: %d\n",
					__func__, cur_fmt);
	// mvcam_add_extension_pixformat(mvcam);
	return 0;

err:
	return -ENODEV;
}

static void mvcam_get_mipifeature(struct mvcam *mvcam)
{
    u32 lane_num;
    u32 mipi_datarate;
    struct i2c_client *client = mvcam->client;
    mvcam_read(client, Lane_Num, &lane_num);
    if(lane_num == 4){
        mvcam->lane_num = 4;
    }else{
        mvcam->lane_num = 2;
    }
    
    mvcam_read(client, MIPI_DataRate, &mipi_datarate);
    if(mipi_datarate == 0xFFFFFFFF)
        mipi_datarate = MVCAM_DEFAULT_LINK_FREQ;
    else
        mipi_datarate *=1000;//register value is kbps
    
    mvcam->mipi_datarate = mipi_datarate;

    debug_printk("%s: lane num %d, datarate %d bps\n",
					__func__, mvcam->lane_num,mvcam->mipi_datarate);
    return;
}

/* Start streaming */
static int mvcam_start_streaming(struct mvcam *mvcam)
{
	struct i2c_client *client = mvcam->client;
	int ret;
    
	/* Apply customized values from user */
 //   ret =  __v4l2_ctrl_handler_setup(mvcam->sd.ctrl_handler);
    debug_printk("mvcam_start_streaming \n");
	/* set stream on register */
    ret = mvcam_write(client, Image_Acquisition,1);
	if (ret)
		return ret;

	/* some v4l2 ctrls cannot change during streaming */
    mvcam_v4l2_ctrl_grab(mvcam,true);
	return ret;
}

/* Stop streaming */
static int mvcam_stop_streaming(struct mvcam *mvcam)
{
	struct i2c_client *client = mvcam->client;
	int ret;

	/* set stream off register */
    ret = mvcam_write(client, Image_Acquisition,0);
	if (ret)
		dev_err(&client->dev, "%s failed to set stream\n", __func__);
    debug_printk("mvcam_stop_streaming \n");
    
   	 mvcam_v4l2_ctrl_grab(mvcam,false);

	/*
	 * Return success even if it was an error, as there is nothing the
	 * caller can do about it.
	 */
	return 0;
}


static int mvcam_set_stream(struct v4l2_subdev *sd, int enable)
{
	struct mvcam *mvcam = to_mvcam(sd);
    struct i2c_client *client = mvcam->client;
	int ret = 0;
	enable = !!enable;
	
	if (mvcam->streaming == enable) {
        dev_info(&client->dev, "%s already streamed!\n", __func__);
		return 0;
	}
VEYE_TRACE
	if (enable) {

		/*
		 * Apply default & customized values
		 * and then start streaming.
		 */
		ret = mvcam_start_streaming(mvcam);
		if (ret)
			goto end;
	} else {
		mvcam_stop_streaming(mvcam);
	}
	mvcam->streaming = enable;

	return ret;
end:
	return ret;
}


/* Power management functions */
static int mvcam_power_on(struct mvcam *mvcam)
{
     if(mvcam){
        debug_printk("mvcam_power_on\n");
	    gpio_set_value(mvcam->reset_gpio, 1);
    }else{
        debug_printk("mvcam_power_on mvcam is NULL\n");
    }
	usleep_range(STARTUP_MIN_DELAY_US,
		     STARTUP_MIN_DELAY_US + STARTUP_DELAY_RANGE_US);
	return 0;
}

static int mvcam_power_off(struct mvcam *mvcam)
{
    //do not really power off, because we might use i2c script at any time
    if(mvcam){
        debug_printk("mvcam_power_off\n");
	    gpio_set_value(mvcam->reset_gpio, 1);
    }else{
        debug_printk("mvcam_power_off mvcam is NULL\n");
    }
	return 0;
}

int mvcam_s_power(struct v4l2_subdev *sd, int on)
{
    struct mvcam *mvcam = to_mvcam(sd);
	//struct camera_common_data *s_data = mvcam->s_data;
    //may be could delete this. 
    camera_common_s_power(sd,on);
	if (on) {
        mvcam_power_on(mvcam);
	} else {
		mvcam_power_off(mvcam);
	}
	return 0;
}


static int mvcam_csi2_enum_frameintervals(struct v4l2_subdev *sd,
				struct v4l2_subdev_pad_config *cfg,
				struct v4l2_subdev_frame_interval_enum *fie)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct camera_common_data *s_data = to_camera_common_data(&client->dev);
	struct mvcam *mvcam = (struct mvcam *)s_data->priv;
	struct v4l2_fract tpf;
    u32 fps_reg;
    VEYE_TRACE

	/* Only one frame rate should be returned
	 * - the current frame rate
	 */
	if(fie->index > 0)
		return -EINVAL;

    mvcam_read(client, Framerate,&fps_reg);
    mvcam->cur_fps = fps_reg/100;

	/* Translate frequency to timeperframe
	* by inverting the fraction
	*/
	tpf.numerator = 1;
	tpf.denominator = mvcam->cur_fps;

    v4l2_dbg(1, debug, sd, "%s: code = (0x%X), index = (%d), fps=(%d)\n",
                 __func__, fie->code, fie->index,mvcam->cur_fps);
    
	fie->interval = tpf;

	return 0;
}
static int mvcam_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
	struct mvcam *mvcam = to_mvcam(sd);
	struct v4l2_mbus_framefmt *try_fmt =
		v4l2_subdev_get_try_format(sd, fh->pad, IMAGE_PAD);
    
//	struct v4l2_mbus_framefmt *try_fmt_meta =
//		v4l2_subdev_get_try_format(sd, fh->pad, METADATA_PAD);
    struct v4l2_rect *try_crop;
	/* Initialize try_fmt */
	try_fmt->width = mvcam->max_width;
	try_fmt->height = mvcam->max_height;
	try_fmt->code = mvcam->supported_formats[0].mbus_code;
	try_fmt->field = V4L2_FIELD_NONE;

	/* Initialize try_fmt for the embedded metadata pad */
/*	try_fmt_meta->width = MVCAM_EMBEDDED_LINE_WIDTH;
	try_fmt_meta->height = MVCAM_NUM_EMBEDDED_LINES;
	try_fmt_meta->code = MEDIA_BUS_FMT_SENSOR_DATA;
	try_fmt_meta->field = V4L2_FIELD_NONE;
*/
    /* Initialize try_crop rectangle. */
	try_crop = v4l2_subdev_get_try_crop(sd, fh->pad, 0);
	try_crop->top = 0;
	try_crop->left = 0;
	try_crop->width = mvcam->max_width;
	try_crop->height = mvcam->max_height;
    
    
	return 0;
}
static const struct v4l2_subdev_core_ops mvcam_core_ops = {
	 .s_power = mvcam_s_power,  
};

static const struct v4l2_subdev_video_ops mvcam_video_ops = {
	.s_stream = mvcam_set_stream,
    #if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
	.g_mbus_config	= mvcam_g_mbus_config,
    #endif
    .g_input_status = mvcam_g_input_status,
};

static const struct v4l2_subdev_pad_ops mvcam_pad_ops = {
	.enum_mbus_code = mvcam_csi2_enum_mbus_code,
	.get_fmt = mvcam_csi2_get_fmt,
	.set_fmt = mvcam_csi2_set_fmt,
	.enum_frame_size = mvcam_csi2_enum_framesizes,
    //v4.9 kernel can not support selection correctly
	.get_selection = mvcam_get_selection,
	.set_selection = mvcam_set_selection,
    #if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
	.get_mbus_config	= mvcam_get_mbus_config,
    #endif
	.enum_frame_interval = mvcam_csi2_enum_frameintervals,
};

static const struct v4l2_subdev_ops mvcam_subdev_ops = {
	.core = &mvcam_core_ops,
	.video = &mvcam_video_ops,
	.pad = &mvcam_pad_ops,
};

static const struct v4l2_subdev_internal_ops mvcam_internal_ops = {
	.open = mvcam_open,
};

static const struct media_entity_operations mvcam_csi2_media_ops = {
	.link_validate = v4l2_subdev_link_validate,
};

const struct of_device_id mvcam_csi2_of_match[] = {
	{ .compatible = "nvidia,veye_mvcam",},
	{ },
};
MODULE_DEVICE_TABLE(of, mvcam_csi2_of_match);
static int mvcam_enum_controls(struct mvcam *mvcam)
{
    struct i2c_client *client = mvcam->client;
    struct v4l2_ctrl_handler *ctrl_hdlr;
    int ret;
    int i;
    struct v4l2_ctrl *ctrl;
    ctrl_hdlr = &mvcam->ctrl_handler;
    ret = v4l2_ctrl_handler_init(ctrl_hdlr, ARRAY_SIZE(mvcam_v4l2_ctrls));
    if (ret)
        return ret;
    
    for (i = 0; i < ARRAY_SIZE(mvcam_v4l2_ctrls); ++i) {
		ctrl = v4l2_ctrl_new_custom(
			ctrl_hdlr,
			&mvcam_v4l2_ctrls[i],
			NULL);
		if (ctrl == NULL) {
			dev_err(&client->dev, "Failed to init %d ctrl\n",i);
			continue;
		}
		mvcam->ctrls[i] = ctrl;
        if(mvcam->ctrls[i]->id == V4L2_CID_VEYE_MV_FRAME_RATE){
            mvcam->frmrate = mvcam->ctrls[i];
        }
	}

    
	if (ctrl_hdlr->error) {
		ret = ctrl_hdlr->error;
		dev_err(&client->dev, "%s control init failed (%d)\n",
			__func__, ret);
		goto error;
	}

	mvcam->subdev->ctrl_handler = ctrl_hdlr;
    v4l2_ctrl_handler_setup(ctrl_hdlr);
    
	return 0;

error:
	v4l2_ctrl_handler_free(ctrl_hdlr);

	return ret;

}

/* Verify chip ID */
static int mvcam_identify_module(struct mvcam * mvcam)
{
	int ret;
    u32 device_id;
	u32 firmware_version;
    struct i2c_client *client = v4l2_get_subdevdata(mvcam->subdev);
    
    ret = mvcam_read(client, Model_Name, &device_id);
    if (ret ) {
        dev_err(&client->dev, "failed to read chip id\n");
        ret = -ENODEV;
        return ret;
    }
    switch (device_id)
    {
        case MV_MIPI_IMX178M:
            mvcam->model_id = device_id;
            dev_info(&client->dev, "camera is: MV-MIPI-IMX178M\n");
            break; 
        case MV_MIPI_IMX296M:
            mvcam->model_id = device_id;
            dev_info(&client->dev, "camera is：MV-MIPI-IMX296M\n");
            break; 
        case MV_MIPI_SC130M:
            mvcam->model_id = device_id;
            dev_info(&client->dev, "camera is: MV-MIPI-SC130M\n");
            break; 
        case MV_MIPI_IMX265M:
            mvcam->model_id = device_id;
            dev_info(&client->dev, "camera is: MV-MIPI-IMX265M\n");
            break; 
        case MV_MIPI_IMX264M:
            mvcam->model_id = device_id;
            dev_info(&client->dev, "camera is: MV-MIPI-IMX264M\n");
            break;
        case RAW_MIPI_SC132M:
            mvcam->model_id = device_id;
            dev_info(&client->dev, "camera is: RAW-MIPI-SC132M\n");
            break;
        case MV_MIPI_IMX287M:
            mvcam->model_id = device_id;
            dev_info(&client->dev, "camera is: MV_MIPI_IMX287M\n");
            break;
        case RAW_MIPI_IMX462M:
            mvcam->model_id = device_id;
            dev_info(&client->dev, "camera is: RAW_MIPI_IMX462M\n");
            break;
        case RAW_MIPI_AR0234M:
            mvcam->model_id = device_id;
            dev_info(&client->dev, "camera is: RAW_MIPI_AR0234M\n");
            break;
        default:
            dev_err(&client->dev, "camera id do not support: %x \n",device_id);
		return -EIO;
    }
    
    ret = mvcam_read(client, Device_Version, &firmware_version);
    if (ret) {
        dev_err(&client->dev, "read firmware version failed\n");
    }
    dev_info(&client->dev, "firmware version: 0x%04X\n", firmware_version);
	return 0;
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
static int mvcam_check_hwcfg(struct device *dev,struct mvcam * mvcam)
{
	struct v4l2_of_endpoint *endpoint;
    struct device_node *ep;
    int gpio;
    
    ep = of_graph_get_next_endpoint(dev->of_node, NULL);
	if (!ep) {
		dev_err(dev, "missing endpoint node\n");
		return -EINVAL;
	}
    
	endpoint = v4l2_of_alloc_parse_endpoint(ep);
	if (IS_ERR(endpoint)) {
		dev_err(dev, "failed to parse endpoint\n");
		return PTR_ERR(endpoint);
	}
    
    
    gpio = of_get_named_gpio(dev->of_node, "reset-gpios", 0);
	if (gpio < 0) {
		dev_err(dev, "reset-gpios not found \n");
        mvcam->reset_gpio = 0;
	}else{
        mvcam->reset_gpio = (unsigned int)gpio;
    }
    
	return 0;
}
#else
static int mvcam_check_hwcfg(struct device *dev,struct mvcam * mvcam)
{
	struct fwnode_handle *endpoint;
	struct v4l2_fwnode_endpoint ep_cfg = {
		.bus_type = V4L2_MBUS_CSI2_DPHY
	};
	int ret = -EINVAL;
    int gpio;
	endpoint = fwnode_graph_get_next_endpoint(dev_fwnode(dev), NULL);
	if (!endpoint) {
		dev_err(dev, "endpoint node not found\n");
		return -EINVAL;
	}

	if (v4l2_fwnode_endpoint_alloc_parse(endpoint, &ep_cfg)) {
		dev_err(dev, "could not parse endpoint\n");
		goto error_out;
	}

	gpio = of_get_named_gpio(dev->of_node, "reset-gpios", 0);
	if (gpio < 0) {
		dev_err(dev, "reset-gpios not found \n");
        mvcam->reset_gpio = 0;
	}else{
        mvcam->reset_gpio = (unsigned int)gpio;
    }
	
	ret = 0;

error_out:
	v4l2_fwnode_endpoint_free(&ep_cfg);
	fwnode_handle_put(endpoint);

	return ret;
}
#endif
/*
static int mvcam_init_mode(struct v4l2_subdev *sd)
{
    struct mvcam *mvcam = to_mvcam(sd);
    struct i2c_client *client = mvcam->client;

    struct v4l2_subdev_selection sel;
    //stop acquitsition
    mvcam_write(client, Image_Acquisition,0);
    //set to video stream mode
	mvcam_write(client, Trigger_Mode,0);
    //set roi
    mvcam->roi.left = 0;
    mvcam->roi.top = 0;
    mvcam->roi.width = mvcam->max_width;
    mvcam->roi.height = mvcam->max_height;
    sel.target = V4L2_SEL_TGT_CROP;
	sel.r = mvcam->roi;
    mvcam_set_selection(sd, NULL, &sel);
    return 0;
}*/
static int mvcam_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct mvcam *mvcam;
    struct camera_common_data *common_data;
	int ret;
    
    v4l_dbg(1, debug, client, "chip found @ 0x%x (%s)\n",
		client->addr << 1, client->adapter->name);
	dev_info(dev, "veye mv series camera driver version: %02x.%02x.%02x\n",
		DRIVER_VERSION >> 16,
		(DRIVER_VERSION & 0xff00) >> 8,
		DRIVER_VERSION & 0x00ff);    
		
    common_data = devm_kzalloc(&client->dev,
			sizeof(struct camera_common_data), GFP_KERNEL);
	if (!common_data)
		return -ENOMEM;
	mvcam = devm_kzalloc(&client->dev, sizeof(struct mvcam), GFP_KERNEL);
	if (!mvcam)
		return -ENOMEM;
    
    mvcam->subdev = &common_data->subdev;
	mvcam->subdev->ctrl_handler = &mvcam->ctrl_handler;
	mvcam->client = client;
    mvcam->s_data = common_data;
    
    /* Check the hardware configuration in device tree */
	if(mvcam_check_hwcfg(dev,mvcam))
		return -EINVAL;

	/* Initialize subdev */
	v4l2_i2c_subdev_init(mvcam->subdev, client, &mvcam_subdev_ops);
    mvcam->subdev->internal_ops = &mvcam_internal_ops;
    mvcam->subdev->dev = &client->dev;
    mvcam->subdev->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
    /* Initialize subdev */
    /* Set owner to NULL so we can unload the driver module */
	mvcam->subdev->owner = NULL;
    
    common_data->priv = mvcam;
	common_data->dev = &client->dev;
	common_data->ctrl_handler = &mvcam->ctrl_handler;
	common_data->ctrls = mvcam->ctrls;
    
	ret = mvcam_power_on(mvcam);
	if (ret)
		return ret;
    ret = mvcam_identify_module(mvcam);
	if(ret){
        dev_err(dev, "mvcam_identify_module failed.\n");
		goto error_power_off;
	}
	if (mvcam_enum_pixformat(mvcam)) {
		dev_err(dev, "enum pixformat failed.\n");
		ret = -ENODEV;
		goto error_power_off;
	}
	mvcam_get_mipifeature(mvcam);
    mvcam_read(client, Sensor_Width, &mvcam->max_width);
    mvcam_read(client, Sensor_Height, &mvcam->max_height);
    if(mvcam->model_id == MV_MIPI_IMX178M){
        mvcam->min_width = MV_IMX178M_ROI_W_MIN;
        mvcam->min_height = MV_IMX178M_ROI_H_MIN;
    }else if(mvcam->model_id == MV_MIPI_SC130M){
        mvcam->min_width = MV_SC130M_ROI_W_MIN;
        mvcam->min_height = MV_SC130M_ROI_H_MIN;
    }else if(mvcam->model_id == MV_MIPI_IMX296M){
        mvcam->min_width = MV_IMX296M_ROI_W_MIN;
        mvcam->min_height = MV_IMX296M_ROI_H_MIN;
    }else if(mvcam->model_id == MV_MIPI_IMX265M){
        mvcam->min_width = MV_IMX265M_ROI_W_MIN;
        mvcam->min_height = MV_IMX265M_ROI_H_MIN;
    }else if(mvcam->model_id == MV_MIPI_IMX264M){
        mvcam->min_width = MV_IMX264M_ROI_W_MIN;
        mvcam->min_height = MV_IMX264M_ROI_H_MIN;
    }else if(mvcam->model_id == RAW_MIPI_SC132M){
        mvcam->min_width = RAW_SC132M_ROI_W_MIN;
        mvcam->min_height = RAW_SC132M_ROI_H_MIN;
    }else if(mvcam->model_id == MV_MIPI_IMX287M){
        mvcam->min_width = MV_IMX287M_ROI_W_MIN;
        mvcam->min_height = MV_IMX287M_ROI_H_MIN;
    }else if(mvcam->model_id == RAW_MIPI_IMX462M){
        mvcam->min_width = RAW_IMX462M_ROI_W_MIN;
        mvcam->min_height = RAW_IMX462M_ROI_H_MIN;
    }else if(mvcam->model_id == RAW_MIPI_AR0234M){
        mvcam->min_width = RAW_AR0234M_ROI_W_MIN;
        mvcam->min_height = RAW_AR0234M_ROI_H_MIN;
    }
    v4l2_dbg(1, debug, mvcam->client, "%s: max width %d; max height %d\n",
					__func__, mvcam->max_width,mvcam->max_height);
    //read roi
    mvcam_getroi(mvcam);
    
    mvcam_v4l2_ctrl_init(mvcam);
    
	if (mvcam_enum_controls(mvcam)) {
		dev_err(dev, "enum controls failed.\n");
		ret = -ENODEV;
		goto error_power_off;
	}
	
    //mvcam_init_mode(&mvcam->sd);
    //stop acquitsition
    mvcam_write(client, Image_Acquisition,0);
    
    //mvcam->subdev->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
    //mvcam->subdev->entity.function = MEDIA_ENT_F_CAM_SENSOR;
        /* Initialize source pad */
    //  mvcam->pad[METADATA_PAD].flags = MEDIA_PAD_FL_SOURCE;
    #if defined(CONFIG_MEDIA_CONTROLLER)
    mvcam->pad[IMAGE_PAD].flags = MEDIA_PAD_FL_SOURCE;
	mvcam->subdev->entity.ops = &mvcam_csi2_media_ops;
    ret = tegra_media_entity_init(&mvcam->subdev->entity, NUM_PADS,
				mvcam->pad, true, true);
	if (ret < 0){
        dev_err(&client->dev, "Failed at tegra_media_entity_init \n");
        goto error_handler_free;
	}
    #endif
    //use lesss for us, we do not need dts infomations here
	ret = camera_common_initialize(common_data, "veye_mvcam");
	if (ret) {
		dev_err(&client->dev, "Failed to initialize tegra common for mvcam.\n");
        goto error_media_entity;  
//		return ret;
	}
    
	ret = v4l2_async_register_subdev(mvcam->subdev);
	if (ret < 0){
        dev_err(&client->dev, "Failed at v4l2_async_register_subdev for mvcam.\n");
		goto error_media_entity;
	}

	return 0;

error_media_entity:
	media_entity_cleanup(&mvcam->subdev->entity);

error_handler_free:
	mvcam_free_controls(mvcam);

error_power_off:
	mvcam_power_off(mvcam);

	return ret;
}

static int mvcam_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);
	struct mvcam *mvcam = to_mvcam(sd);

	mvcam_free_controls(mvcam);
    
    v4l2_async_unregister_subdev(sd);
	v4l2_device_unregister_subdev(sd);
    #if defined(CONFIG_MEDIA_CONTROLLER)
	media_entity_cleanup(&sd->entity);
    #endif
	return 0;
}

static struct i2c_device_id veyemv_cam_dt_ids[] = {
	{"veye_mvcam", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, veyemv_cam_dt_ids);

static struct i2c_driver veyemv_cam_i2c_driver = {
	.driver = {
		.name = "mvcam",
        .owner = THIS_MODULE,
		.of_match_table	= veye_mvcam_of_match,
	},
	.probe = mvcam_probe,
	.remove = mvcam_remove,
    .id_table = veyemv_cam_dt_ids,
};

module_i2c_driver(veyemv_cam_i2c_driver);

MODULE_AUTHOR("xumm <www.veye.cc>");
MODULE_DESCRIPTION("VEYE MV series mipi camera v4l2 driver");
MODULE_LICENSE("GPL v2");

