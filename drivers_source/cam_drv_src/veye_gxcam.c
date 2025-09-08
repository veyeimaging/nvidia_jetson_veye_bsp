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
#include <media/v4l2-device.h>
#include <media/v4l2-event.h>
#include <linux/of_graph.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0)
#include <media/v4l2-fwnode.h>
#else
#include <media/v4l2-of.h>
#endif

#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>

#include <media/tegra-v4l2-camera.h>
#include <media/camera_common.h>
//#include <media/mc_common.h>

/*

v1.00.00
1. pre release version

*/

#define DRIVER_VERSION			KERNEL_VERSION(1, 0x00, 0x00) 
#include "veye_gxcam.h"

//reserved
/* Embedded metadata stream structure */
#define VEYE_GX_EMBEDDED_LINE_WIDTH 16384
#define VEYE_GX_NUM_EMBEDDED_LINES 1

enum pad_types {
	IMAGE_PAD,
//	METADATA_PAD,//reserved
	NUM_PADS
};

static const struct of_device_id veye_gxcam_of_match[] = {
	{ .compatible = "veye,gxcam",},
	{ },
};
MODULE_DEVICE_TABLE(of, veye_gxcam_of_match);

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

#define STARTUP_MIN_DELAY_US	50*1000//50ms
#define STARTUP_DELAY_RANGE_US	1000

#define WRITE_HEAD 0xAB
#define PRE_READ_HEAD 0xBC
#define POST_READ_HEAD 0xAC
#define DIR_READ_HEAD 0xDE

#pragma pack(push, 1)  
struct preread_regs {
	uint8_t	pre_head;
	uint16_t reg;
	uint8_t xor;
};
struct write_regs {
	uint8_t	head;
	uint16_t reg;
	uint32_t data;
	uint8_t xor;
};
#pragma pack(pop) 

struct reg_mv {
	u16 addr;
	u32 val;
};

struct gxcam_reg_list {
	unsigned int num_of_regs;
	const struct reg_mv *regs;
};

struct gxcam_format {
	u32 index;
	u32 mbus_code;//mbus format
	u32 data_type;//mv data format
};
struct gxcam_roi
{
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
};

static const s64 link_freq_menu_items[] = {
	GXCAM_DEFAULT_LINK_FREQ,
};
//used for gxcam->ctrls[i] index, this must obey the same order as gxcam_v4l2_ctrls 
enum enum_v4l2_ctrls_index{
    CID_LINK_FREQ,
    CID_PIXEL_RATE,
	CID_VEYE_GX_TRIGGER_MODE,
	CID_VEYE_GX_TRIGGER_SRC,
	CID_VEYE_GX_SOFT_TRGONE,
	CID_VEYE_GX_SYNC_ROLE,
	CID_VEYE_GX_FRAME_RATE,
	GXCAM_MAX_CTRLS,
};
struct gxcam {
	struct v4l2_subdev *subdev;
	struct media_pad pad[NUM_PADS];
	struct kobject kobj;
    struct i2c_client *client;
    u32    model_id; 
    
    //data format 
	struct gxcam_format *supported_formats;
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
	u32 lanecap;
    u32 mipi_datarate;
    u8 camera_model[32];
	u64 pixelrate;
	struct v4l2_ctrl_handler ctrl_handler;
    struct v4l2_ctrl *ctrls[GXCAM_MAX_CTRLS];
	/* V4L2 Controls */
 //   struct v4l2_ctrl *frmrate;

	/* Streaming on/off */
	bool streaming;
};

static struct gxcam *to_gxcam(struct v4l2_subdev *sd)
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

	return (struct gxcam *)s_data->priv;
}

static uint8_t xor8(const uint8_t *data, uint32_t len) 
{
    uint8_t checksum = 0;
    const uint8_t *p = data;
    while (len--) 
    {
        checksum ^= *p++;
    }
    return checksum;
}

//direct register access
static int gxcam_readl_d_reg(struct i2c_client *client,
								   u16 addr, u32 *val)
{
	uint8_t checksum;
	uint8_t buf[3] = {DIR_READ_HEAD,  addr >> 8, addr & 0xff };
    uint8_t bufout[8] = {0};
	struct i2c_msg msgs[2] = {
		{
			 .addr = client->addr,
			 .flags = 0,
			 .len = 3,
			 .buf = buf,
		},
		{
			.addr = client->addr,
			.flags = I2C_M_RD ,
			.len = 6,
			.buf = bufout,
		},
	};

	if (i2c_transfer(client->adapter, msgs,2) != 2) {
		debug_printk( "%s: Failed to read register 0x%02x\n",
			 __func__, addr);
		return -EIO;
	}

	checksum = xor8(bufout, 5);
	if (checksum != bufout[5]) {
		v4l2_err(client, "%s: Read register 0x%02x checksum error\n",
			 __func__, addr);
		return -EIO;
	}
	*val = ntohl(*(uint32_t*)bufout);
	//v4l2_dbg(1, debug, client, "%s: 0x%02x 0x%04x\n",
	//		 __func__, addr, *val);
	
	return 0;
}
//common register access
static int gxcam_readl_reg(struct i2c_client *client,
								   u16 addr, u32 *val)
{
	struct preread_regs regs;
	uint8_t checksum = 0;
	uint8_t buf[3] = {POST_READ_HEAD,  addr >> 8, addr & 0xff };
    uint8_t bufout[8] = {0};

	struct i2c_msg  msg = {
		.addr = client->addr,
		.flags = 0,
		.len = sizeof(regs),
		.buf = (u8 *)&regs,
	};
	struct i2c_msg msgs[2] = {
		{
			 .addr = client->addr,
			 .flags = 0,
			 .len = 3,
			 .buf = buf,
		},
		{
			.addr = client->addr,
			.flags = I2C_M_RD ,
			.len = 6,
			.buf = bufout,
		},
	};

	regs.pre_head = PRE_READ_HEAD;
	regs.reg = htons(addr);
	regs.xor = xor8((uint8_t *)&regs, 3);

	if (i2c_transfer(client->adapter, &msg, 1) != 1) {
		v4l2_err(client, "%s: Failed to write register 0x%02x\n",
			 __func__, addr);
		return -EIO;
	}
	usleep_range(20000, 25000); //20ms


	if (i2c_transfer(client->adapter, msgs,2) != 2) {
		return -EIO;
	}
	checksum = xor8(bufout, 5);
	if (checksum != bufout[5]) {
		v4l2_err(client, "%s: Read register 0x%02x checksum error\n",
			 __func__, addr);
		return -EIO;
	}
	*val = ntohl(*(uint32_t*)bufout);
	debug_printk( "gxcam_readl_reg: 0x%02x 0x%04x\n", addr, *val);
	return 0;
}
				
static int gxcam_writel_reg(struct i2c_client *client,
									u16 addr, u32 val)
{
	struct write_regs wr = {
		.head = WRITE_HEAD,
		.reg = htons(addr),
		.data = htonl(val),
		.xor = xor8((uint8_t *)&wr, sizeof(wr) - 1),
	};
	struct i2c_msg msg = {
		.addr = client->addr,
		.flags = 0,
		.len = sizeof(wr),
		.buf = (u8 *)&wr,
	};

	if (i2c_transfer(client->adapter, &msg, 1) != 1) {
		return -EIO;
	}
	
	return 0;
}

static int gxcam_read(struct i2c_client *client, u16 addr, u32 *value)
{
	int ret;
	int count = 0;
	while (count++ < I2C_READ_RETRY_COUNT) {
		ret = gxcam_readl_reg(client, addr, value);
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
//read direct registers
static int gxcam_read_d(struct i2c_client *client, u16 addr, u32 *value)
{
	int ret;
	int count = 0;
	while (count++ < I2C_READ_RETRY_COUNT) {
		ret = gxcam_readl_d_reg(client, addr, value);
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

static int gxcam_write(struct i2c_client *client, u16 addr, u32 value)
{
	int ret;
	int count = 0;
	while (count++ < I2C_WRITE_RETRY_COUNT) {
		ret = gxcam_writel_reg(client, addr, value);
		if(!ret)
			return ret;
	}
	v4l2_err(client, "%s: Write 0x%04x to register 0x%02x failed\n",
			 __func__, value, addr);
	return ret;
}

/* Write a list of registers */
static int __maybe_unused  gxcam_write_regs(struct i2c_client *client,
			     const struct reg_mv *regs, u32 len)
{
	unsigned int i;
	int ret;

	for (i = 0; i < len; i++) {
		ret = gxcam_write(client, regs[i].addr,regs[i].val);
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

static int gxcam_getroi(struct gxcam *gxcam)
{
  //  int ret;
    struct i2c_client *client = gxcam->client;
    gxcam_read_d(client, ROI_Offset_X,&gxcam->roi.left);
    gxcam_read_d(client, ROI_Offset_Y,&gxcam->roi.top);
    gxcam_read_d(client, ROI_Width,&gxcam->roi.width);
    gxcam_read_d(client, ROI_Height,&gxcam->roi.height);
    v4l2_dbg(1, debug, gxcam->client, "%s:get roi(%d,%d,%d,%d)\n",
			 __func__, gxcam->roi.left,gxcam->roi.top,gxcam->roi.width,gxcam->roi.height);
    return 0;
}



static int gxcam_g_volatile_ctrl(struct v4l2_ctrl *ctrl)
{
	int ret;
    struct gxcam *gxcam = 
		container_of(ctrl->handler, struct gxcam, ctrl_handler);
    struct i2c_client *client = gxcam->client;
    
	switch (ctrl->id) {
	case V4L2_CID_VEYE_GX_TRIGGER_MODE:
        //ret = gxcam_read_d(client, Trigger_Mode,&ctrl->val);
		//TODO test
		ctrl->val = 0;
		ret = 0;
		break;
	case V4L2_CID_VEYE_GX_TRIGGER_SRC:
        //ret = gxcam_read_d(client, Trigger_Source,&ctrl->val);
		ctrl->val = 0;
		ret = 0;
		break;

	case V4L2_CID_VEYE_GX_FRAME_RATE:
        ret = gxcam_read_d(client, Framerate_ex,&ctrl->val);
        ctrl->val = ctrl->val/10000;
        gxcam->cur_fps = ctrl->val;
		break;
	case V4L2_CID_VEYE_GX_SYNC_ROLE:
		//ret = gxcam_read_d(client, Sync_Role,&ctrl->val);
		//TODO test
		ctrl->val = 0;
		ret = 0;
		break;	
	default:
		dev_info(&client->dev,
			 "gxcam_g_volatile_ctrl ctrl(id:0x%x,val:0x%x) is not handled\n",
			 ctrl->id, ctrl->val);
		ret = -EINVAL;
		break;
	}
    
    v4l2_dbg(1, debug, gxcam->client, "%s: cid = (0x%X), value = (%d).\n",
                     __func__, ctrl->id, ctrl->val);

	return ret;
}

static int gxcam_s_ctrl(struct v4l2_ctrl *ctrl)
{
	int ret;
	struct gxcam *gxcam = 
		container_of(ctrl->handler, struct gxcam, ctrl_handler);
    struct i2c_client *client = gxcam->client;
    
	v4l2_dbg(1, debug, gxcam->client, "%s: cid = (0x%X), value = (%d).\n",
			 __func__, ctrl->id, ctrl->val);
	
    switch (ctrl->id) {
	case V4L2_CID_VEYE_GX_TRIGGER_MODE:
        ret = gxcam_write(client, Trigger_Mode,ctrl->val);
		break;
	case V4L2_CID_VEYE_GX_TRIGGER_SRC:
        ret = gxcam_write(client, Trigger_Source,ctrl->val);
		break;
	case V4L2_CID_VEYE_GX_SOFT_TRGONE:
        ret = gxcam_write(client, Trigger_Software,1);
		break;
	case V4L2_CID_VEYE_GX_FRAME_RATE:
        ret = gxcam_write(client, Framerate_ex,ctrl->val*10000);
        gxcam->cur_fps = ctrl->val;
		break;
	case V4L2_CID_VEYE_GX_SYNC_ROLE:
		ret = gxcam_write(client, Sync_Role,ctrl->val);
		break;
	default:
		dev_info(&client->dev,
			 "gxcam_s_ctrl ctrl(id:0x%x,val:0x%x) is not handled\n",
			 ctrl->id, ctrl->val);
		ret = -EINVAL;
		break;
	}

	return ret;
}


static const struct v4l2_ctrl_ops gxcam_ctrl_ops = {
    .g_volatile_ctrl = gxcam_g_volatile_ctrl,
	.s_ctrl = gxcam_s_ctrl,
};

//must add to enum_v4l2_ctrls_index once you add a new ctrl here
static struct v4l2_ctrl_config gxcam_v4l2_ctrls[] = {
    //standard v4l2_ctrls
    {
		.ops = NULL,
		.id = V4L2_CID_LINK_FREQ,
		.name = NULL,//kernel will fill it
		.type = V4L2_CTRL_TYPE_MENU ,
        .def = 0,
		.min = 0,
        .max = ARRAY_SIZE(link_freq_menu_items) - 1,
        .step = 0,
        .qmenu_int = link_freq_menu_items,
		.flags = V4L2_CTRL_FLAG_READ_ONLY,
	},
	{
		.ops = &gxcam_ctrl_ops,
		.id = V4L2_CID_PIXEL_RATE,
		.name = NULL,//kernel fill fill it
		.type = V4L2_CTRL_TYPE_INTEGER,
		.def = GXCAM_PIXEL_RATE,
		.min = GXCAM_PIXEL_RATE,
		.max = GXCAM_PIXEL_RATE,
		.step = 1,
		.flags = V4L2_CTRL_FLAG_READ_ONLY,
	},
	//custom v4l2-ctrls
	{
		.ops = &gxcam_ctrl_ops,
		.id = V4L2_CID_VEYE_GX_TRIGGER_MODE,
		.name = "trigger_mode",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.def = Video_Streaming_mode,
		.min = 0,
		.max = Trigger_mode_num-1,
		.step = 1,
		.flags = V4L2_CTRL_FLAG_VOLATILE|V4L2_CTRL_FLAG_EXECUTE_ON_WRITE,
	},
	{
		.ops = &gxcam_ctrl_ops,
		.id = V4L2_CID_VEYE_GX_TRIGGER_SRC,
		.name = "trigger_src",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.def = Trg_Hard,
		.min = 0,
		.max = Trg_src_num-1,
		.step = 1,
		.flags = V4L2_CTRL_FLAG_VOLATILE|V4L2_CTRL_FLAG_EXECUTE_ON_WRITE,
	},
	{
		.ops = &gxcam_ctrl_ops,
		.id = V4L2_CID_VEYE_GX_SOFT_TRGONE,
		.name = "soft_trgone",
		.type = V4L2_CTRL_TYPE_BUTTON,
		.def = 0,
		.min = 0,
		.max = 0,
		.step = 0,
	},
	{
		.ops = &gxcam_ctrl_ops,
		.id = V4L2_CID_VEYE_GX_SYNC_ROLE,
		.name = "sync_role",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.def = 0, //default is master
		.min = 0,
		.max = 1, //0:master, 1:slave
		.step = 1,
		.flags = V4L2_CTRL_FLAG_VOLATILE|V4L2_CTRL_FLAG_EXECUTE_ON_WRITE,
	},
	{
		.ops = &gxcam_ctrl_ops,
		.id = V4L2_CID_VEYE_GX_FRAME_RATE,
		.name = "frame_rate",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.def = GXCAM_DEF_FPS,
		.min = 0,
		.max = GXCAM_DEF_FPS,
		.step = 1,
		.flags = V4L2_CTRL_FLAG_VOLATILE|V4L2_CTRL_FLAG_EXECUTE_ON_WRITE,
	},

};
//grab some ctrls while streaming
static void gxcam_v4l2_ctrl_grab(struct gxcam *gxcam,bool grabbed)
{
    if (gxcam->ctrls[CID_VEYE_GX_TRIGGER_MODE])
        v4l2_ctrl_grab(gxcam->ctrls[CID_VEYE_GX_TRIGGER_MODE], grabbed);
    if (gxcam->ctrls[CID_VEYE_GX_TRIGGER_SRC])
        v4l2_ctrl_grab(gxcam->ctrls[CID_VEYE_GX_TRIGGER_SRC], grabbed);
    if (gxcam->ctrls[CID_VEYE_GX_FRAME_RATE])
        v4l2_ctrl_grab(gxcam->ctrls[CID_VEYE_GX_FRAME_RATE], grabbed);
    if (gxcam->ctrls[CID_VEYE_GX_SYNC_ROLE])
        v4l2_ctrl_grab(gxcam->ctrls[CID_VEYE_GX_SYNC_ROLE], grabbed);

}

static void gxcam_v4l2_ctrl_init(struct gxcam *gxcam)
{
    int i = 0;
    u32 value = 0;
    struct i2c_client *client = gxcam->client;
    for (i = 0; i < ARRAY_SIZE(gxcam_v4l2_ctrls); ++i) {
		switch(gxcam_v4l2_ctrls[i].id)
        {
            case V4L2_CID_VEYE_GX_TRIGGER_MODE:
                //todo
				//gxcam_read_d(client, Trigger_Mode,&value);
				value = 0;
                gxcam_v4l2_ctrls[i].def = value;
                v4l2_dbg(1, debug, gxcam->client, "%s:default trigger mode %d\n", __func__, value);
            break;
            case V4L2_CID_VEYE_GX_TRIGGER_SRC:
                //gxcam_read_d(client, Trigger_Source,&value);
				value = 0;
                gxcam_v4l2_ctrls[i].def = value;
                v4l2_dbg(1, debug, gxcam->client, "%s:default trigger source %d\n", __func__, value);
            break;
			case V4L2_CID_VEYE_GX_SYNC_ROLE:
				//gxcam_read_d(client, Sync_Role,&value);
				value = 0; //default is master
				gxcam_v4l2_ctrls[i].def = value;
				v4l2_dbg(1, debug, gxcam->client, "%s:default sync role %d\n", __func__, value);
			break;
            case V4L2_CID_VEYE_GX_FRAME_RATE:
                gxcam_read_d(client, Framerate_ex,&value);
                gxcam_v4l2_ctrls[i].def = value/10000;
                gxcam_read_d(client, MaxFrame_Rate,&value);
                gxcam_v4l2_ctrls[i].max = value/10000;
                v4l2_dbg(1, debug, gxcam->client, "%s:default framerate %lld , max fps %lld \n", __func__, \
                    gxcam_v4l2_ctrls[i].def,gxcam_v4l2_ctrls[i].max);
            break;
			case V4L2_CID_PIXEL_RATE:
				if(gxcam->pixelrate != 0){
					gxcam_v4l2_ctrls[i].min = gxcam->pixelrate;
					gxcam_v4l2_ctrls[i].def = gxcam->pixelrate;
					gxcam_v4l2_ctrls[i].max = gxcam->pixelrate;
				}
            default:
            break;
        }
	}
}


#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
int gxcam_g_mbus_config(struct v4l2_subdev *sd,
				struct v4l2_mbus_config *cfg)
#else
int gxcam_get_mbus_config(struct v4l2_subdev *sd,
				unsigned int pad,
				struct v4l2_mbus_config *cfg)
#endif
{
    struct gxcam *gxcam = to_gxcam(sd);
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
    if(gxcam->lane_num == 4){
        cfg->flags |= V4L2_MBUS_CSI2_4_LANE; /* XXX wierd */
    }else{
        cfg->flags |= V4L2_MBUS_CSI2_2_LANE; /* XXX wierd */
    }
    debug_printk("gxcam_get_mbus_config lane num %d\n",gxcam->lane_num);
	return 0;
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 15, 0)
static int gxcam_csi2_enum_mbus_code(
			struct v4l2_subdev *sd,
			struct v4l2_subdev_pad_config *cfg,
			struct v4l2_subdev_mbus_code_enum *code)
#else
static int gxcam_csi2_enum_mbus_code(
			struct v4l2_subdev *sd,
			struct v4l2_subdev_state *sd_state,
			struct v4l2_subdev_mbus_code_enum *code)
#endif
{
	struct gxcam *gxcam = to_gxcam(sd);
	struct gxcam_format *supported_formats = gxcam->supported_formats;
	int num_supported_formats = gxcam->num_supported_formats;
	VEYE_TRACE

	if (code->index >= num_supported_formats)
		return -EINVAL;
	code->code = supported_formats[code->index].mbus_code;
    v4l2_dbg(1, debug, sd, "%s: index = (%d) mbus code (%x)\n", __func__, code->index,code->code);

	return 0;
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 15, 0)
static int gxcam_csi2_enum_framesizes(
			struct v4l2_subdev *sd,
			struct v4l2_subdev_pad_config *cfg,
			struct v4l2_subdev_frame_size_enum *fse)
#else
static int gxcam_csi2_enum_framesizes(
			struct v4l2_subdev *sd,
			struct v4l2_subdev_state *sd_state,
			struct v4l2_subdev_frame_size_enum *fse)
#endif
{
	struct gxcam *gxcam = to_gxcam(sd);
    VEYE_TRACE

    if (fse->index != 0)
		return -EINVAL;
	fse->min_width = fse->max_width =
		gxcam->roi.width;
	fse->min_height = fse->max_height =
		gxcam->roi.height;
    v4l2_dbg(1, debug, sd, "%s: code = (0x%X), index = (%d) frm width %d height %d\n",
		 __func__, fse->code, fse->index,fse->max_width,fse->max_height);
	return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 15, 0)
static int gxcam_csi2_get_fmt(struct v4l2_subdev *sd,
								struct v4l2_subdev_pad_config *cfg,
								struct v4l2_subdev_format *format)
#else
static int gxcam_csi2_get_fmt(struct v4l2_subdev *sd,
								struct v4l2_subdev_state *sd_state,
								struct v4l2_subdev_format *format)
#endif
{
	struct gxcam *gxcam = to_gxcam(sd);
	struct gxcam_format *current_format;
	VEYE_TRACE

	current_format = &gxcam->supported_formats[gxcam->current_format_idx];
	format->format.width = gxcam->roi.width;
	format->format.height = gxcam->roi.height;
    
	format->format.code = current_format->mbus_code;
	format->format.field = V4L2_FIELD_NONE;
	format->format.colorspace = V4L2_COLORSPACE_REC709;//color
	v4l2_dbg(1, debug, sd, "%s: width: (%d) height: (%d) code: (0x%X)\n",
		__func__, format->format.width,format->format.height,
			format->format.code);
	return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 15, 0)
static int gxcam_get_selection(struct v4l2_subdev *sd,
				struct v4l2_subdev_pad_config *cfg,
				struct v4l2_subdev_selection *sel)
#else
static int gxcam_get_selection(struct v4l2_subdev *sd,
				struct v4l2_subdev_state *sd_state,
				struct v4l2_subdev_selection *sel)
#endif

{
	struct gxcam *gxcam = to_gxcam(sd);
    VEYE_TRACE
	switch (sel->target) {
	case V4L2_SEL_TGT_CROP: {
    		//sel->r = *__gxcam_get_pad_crop(gxcam, cfg, sel->pad,
    		//				sel->which);
            sel->r = gxcam->roi;
    		break;
	    }        
        //active area
        case V4L2_SEL_TGT_CROP_DEFAULT:
        case V4L2_SEL_TGT_NATIVE_SIZE:
        case V4L2_SEL_TGT_CROP_BOUNDS:
            sel->r.top = 0;
            sel->r.left = 0;
            sel->r.width = gxcam->max_width;
            sel->r.height = gxcam->max_height;
		break;
        default:
		return -EINVAL;
	}
    sel->flags = V4L2_SEL_FLAG_LE;
    v4l2_dbg(1, debug, sd, "%s: target %d\n", __func__,V4L2_SEL_TGT_CROP);
    return 0;
}
static int gxcam_frm_supported(int roi_x,int wmin, int wmax, int ws,
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

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 15, 0)
static int gxcam_csi2_try_fmt(struct v4l2_subdev *sd,
		struct v4l2_subdev_pad_config *cfg,
		struct v4l2_subdev_format *format)
#else
static int gxcam_csi2_try_fmt(struct v4l2_subdev *sd,
		struct v4l2_subdev_state *sd_state,
		struct v4l2_subdev_format *format)
#endif
{
	struct gxcam *gxcam = to_gxcam(sd);
	int ret = 0;

	ret = gxcam_frm_supported(
			gxcam->roi.left,gxcam->min_width, gxcam->max_width, GX_CAM_ROI_W_ALIGN,
			gxcam->roi.top,gxcam->min_height, gxcam->max_height, GX_CAM_ROI_H_ALIGN,
			format->format.width, format->format.height);

	if (ret < 0) {
		v4l2_err(sd, "Not supported size!\n");
		return ret;
	}

	return 0;
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 15, 0)
static int gxcam_csi2_set_fmt(struct v4l2_subdev *sd,
                              struct v4l2_subdev_pad_config *cfg,
                              struct v4l2_subdev_format *format)
#else
static int gxcam_csi2_set_fmt(struct v4l2_subdev *sd,
                              struct v4l2_subdev_state *sd_state,
                              struct v4l2_subdev_format *format)
#endif
{
    struct gxcam *gxcam = to_gxcam(sd);
//    struct v4l2_mbus_framefmt *framefmt;

    VEYE_TRACE;

    if (format->format.width != gxcam->roi.width ||
        format->format.height != gxcam->roi.height) {
        v4l2_info(sd, "Changing the resolution is not supported now\n");
        v4l2_info(sd, "%d,%d,%d,%d\n",
                  format->format.width, gxcam->roi.width,
                  format->format.height, gxcam->roi.height);
        return -EINVAL;
    }

    v4l2_dbg(1, debug, sd, "%s: code: 0x%X",
             __func__, format->format.code);

    if (format->which == V4L2_SUBDEV_FORMAT_TRY) {
        v4l2_info(sd, "csi2_try format\n");
			   #if LINUX_VERSION_CODE < KERNEL_VERSION(5, 15, 0)
                return gxcam_csi2_try_fmt(sd, cfg, format);
				#else
				return gxcam_csi2_try_fmt(sd, sd_state, format);
				#endif
    } else {
    }

    // update_controls(gxcam);

    return 0;
}

static int gxcam_g_input_status(struct v4l2_subdev *sd, u32 *status)
{
    VEYE_TRACE
	*status = 1;
	return 0;
}


static void gxcam_free_controls(struct gxcam *gxcam)
{
	v4l2_ctrl_handler_free(gxcam->subdev->ctrl_handler);
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
  //  case GX_DT_Mono14:
	//	return MEDIA_BUS_FMT_Y14_1X14;
	case MV_DT_UYVY:
		//return MEDIA_BUS_FMT_UYVY8_2X8;//0x2006
		return MEDIA_BUS_FMT_UYVY8_1X16;//0x200f
	case MV_DT_YUYV:
		return MEDIA_BUS_FMT_YUYV8_1X16;
	}
    //not supported
	return -1;
}

int get_fmt_index(struct gxcam *gxcam,u32 datatype)
{
    int i = 0;
    for(;i < gxcam->num_supported_formats;++i)
    {
        if((gxcam->supported_formats[i].data_type) == datatype)
            return i;
    }
    return -1;
}

static int gxcam_enum_pixformat(struct gxcam *gxcam)
{
	int ret = 0;
	u32 mbus_code = 0;
	int pixformat_type;
	int index = 0;
    int bitindex = 0;
	int num_pixformat = 0;
    u32 fmtcap = 0;
    u32 cur_fmt;
	struct i2c_client *client = gxcam->client;
    
    ret = gxcam_read_d(client, Format_cap, &fmtcap);
    if (ret < 0)
		goto err;
	num_pixformat = bit_count(fmtcap);
	if (num_pixformat < 0)
		goto err;
    
    v4l2_dbg(1, debug, gxcam->client, "%s: format count: %d; format cap 0x%x\n",
					__func__, num_pixformat,fmtcap);
    
	gxcam->supported_formats = devm_kzalloc(&client->dev,
		sizeof(struct gxcam_format) * (num_pixformat+1), GFP_KERNEL);
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
    		gxcam->supported_formats[index].index = index;
    		gxcam->supported_formats[index].mbus_code = mbus_code;
    		gxcam->supported_formats[index].data_type = pixformat_type;
            v4l2_dbg(1, debug, gxcam->client, "%s support format index %d mbuscode 0x%x datatype: %d\n",
    					__func__, index,mbus_code,pixformat_type);
            index++;
        }
	}
	gxcam->num_supported_formats = num_pixformat;

    gxcam_read_d(client, Pixel_Format, &cur_fmt);
	gxcam->current_format_idx = get_fmt_index(gxcam,cur_fmt);
    v4l2_dbg(1, debug, gxcam->client, "%s: cur format: %d\n",
					__func__, cur_fmt);
	// gxcam_add_extension_pixformat(gxcam);
	return 0;

err:
	return -ENODEV;
}

static void gxcam_get_mipifeature(struct gxcam *gxcam)
{
    u32 lane_num;
    u32 lanenum_cap;
    u32 mipi_datarate;
    struct i2c_client *client = gxcam->client;
    gxcam_read_d(client, Lane_Num, &lane_num);
    if(lane_num == 4){
        gxcam->lane_num = 4;
    }else{
        gxcam->lane_num = 2;
    }
    gxcam_read(client, LaneNum_Cap, &lanenum_cap);
	if (lanenum_cap <= 0xF)//
	{
		gxcam->lanecap = lanenum_cap;
	}
    gxcam_read_d(client, MIPI_DataRate, &mipi_datarate);
    if(mipi_datarate == 0xFFFFFFFF)
        mipi_datarate = GXCAM_DEFAULT_LINK_FREQ;
    else
        mipi_datarate *=1000;//register value is kbps
    
    gxcam->mipi_datarate = mipi_datarate;

    debug_printk("%s: lane num %d, datarate %d bps\n",
					__func__, gxcam->lane_num,gxcam->mipi_datarate);
    return;
}

/* Start streaming */
static int gxcam_start_streaming(struct gxcam *gxcam)
{
	struct i2c_client *client = gxcam->client;
	int ret;
    
	/* Apply customized values from user */
 //   ret =  __v4l2_ctrl_handler_setup(gxcam->sd.ctrl_handler);
    debug_printk("gxcam_start_streaming \n");
	/* set stream on register */
    ret = gxcam_write(client, Image_Acquisition,1);
	if (ret)
		return ret;

	/* some v4l2 ctrls cannot change during streaming */
    gxcam_v4l2_ctrl_grab(gxcam,true);
	return ret;
}

/* Stop streaming */
static int gxcam_stop_streaming(struct gxcam *gxcam)
{
	struct i2c_client *client = gxcam->client;
	int ret;

	/* set stream off register */
    ret = gxcam_write(client, Image_Acquisition,0);
	if (ret)
		dev_err(&client->dev, "%s failed to set stream\n", __func__);
    debug_printk("gxcam_stop_streaming \n");
    
   	 gxcam_v4l2_ctrl_grab(gxcam,false);

	/*
	 * Return success even if it was an error, as there is nothing the
	 * caller can do about it.
	 */
	return 0;
}


static int gxcam_set_stream(struct v4l2_subdev *sd, int enable)
{
	struct gxcam *gxcam = to_gxcam(sd);
    struct i2c_client *client = gxcam->client;
	int ret = 0;
	enable = !!enable;
	
	if (gxcam->streaming == enable) {
        dev_info(&client->dev, "%s streaming state not changed %d!\n", __func__,enable);
		return 0;
	}
VEYE_TRACE
	if (enable) {

		/*
		 * Apply default & customized values
		 * and then start streaming.
		 */
		ret = gxcam_start_streaming(gxcam);
		if (ret)
			goto end;
	} else {
		gxcam_stop_streaming(gxcam);
	}
	gxcam->streaming = enable;

	return ret;
end:
	return ret;
}


/* Power management functions */
static int gxcam_power_on(struct gxcam *gxcam)
{
     if(gxcam){
        debug_printk("gxcam_power_on\n");
	    gpio_set_value(gxcam->reset_gpio, 1);
    }else{
        debug_printk("gxcam_power_on gxcam is NULL\n");
    }
	usleep_range(STARTUP_MIN_DELAY_US,
		     STARTUP_MIN_DELAY_US + STARTUP_DELAY_RANGE_US);
	return 0;
}

static int gxcam_power_off(struct gxcam *gxcam)
{
    //do not really power off, because we might use i2c script at any time
    if(gxcam){
        debug_printk("gxcam_power_off\n");
	    gpio_set_value(gxcam->reset_gpio, 1);
    }else{
        debug_printk("gxcam_power_off gxcam is NULL\n");
    }
	return 0;
}

int gxcam_s_power(struct v4l2_subdev *sd, int on)
{
    struct gxcam *gxcam = to_gxcam(sd);
	//struct camera_common_data *s_data = gxcam->s_data;
    //may be could delete this. 
    camera_common_s_power(sd,on);
	if (on) {
        gxcam_power_on(gxcam);
	} else {
		gxcam_power_off(gxcam);
	}
	return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 15, 0)
static int gxcam_enum_frame_interval(struct v4l2_subdev *sd,
				struct v4l2_subdev_pad_config *cfg,
				struct v4l2_subdev_frame_interval_enum *fie)
#else
static int gxcam_enum_frame_interval(struct v4l2_subdev *sd,
				struct v4l2_subdev_state *sd_state,
				struct v4l2_subdev_frame_interval_enum *fie)
#endif
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct camera_common_data *s_data = to_camera_common_data(&client->dev);
	struct gxcam *gxcam = (struct gxcam *)s_data->priv;
	struct v4l2_fract tpf;
    u32 fps_reg;
    VEYE_TRACE

	/* Only one frame rate should be returned
	 * - the current frame rate
	 */
	if(fie->index > 0)
		return -EINVAL;

    gxcam_read(client, Framerate_ex,&fps_reg);
    gxcam->cur_fps = fps_reg/10000;

	/* Translate frequency to timeperframe
	* by inverting the fraction
	*/
	tpf.numerator = 10000;
	tpf.denominator = gxcam->cur_fps*10000;

    v4l2_dbg(1, debug, sd, "%s: code = (0x%X), index = (%d), fps=(%d)\n",
                 __func__, fie->code, fie->index,gxcam->cur_fps);
    
	fie->interval = tpf;

	return 0;
}
static int gxcam_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
	struct gxcam *gxcam = to_gxcam(sd);
	struct v4l2_mbus_framefmt *try_fmt =
	#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 15, 0)
		v4l2_subdev_get_try_format(sd, fh->pad, IMAGE_PAD);
    #else
		v4l2_subdev_get_try_format(sd, fh->state, IMAGE_PAD);
	#endif

    struct v4l2_rect *try_crop;
	/* Initialize try_fmt */
	try_fmt->width = gxcam->max_width;
	try_fmt->height = gxcam->max_height;
	try_fmt->code = gxcam->supported_formats[0].mbus_code;
	try_fmt->field = V4L2_FIELD_NONE;

	/* Initialize try_fmt for the embedded metadata pad */
/*	try_fmt_meta->width = GXCAM_EMBEDDED_LINE_WIDTH;
	try_fmt_meta->height = GXCAM_NUM_EMBEDDED_LINES;
	try_fmt_meta->code = MEDIA_BUS_FMT_SENSOR_DATA;
	try_fmt_meta->field = V4L2_FIELD_NONE;
*/
    /* Initialize try_crop rectangle. */
	#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 15, 0)
		try_crop = v4l2_subdev_get_try_crop(sd, fh->pad, 0);
	#else
		try_crop = v4l2_subdev_get_try_crop(sd, fh->state, 0);
	#endif
	try_crop->top = 0;
	try_crop->left = 0;
	try_crop->width = gxcam->max_width;
	try_crop->height = gxcam->max_height;
    
    
	return 0;
}
static const struct v4l2_subdev_core_ops gxcam_core_ops = {
	 .s_power = gxcam_s_power,  
};

static const struct v4l2_subdev_video_ops gxcam_video_ops = {
	.s_stream = gxcam_set_stream,
    #if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
	.g_mbus_config	= gxcam_g_mbus_config,
    #endif
    .g_input_status = gxcam_g_input_status,
};

static const struct v4l2_subdev_pad_ops gxcam_pad_ops = {
	.enum_mbus_code = gxcam_csi2_enum_mbus_code,
	.get_fmt = gxcam_csi2_get_fmt,
	.set_fmt = gxcam_csi2_set_fmt,
	.enum_frame_size = gxcam_csi2_enum_framesizes,
    //v4.9 kernel can not support selection correctly
	.get_selection = gxcam_get_selection,
	//.set_selection = gxcam_set_selection,
    #if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
	.get_mbus_config	= gxcam_get_mbus_config,
    #endif
	.enum_frame_interval = gxcam_enum_frame_interval,
};

static const struct v4l2_subdev_ops gxcam_subdev_ops = {
	.core = &gxcam_core_ops,
	.video = &gxcam_video_ops,
	.pad = &gxcam_pad_ops,
};

static const struct v4l2_subdev_internal_ops gxcam_internal_ops = {
	.open = gxcam_open,
};

static const struct media_entity_operations gxcam_csi2_media_ops = {
	.link_validate = v4l2_subdev_link_validate,
};

const struct of_device_id gxcam_csi2_of_match[] = {
	{ .compatible = "nvidia,veye_gxcam",},
	{ },
};
MODULE_DEVICE_TABLE(of, gxcam_csi2_of_match);
static int gxcam_enum_controls(struct gxcam *gxcam)
{
    struct i2c_client *client = gxcam->client;
    struct v4l2_ctrl_handler *ctrl_hdlr;
    int ret;
    int i;
    struct v4l2_ctrl *ctrl;
    ctrl_hdlr = &gxcam->ctrl_handler;
    ret = v4l2_ctrl_handler_init(ctrl_hdlr, ARRAY_SIZE(gxcam_v4l2_ctrls));
    if (ret)
        return ret;
    
    for (i = 0; i < ARRAY_SIZE(gxcam_v4l2_ctrls); ++i) {
		ctrl = v4l2_ctrl_new_custom(
			ctrl_hdlr,
			&gxcam_v4l2_ctrls[i],
			NULL);
		if (ctrl == NULL) {
			dev_err(&client->dev, "Failed to init %d ctrl\n",i);
			gxcam->ctrls[i] = NULL;
			continue;
		}
		gxcam->ctrls[i] = ctrl;
        dev_dbg(&client->dev, "init control %s success\n",gxcam_v4l2_ctrls[i].name);
	}

    
	if (ctrl_hdlr->error) {
		ret = ctrl_hdlr->error;
		dev_err(&client->dev, "%s control init failed (%d)\n",
			__func__, ret);
		goto error;
	}

	gxcam->subdev->ctrl_handler = ctrl_hdlr;
    v4l2_ctrl_handler_setup(ctrl_hdlr);
    
	return 0;

error:
	v4l2_ctrl_handler_free(ctrl_hdlr);

	return ret;

}
/* Verify chip ID */
static int gxcam_identify_module(struct gxcam * gxcam)
{
	int ret;
    u32 device_id;
	//u32 firmware_version;
	u32 value[8];
    struct i2c_client *client = v4l2_get_subdevdata(gxcam->subdev);
    
    ret = gxcam_read_d(client, Model_Name, &device_id);
    if (ret ) {
        dev_err(&client->dev, "failed to read chip id\n");
        ret = -ENODEV;
        return ret;
    }
	ret = gxcam_read_d(client, CameraModel0, &value[0]);
	ret |= gxcam_read_d(client, CameraModel1, &value[1]);
	ret |= gxcam_read_d(client, CameraModel2, &value[2]);
	ret |= gxcam_read_d(client, CameraModel3, &value[3]);
	ret |= gxcam_read_d(client, CameraModel4, &value[4]);
	ret |= gxcam_read_d(client, CameraModel5, &value[5]);
	ret |= gxcam_read_d(client, CameraModel6, &value[6]);
	ret |= gxcam_read_d(client, CameraModel7, &value[7]);

	if (ret) {
		dev_err(&client->dev, "failed to read camera model\n");
		ret = -ENODEV;
		return ret;
	}
	strncpy(gxcam->camera_model, (char *)value, sizeof(gxcam->camera_model));

	dev_info(&client->dev, "camera is: %s\n", gxcam->camera_model);
    
	return 0;
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
static int gxcam_check_hwcfg(struct device *dev,struct gxcam * gxcam)
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
        gxcam->reset_gpio = 0;
	}else{
        gxcam->reset_gpio = (unsigned int)gpio;
    }
    
	return 0;
}
#else
static int gxcam_check_hwcfg(struct device *dev,struct gxcam * gxcam)
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
        gxcam->reset_gpio = 0;
	}else{
        gxcam->reset_gpio = (unsigned int)gpio;
    }
	
	ret = 0;

error_out:
	v4l2_fwnode_endpoint_free(&ep_cfg);
	fwnode_handle_put(endpoint);

	return ret;
}
#endif

static ssize_t model_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    struct gxcam *cam = container_of(kobj, struct gxcam, kobj);
    return sprintf(buf, "%s\n", cam->camera_model);
}

static ssize_t width_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    struct gxcam *cam = container_of(kobj, struct gxcam, kobj);
    return sprintf(buf, "%d\n", cam->roi.width);
}

static ssize_t height_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    struct gxcam *cam = container_of(kobj, struct gxcam, kobj);
    return sprintf(buf, "%d\n", cam->roi.height);
}

static ssize_t fps_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    struct gxcam *cam = container_of(kobj, struct gxcam, kobj);
    return sprintf(buf, "%d\n", cam->cur_fps);
}

static struct kobj_attribute model_attribute = __ATTR(camera_model, 0444, model_show, NULL);
static struct kobj_attribute width_attribute = __ATTR(width, 0444, width_show, NULL);
static struct kobj_attribute height_attribute = __ATTR(height, 0444, height_show, NULL);
static struct kobj_attribute fps_attribute = __ATTR(fps, 0444, fps_show, NULL);

static struct attribute *gxcam_attrs[] = {
    &model_attribute.attr,
    &width_attribute.attr,
    &height_attribute.attr,
	&fps_attribute.attr,
    NULL,
};

static struct attribute_group gxcam_attr_group = {
    .attrs = gxcam_attrs,
};

static const struct attribute_group *gxcam_attr_groups[] = {
    &gxcam_attr_group,
    NULL,
};

static struct kobj_type gxcam_ktype = {
    .sysfs_ops = &kobj_sysfs_ops,
    .default_groups = gxcam_attr_groups,
};

static int gxcam_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct gxcam *gxcam;
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
	gxcam = devm_kzalloc(&client->dev, sizeof(struct gxcam), GFP_KERNEL);
	if (!gxcam)
		return -ENOMEM;
    
    gxcam->subdev = &common_data->subdev;
	gxcam->subdev->ctrl_handler = &gxcam->ctrl_handler;
	gxcam->client = client;
    gxcam->s_data = common_data;
    
       /* Check the hardware configuration in device tree */
	if(gxcam_check_hwcfg(dev,gxcam))
		return -EINVAL;

	/* Initialize subdev */
	v4l2_i2c_subdev_init(gxcam->subdev, client, &gxcam_subdev_ops);
    gxcam->subdev->internal_ops = &gxcam_internal_ops;
    gxcam->subdev->dev = &client->dev;
    gxcam->subdev->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
    /* Initialize subdev */
    /* Set owner to NULL so we can unload the driver module */
	gxcam->subdev->owner = NULL;
    
    common_data->priv = gxcam;
	common_data->dev = &client->dev;
	common_data->ctrl_handler = &gxcam->ctrl_handler;
	common_data->ctrls = gxcam->ctrls;
    
	ret = gxcam_power_on(gxcam);
	if (ret)
		return ret;
    ret = gxcam_identify_module(gxcam);
	if(ret){
        dev_err(dev, "gxcam_identify_module failed.\n");
		goto error_power_off;
	}
	if (gxcam_enum_pixformat(gxcam)) {
		dev_err(dev, "enum pixformat failed.\n");
		ret = -ENODEV;
		goto error_power_off;
	}
	gxcam_get_mipifeature(gxcam);


    gxcam_read_d(client, Sensor_Width, &gxcam->max_width);
    gxcam_read_d(client, Sensor_Height, &gxcam->max_height);
	gxcam_read_d(client, MIN_ROI_Width, &gxcam->min_width);
	gxcam_read_d(client, MIN_ROI_Height, &gxcam->min_height);
    v4l2_dbg(1, debug, gxcam->client, "%s: max width %d; max height %d\n",
					__func__, gxcam->max_width,gxcam->max_height);
    //read roi
    gxcam_getroi(gxcam);
    
    gxcam_v4l2_ctrl_init(gxcam);
    
	if (gxcam_enum_controls(gxcam)) {
		dev_err(dev, "enum controls failed.\n");
		ret = -ENODEV;
		goto error_power_off;
	}
	
    //gxcam_init_mode(&gxcam->sd);
    //stop acquitsition
    gxcam_write(client, Image_Acquisition,0);
    
    //gxcam->subdev->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
    //gxcam->subdev->entity.function = MEDIA_ENT_F_CAM_SENSOR;
        /* Initialize source pad */
    //  gxcam->pad[METADATA_PAD].flags = MEDIA_PAD_FL_SOURCE;
    #if defined(CONFIG_MEDIA_CONTROLLER)
    gxcam->pad[IMAGE_PAD].flags = MEDIA_PAD_FL_SOURCE;
	gxcam->subdev->entity.ops = &gxcam_csi2_media_ops;
    ret = tegra_media_entity_init(&gxcam->subdev->entity, NUM_PADS,
				gxcam->pad, true, true);
	if (ret < 0){
        dev_err(&client->dev, "Failed at tegra_media_entity_init \n");
        goto error_handler_free;
	}
    #endif
    //use lesss for us, we do not need dts infomations here
	ret = camera_common_initialize(common_data, "veye_gxcam");
	if (ret) {
		dev_err(&client->dev, "Failed to initialize tegra common for gxcam.\n");
        goto error_media_entity;  
//		return ret;
	}
    
	ret = kobject_init_and_add(&gxcam->kobj, &gxcam_ktype, &client->dev.kobj, "veye_gxcam");
    if (ret) {
        dev_err(dev, "kobject_init_and_add failed\n");
        goto error_media_entity;
    }
	
	ret = v4l2_async_register_subdev(gxcam->subdev);
	if (ret < 0){
        dev_err(&client->dev, "Failed at v4l2_async_register_subdev for gxcam.\n");
		goto error_media_entity;
	}

	return 0;

error_media_entity:
	media_entity_cleanup(&gxcam->subdev->entity);

error_handler_free:
	gxcam_free_controls(gxcam);

error_power_off:
	gxcam_power_off(gxcam);

	return ret;
}

static int gxcam_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);
	struct gxcam *gxcam = to_gxcam(sd);

	gxcam_free_controls(gxcam);
    
    v4l2_async_unregister_subdev(sd);
	v4l2_device_unregister_subdev(sd);
    #if defined(CONFIG_MEDIA_CONTROLLER)
	media_entity_cleanup(&sd->entity);
    #endif
	return 0;
}

static struct i2c_device_id veyegx_cam_dt_ids[] = {
	{"veye_gxcam", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, veyegx_cam_dt_ids);

static struct i2c_driver veyegx_cam_i2c_driver = {
	.driver = {
		.name = "gxcam",
        .owner = THIS_MODULE,
		.of_match_table	= veye_gxcam_of_match,
	},
	.probe = gxcam_probe,
	.remove = gxcam_remove,
    .id_table = veyegx_cam_dt_ids,
};

module_i2c_driver(veyegx_cam_i2c_driver);

MODULE_AUTHOR("xumm <www.veye.cc>");
MODULE_DESCRIPTION("VEYE GX series mipi camera v4l2 driver");
MODULE_LICENSE("GPL v2");

