/*
 * veyecam.c - veyecam sensor driver
 *
 * Copyright (c) 2016-2019, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 /*
    This dts is for all VEYE series cameras :
    VEYE-MIPI-327E
    VEYE-MIPI-IMX327S
    VEYE-MIPI-IMX462
    VEYE-MIPI-IMX385
    ...
*/
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>

#include <media/tegra_v4l2_camera.h>
#include <media/tegracam_core.h>
#include "veyecam.h"

#define CREATE_TRACE_POINTS

#define VEYECAM_FUSE_ID_ADDR	0x3382
#define VEYECAM_FUSE_ID_SIZE	6
#define VEYECAM_FUSE_ID_STR_SIZE	(VEYECAM_FUSE_ID_SIZE * 2)

/* veyecam model register address */
#define VEYECAM_MODEL_ID_ADDR		0x0001
#define VEYECAM_DEVICE_ID 		0x06

#define VEYECAM_HDVER_ADDR    0x00

#define SENSOR_TYPR_ADDR_L    0x20
#define SENSOR_TYPR_ADDR_H    0x21

#define BOARD_TYPR_ADDR    0x25

//#define DEBUG_PRINTK
#ifndef DEBUG_PRINTK
#define debug_printk(s , ... )
#define VEYE_TRACE 
#else
#define debug_printk printk
#define VEYE_TRACE printk("%s %d \n",__FUNCTION__,__LINE__);
#endif


static const struct of_device_id veyecam_of_match[] = {
	{ .compatible = "nvidia,veyecam",},
	{ },
};
MODULE_DEVICE_TABLE(of, veyecam_of_match);

static const u32 ctrl_cid_list[] = {
	/*TEGRA_CAMERA_CID_GAIN,
	TEGRA_CAMERA_CID_EXPOSURE,
	TEGRA_CAMERA_CID_FRAME_RATE,
	TEGRA_CAMERA_CID_FUSE_ID,
	TEGRA_CAMERA_CID_HDR_EN,*/
	TEGRA_CAMERA_CID_SENSOR_MODE_ID,
};

struct veyecam {
	struct i2c_client	*i2c_client;
	struct v4l2_subdev	*subdev;
	//u8 fuse_id[VEYECAM_FUSE_ID_SIZE];
	u32				frame_length;
	s64 last_wdr_et_val;
	struct camera_common_data	*s_data;
	struct tegracam_device		*tc_dev;
};

static const struct regmap_config sensor_regmap_config = {
	.reg_bits = 16,
	.val_bits = 8,
	.cache_type = REGCACHE_RBTREE,
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 4, 0)
	.use_single_rw = true,
#else
	.use_single_read = true,
	.use_single_write = true,
#endif
};

static inline void veyecam_get_frame_length_regs(veyecam_reg *regs,
				u32 frame_length)
{
    return ;
}

static inline void veyecam_get_coarse_time_regs_shs1(veyecam_reg *regs,
				u32 coarse_time)
{
    return ;
}

static inline void veyecam_get_coarse_time_regs_shs2(veyecam_reg *regs,
				u32 coarse_time)
{
    return;

}

static inline void veyecam_get_gain_reg(veyecam_reg *regs,
				u8 gain)
{
    return;
}

//static int test_mode;
//module_param(test_mode, int, 0644);

static inline int veyecam_read_reg(struct camera_common_data *s_data,
				u16 addr, u8 *val)
{
	int err = 0;
	u32 reg_val = 0;
	err = regmap_read(s_data->regmap, addr, &reg_val);
	*val = reg_val & 0xFF;
    VEYE_TRACE
	return err;
}

static int veyecam_write_reg(struct camera_common_data *s_data,
				u16 addr, u8 val)
{
	int err;
	struct device *dev = s_data->dev;
	err = regmap_write(s_data->regmap, addr, val);
	if (err)
		dev_err(dev, "%s: i2c write failed, 0x%x = %x\n",
			__func__, addr, val);
    VEYE_TRACE
	return err;
}

static int veyecam_write_table(struct veyecam *priv,
				const veyecam_reg table[])
{
	struct camera_common_data *s_data = priv->s_data;
    
	return regmap_util_write_table_8(s_data->regmap,
					 table,
					 NULL, 0,
					 VEYECAM_TABLE_WAIT_MS,
					 VEYECAM_TABLE_END);
}

static int veyecam_set_group_hold(struct tegracam_device *tc_dev, bool val)
{
    VEYE_TRACE
	return 0;
}
#if 0
static int veyecam_set_gain(struct tegracam_device *tc_dev, s64 val)
{
    #if 0
	struct camera_common_data *s_data = tc_dev->s_data;
	struct veyecam *priv = (struct veyecam *)tc_dev->priv;
	struct device *dev = tc_dev->dev;
	const struct sensor_mode_properties *mode =
		&s_data->sensor_props.sensor_modes[s_data->mode_prop_idx];
	veyecam_reg reg_list[1];
	int err;
	u8 gain;

	/* translate value */
	gain = (u8) (val * 160 / (48 * mode->control_properties.gain_factor));
	dev_dbg(dev, "%s:  gain reg: %d\n",  __func__, gain);

	veyecam_get_gain_reg(reg_list, gain);

	err = veyecam_write_reg(priv->s_data, reg_list[0].addr,
		 reg_list[0].val);
	if (err)
		goto fail;
    
	return 0;

fail:
	dev_dbg(dev, "%s: GAIN control error\n", __func__);
	return err;
    #endif
    VEYE_TRACE
    return 0;
}
#endif
#if 0
static int veyecam_set_coarse_time(struct veyecam *priv, s64 val)
{
    
	struct camera_common_data *s_data = priv->s_data;
	const struct sensor_mode_properties *mode =
		&s_data->sensor_props.sensor_modes[s_data->mode];
	struct device *dev = &priv->i2c_client->dev;
	veyecam_reg reg_list[3];
	int err;
	u32 coarse_time_shs1;
	u32 reg_shs1;
	int i = 0;

	coarse_time_shs1 = mode->signal_properties.pixel_clock.val *
		val / mode->image_properties.line_length /
		mode->control_properties.exposure_factor;

	if (priv->frame_length == 0)
		priv->frame_length = VEYECAM_MIN_FRAME_LENGTH;

	reg_shs1 = priv->frame_length - coarse_time_shs1 - 1;

	dev_dbg(dev, "%s: coarse1:%d, shs1:%d, FL:%d\n", __func__,
		 coarse_time_shs1, reg_shs1, priv->frame_length);

	veyecam_get_coarse_time_regs_shs1(reg_list, reg_shs1);

	for (i = 0; i < 3; i++) {
		err = veyecam_write_reg(priv->s_data, reg_list[i].addr,
			 reg_list[i].val);
		if (err)
			goto fail;
	}

	return 0;

fail:
	dev_dbg(dev, "%s: set coarse time error\n", __func__);
	return err;
    
    VEYE_TRACE
    return 0;
}
#endif
#if 0
static int veyecam_set_coarse_time_hdr(struct veyecam *priv, s64 val)
{
	struct device *dev = &priv->i2c_client->dev;
	struct camera_common_data *s_data = priv->s_data;
	const struct sensor_mode_properties *mode =
		&s_data->sensor_props.sensor_modes[s_data->mode];
	veyecam_reg reg_list_shs1[3];
	veyecam_reg reg_list_shs2[3];
	u32 coarse_time_shs1;
	u32 coarse_time_shs2;
	u32 reg_shs1;
	u32 reg_shs2;
	int err;
	int i = 0;

	if (priv->frame_length == 0)
		priv->frame_length = VEYECAM_MIN_FRAME_LENGTH;

	priv->last_wdr_et_val = val;

	/*WDR, update SHS1 as short ET, and SHS2 is 16x of short*/
	coarse_time_shs1 = mode->signal_properties.pixel_clock.val *
		val / mode->image_properties.line_length /
		mode->control_properties.exposure_factor / 16;
	if (coarse_time_shs1 < VEYECAM_MIN_SHS1_1080P_HDR)
		coarse_time_shs1 = VEYECAM_MIN_SHS1_1080P_HDR;
	if (coarse_time_shs1 > VEYECAM_MAX_SHS1_1080P_HDR)
		coarse_time_shs1 = VEYECAM_MAX_SHS1_1080P_HDR;

	coarse_time_shs2 = (coarse_time_shs1 - VEYECAM_MIN_SHS1_1080P_HDR) * 16 +
				VEYECAM_MIN_SHS2_1080P_HDR;

	reg_shs1 = priv->frame_length - coarse_time_shs1 - 1;
	reg_shs2 = priv->frame_length - coarse_time_shs2 - 1;

	veyecam_get_coarse_time_regs_shs1(reg_list_shs1, reg_shs1);
	veyecam_get_coarse_time_regs_shs2(reg_list_shs2, reg_shs2);

	dev_dbg(dev, "%s: coarse1:%d, shs1:%d, coarse2:%d, shs2: %d, FL:%d\n",
		__func__,
		 coarse_time_shs1, reg_shs1,
		 coarse_time_shs2, reg_shs2,
		 priv->frame_length);

	for (i = 0; i < 3; i++) {
		err = veyecam_write_reg(priv->s_data, reg_list_shs1[i].addr,
			 reg_list_shs1[i].val);
		if (err)
			goto fail;

		err = veyecam_write_reg(priv->s_data, reg_list_shs2[i].addr,
			 reg_list_shs2[i].val);
		if (err)
			goto fail;
	}
	return 0;

fail:
	dev_dbg(dev, "%s: set WDR coarse time error\n", __func__);
	return err;
    
    VEYE_TRACE
    return 0;
}
#endif
#if 0
static int veyecam_set_frame_rate(struct tegracam_device *tc_dev, s64 val)
{
    #if 0
	struct camera_common_data *s_data = tc_dev->s_data;
	struct veyecam *priv = (struct veyecam *)tc_dev->priv;
	struct device *dev = tc_dev->dev;
	veyecam_reg reg_list[3];
	int err;
	u32 frame_length;
	const struct sensor_mode_properties *mode =
		&s_data->sensor_props.sensor_modes[s_data->mode_prop_idx];
	struct v4l2_control control;
	int hdr_en;
	int i = 0;

	frame_length = mode->signal_properties.pixel_clock.val *
		mode->control_properties.framerate_factor /
		mode->image_properties.line_length / val;

	priv->frame_length = frame_length;
	if (priv->frame_length > VEYECAM_MAX_FRAME_LENGTH)
		priv->frame_length = VEYECAM_MAX_FRAME_LENGTH;

	dev_dbg(dev, "%s: val: %lld, , frame_length: %d\n", __func__,
		val, priv->frame_length);

	veyecam_get_frame_length_regs(reg_list, priv->frame_length);

	for (i = 0; i < 3; i++) {
		err = veyecam_write_reg(priv->s_data, reg_list[i].addr,
			 reg_list[i].val);
		if (err)
			goto fail;
	}

	/* check hdr enable ctrl */
	control.id = TEGRA_CAMERA_CID_HDR_EN;
	err = camera_common_g_ctrl(s_data, &control);
	if (err < 0) {
		dev_err(dev, "could not find device ctrl.\n");
		return err;
	}

	hdr_en = switch_ctrl_qmenu[control.value];
	if ((hdr_en == SWITCH_ON) && (priv->last_wdr_et_val != 0)) {
		err = veyecam_set_coarse_time_hdr(priv, priv->last_wdr_et_val);
		if (err)
			dev_dbg(dev,
			"%s: error coarse time SHS1 SHS2 override\n", __func__);
	}
    
	return 0;

fail:
	dev_dbg(dev, "%s: FRAME_LENGTH control error\n", __func__);
	return err;
    #endif
    VEYE_TRACE
    return 0;
}

static int veyecam_set_exposure(struct tegracam_device *tc_dev, s64 val)
{
    #if 0
	struct camera_common_data *s_data = tc_dev->s_data;
	struct veyecam *priv = (struct veyecam *)tc_dev->priv;
	struct device *dev = tc_dev->dev;
	int err;
	struct v4l2_control control;
	int hdr_en;

	dev_dbg(dev, "%s: val: %lld\n", __func__, val);

	/* check hdr enable ctrl */
	control.id = TEGRA_CAMERA_CID_HDR_EN;
	err = camera_common_g_ctrl(s_data, &control);
	if (err < 0) {
		dev_err(dev, "could not find device ctrl.\n");
		return err;
	}

	hdr_en = switch_ctrl_qmenu[control.value];
	if (hdr_en == SWITCH_ON) {
		err = veyecam_set_coarse_time_hdr(priv, val);
		if (err)
			dev_dbg(dev,
			"%s: error coarse time SHS1 SHS2 override\n", __func__);
	} else {
		err = veyecam_set_coarse_time(priv, val);
		if (err)
			dev_dbg(dev,
			"%s: error coarse time SHS1 override\n", __func__);
	}

	return err;
    VEYE_TRACE
     #endif
    return 0;
}
  #endif
static int veyecam_fill_string_ctrl(struct tegracam_device *tc_dev,
				struct v4l2_ctrl *ctrl)
{
    #if 0
	struct veyecam *priv = tc_dev->priv;
	int i;

	switch (ctrl->id) {
	case TEGRA_CAMERA_CID_FUSE_ID:
		for (i = 0; i < VEYECAM_FUSE_ID_SIZE; i++)
			sprintf(&ctrl->p_new.p_char[i*2], "%02x",
				priv->fuse_id[i]);
		break;
	default:
		return -EINVAL;
	}

	ctrl->p_cur.p_char = ctrl->p_new.p_char;
    #endif
    VEYE_TRACE
	return 0;
}

static struct tegracam_ctrl_ops veyecam_ctrl_ops = {
	.numctrls = ARRAY_SIZE(ctrl_cid_list),
	.ctrl_cid_list = ctrl_cid_list,
	//.string_ctrl_size = {0, VEYECAM_FUSE_ID_STR_SIZE},
	//.set_gain = veyecam_set_gain,
	//.set_exposure = veyecam_set_exposure,
	//.set_frame_rate = veyecam_set_frame_rate,
	.set_group_hold = veyecam_set_group_hold,
	.fill_string_ctrl = veyecam_fill_string_ctrl,
};

static int veyecam_power_on(struct camera_common_data *s_data)
{
	//int err = 0;
	struct camera_common_power_rail *pw = s_data->power;
	//struct camera_common_pdata *pdata = s_data->pdata;
	struct device *dev = s_data->dev;

    pw->state = SWITCH_ON;

	dev_dbg(dev, "%s: power on\n", __func__);
	/*if (pdata && pdata->power_on) {
		err = pdata->power_on(pw);
		if (err)
			dev_err(dev, "%s failed.\n", __func__);
		else
			pw->state = SWITCH_ON;
		return err;
	}*/

	/*exit reset mode: XCLR */
	if (pw->reset_gpio) {
		//gpio_set_value(pw->reset_gpio, 0);
		//usleep_range(30, 50);
		gpio_set_value(pw->reset_gpio, 1);
        debug_printk("veyecam set resetio high!\n");
		msleep(1);
	}else{
        debug_printk("veyecam resetio not recognize!\n");
    }

	pw->state = SWITCH_ON;
    VEYE_TRACE
	return 0;

}

static int veyecam_power_off(struct camera_common_data *s_data)
{
	//int err = 0;
	struct camera_common_power_rail *pw = s_data->power;
	//struct camera_common_pdata *pdata = s_data->pdata;
	//struct device *dev = s_data->dev;

    pw->state = SWITCH_OFF;
    VEYE_TRACE
	return 0;
    #if 0
	dev_dbg(dev, "%s: power off\n", __func__);

	/*if (pdata && pdata->power_off) {
		err = pdata->power_off(pw);
		if (!err)
			goto power_off_done;
		else
			dev_err(dev, "%s failed.\n", __func__);
		return err;
	}*/
	/* enter reset mode: XCLR */
	usleep_range(1, 2);
	if (pw->reset_gpio)
		gpio_set_value(pw->reset_gpio, 0);

power_off_done:
	pw->state = SWITCH_OFF;
    VEYE_TRACE
	return 0;
    #endif
}

static int veyecam_power_get(struct tegracam_device *tc_dev)
{
	struct device *dev = tc_dev->dev;
	struct camera_common_data *s_data = tc_dev->s_data;
	struct camera_common_power_rail *pw = s_data->power;
	struct camera_common_pdata *pdata = s_data->pdata;
	const char *mclk_name;
	struct clk *parent;
	int err = 0;
    if(pdata->mclk_name)
    {
        mclk_name = pdata->mclk_name ?
                pdata->mclk_name : "extperiph1";
        pw->mclk = devm_clk_get(dev, mclk_name);
        if (IS_ERR(pw->mclk)) {
            dev_err(dev, "unable to get clock %s\n", mclk_name);
            return PTR_ERR(pw->mclk);
        }
        parent = devm_clk_get(dev, "pllp_grtba");
        if (IS_ERR(parent))
            dev_err(dev, "devm_clk_get failed for pllp_grtba");
        else
            clk_set_parent(pw->mclk, parent);
    }
	pw->reset_gpio = pdata->reset_gpio;

	pw->state = SWITCH_OFF;
    VEYE_TRACE
	return err;
}

static int veyecam_power_put(struct tegracam_device *tc_dev)
{
	struct camera_common_data *s_data = tc_dev->s_data;
	struct camera_common_power_rail *pw = s_data->power;

	if (unlikely(!pw))
		return -EFAULT;
    VEYE_TRACE
	return 0;
}

static struct camera_common_pdata *veyecam_parse_dt(struct tegracam_device *tc_dev)
{
	struct device *dev = tc_dev->dev;
	struct device_node *np = dev->of_node;
	struct camera_common_pdata *board_priv_pdata;
	const struct of_device_id *match;
	//struct camera_common_pdata *ret = NULL;
	int err;
	int gpio;

	if (!np)
		return NULL;

	match = of_match_device(veyecam_of_match, dev);
	if (!match) {
		dev_err(dev, "Failed to find matching dt id\n");
		return NULL;
	}

	board_priv_pdata = devm_kzalloc(dev,
					sizeof(*board_priv_pdata), GFP_KERNEL);
	if (!board_priv_pdata)
		return NULL;

	gpio = of_get_named_gpio(np, "reset-gpios", 0);
	if (gpio < 0) {
		//if (gpio == -EPROBE_DEFER)
		//	ret = ERR_PTR(-EPROBE_DEFER);
		dev_err(dev, "reset-gpios not found \n");
        board_priv_pdata->reset_gpio = 0;
		//goto error;
	}else{
        board_priv_pdata->reset_gpio = (unsigned int)gpio;
    }
    
	err = of_property_read_string(np, "mclk", &board_priv_pdata->mclk_name);
	if (err)
		dev_dbg(dev, "mclk name not present, "
			"assume sensor driven externally\n");

	err = of_property_read_string(np, "avdd-reg",
		&board_priv_pdata->regulators.avdd);
	err |= of_property_read_string(np, "iovdd-reg",
		&board_priv_pdata->regulators.iovdd);
	err |= of_property_read_string(np, "dvdd-reg",
		&board_priv_pdata->regulators.dvdd);
	if (err)
		dev_dbg(dev, "avdd, iovdd and/or dvdd reglrs. not present, "
			"assume sensor powered independently\n");

	board_priv_pdata->has_eeprom =
		of_property_read_bool(np, "has-eeprom");
    VEYE_TRACE
	return board_priv_pdata;

/*error:
	devm_kfree(dev, board_priv_pdata);
	return ret;*/
}

static int veyecam_set_mode(struct tegracam_device *tc_dev)
{
	struct veyecam *priv = (struct veyecam *)tegracam_get_privdata(tc_dev);
	struct camera_common_data *s_data = tc_dev->s_data;
	struct device *dev = tc_dev->dev;
	struct device_node *np = dev->of_node;
	bool limit_analog_gain = false;
	const struct of_device_id *match;
	int err;

	match = of_match_device(veyecam_of_match, dev);
	if (!match) {
		dev_err(dev, "Failed to find matching dt id\n");
		return -EINVAL;
	}

	limit_analog_gain = of_property_read_bool(np, "limit_analog_gain");

	err = veyecam_write_table(priv, mode_table[s_data->mode_prop_idx]);
	if (err)
		return err;

	/*if (limit_analog_gain) {
		err = veyecam_write_reg(priv->s_data,
			VEYECAM_ANALOG_GAIN_LIMIT_ADDR,
			VEYECAM_ANALOG_GAIN_LIMIT_VALUE);
		if (err)
			return err;
	}*/
    VEYE_TRACE
	return 0;
}

static int veyecam_start_streaming(struct tegracam_device *tc_dev)
{
	struct veyecam *priv = (struct veyecam *)tegracam_get_privdata(tc_dev);
	int err;

	/*if (test_mode) {
		err = veyecam_write_table(priv,
			mode_table[VEYECAM_MODE_TEST_PATTERN]);
		if (err)
			return err;
	}*/

	err = veyecam_write_table(priv,
		mode_table[VEYECAM_MODE_START_STREAM]);
	if (err)
		return err;
    VEYE_TRACE
	return 0;
}

static int veyecam_stop_streaming(struct tegracam_device *tc_dev)
{
	//struct camera_common_data *s_data = tc_dev->s_data;
	struct veyecam *priv = (struct veyecam *)tegracam_get_privdata(tc_dev);
	int err;

	err = veyecam_write_table(priv, mode_table[VEYECAM_MODE_STOP_STREAM]);
	if (err)
		return err;

	/* SW_RESET will have no ACK */
	//veyecam_write_reg(s_data, VEYECAM_SW_RESET_ADDR, 0x01);

	/*
	 * Wait for one frame to make sure sensor is set to
	 * software standby in V-blank
	 *
	 * delay = frame length rows * Tline (10 us)
	 */
	usleep_range(priv->frame_length * 10, priv->frame_length * 10 + 1000);
    VEYE_TRACE
	return 0;
}


static struct camera_common_sensor_ops veyecam_common_ops = {
	.numfrmfmts = ARRAY_SIZE(veyecam_frmfmt),
	.frmfmt_table = veyecam_frmfmt,
	.power_on = veyecam_power_on,
	.power_off = veyecam_power_off,
	.write_reg = veyecam_write_reg,
	.read_reg = veyecam_read_reg,
	.parse_dt = veyecam_parse_dt,
	.power_get = veyecam_power_get,
	.power_put = veyecam_power_put,
	.set_mode = veyecam_set_mode,
	.start_streaming = veyecam_start_streaming,
	.stop_streaming = veyecam_stop_streaming,
};

static int veyecam_fuse_id_setup(struct veyecam *priv)
{
    # if 0
	int err;
	int i;
	struct camera_common_data *s_data = priv->s_data;
	struct device *dev = s_data->dev;
	u8 bak = 0;

	for (i = 0; i < VEYECAM_FUSE_ID_SIZE; i++) {
		err |= veyecam_read_reg(s_data,
			VEYECAM_FUSE_ID_ADDR + i, &bak);
		if (!err)
			priv->fuse_id[i] = bak;
		else {
			dev_err(dev, "%s: can not read fuse id\n", __func__);
			return -EINVAL;
		}
	}
    #endif
    VEYE_TRACE
	return 0;
}
static int veyecam_read_model(struct veyecam *priv)
{
    struct camera_common_data *s_data = priv->s_data;
	struct device *dev = s_data->dev;
	int ret;
    u8 snr_l;
    u8 snr_h;
    u8 board_no;
    ret = veyecam_read_reg(s_data, SENSOR_TYPR_ADDR_L, &snr_l);
	if (ret) {
		dev_err(dev, "probe failed \n");
		return -ENODEV;
	}
    ret = veyecam_read_reg(s_data, SENSOR_TYPR_ADDR_H, &snr_h);
	if (ret) {
		dev_err(dev, "probe failed \n");
		return -ENODEV;
	}
    ret = veyecam_read_reg(s_data, BOARD_TYPR_ADDR, &board_no);
	if (ret) {
		dev_err(dev, "probe failed \n");
		return -ENODEV;
	}
    if(snr_l == 0x03 && snr_h == 0x27){
        dev_err(dev, "sensor is IMX327\n");
    }
    else if(snr_l == 0x04 && snr_h == 0x62){
        dev_err(dev, "sensor is IMX462\n");
    }
     else if(snr_l == 0x03 && snr_h == 0x85){
        dev_err(dev, "sensor is IMX385\n");
    }
     if(board_no == 0x4C){
        dev_err(dev, "board type is ONE board\n");
    }else{
        dev_err(dev, "board type is TWO board\n");
    }
    return 0;
}
/*verify hdver*/
static int veyecam_check_version(struct veyecam *priv)
{
    struct camera_common_data *s_data = priv->s_data;
	struct device *dev = s_data->dev;
    int ret;
    u8 hdver;
    ret = veyecam_read_reg(s_data, VEYECAM_HDVER_ADDR, &hdver);
	if (ret) {
		dev_err(dev, "check hdver error \n");
		return -ENODEV;
	}
    dev_info(dev, "check hdver %d \n",hdver);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
    if(hdver <=0x6){
        dev_err(dev, "Xavier and Orin have stricter mipi timing verification, please use a newer version(>=7) camera.\n");
    }
#endif
    return 0;
}
/* Verify chip ID */
static int veyecam_identify_module(struct veyecam *priv)
{
    struct camera_common_data *s_data = priv->s_data;
	struct device *dev = s_data->dev;
	int ret;
	//u32 val;
    int err;
    u8 device_id;
    VEYE_TRACE
	ret = veyecam_read_reg(s_data, VEYECAM_MODEL_ID_ADDR, &device_id);
	if (ret) {
		dev_err(dev, "probe failed \n");
		return -ENODEV;
	}
    if (device_id == VEYECAM_DEVICE_ID) 
    {
        err = 0;
        dev_err(dev, " camera id is veyecam\n");
    }
    else
    {
        err = -ENODEV;
		dev_err(dev, "%s: invalid sensor model id: %d\n",
			__func__, device_id);
    }
    veyecam_read_model(priv);
    veyecam_check_version(priv);
	return err;
}

static int veyecam_board_setup(struct veyecam *priv)
{
	struct camera_common_data *s_data = priv->s_data;
	struct device *dev = s_data->dev;
    struct camera_common_pdata *pdata = s_data->pdata;
	int err = 0;
   // u8 reg_val[2];
	dev_dbg(dev, "%s++\n", __func__);

	err = camera_common_mclk_enable(s_data);
	if (err) {
		dev_err(dev,
			"Error %d turning on mclk\n", err);
		return err;
	}

	err = veyecam_power_on(s_data);
	if (err) {
		dev_err(dev,
			"Error %d during power on sensor\n", err);
		return err;
	}

	err = veyecam_fuse_id_setup(priv);
	if (err) {
		dev_err(dev,
			"Error %d reading fuse id data\n", err);
		goto err_power_on;
	}
	/* Probe sensor model id registers */
    err = veyecam_identify_module(priv);
	if (err)
		goto err_reg_probe;
    VEYE_TRACE
err_reg_probe:
	veyecam_power_off(s_data);

err_power_on:
	if (pdata->mclk_name)
        camera_common_mclk_disable(s_data);

	return err;
}

static int veyecam_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	dev_dbg(&client->dev, "%s:\n", __func__);
    VEYE_TRACE
	return 0;
}

static const struct v4l2_subdev_internal_ops veyecam_subdev_internal_ops = {
	.open = veyecam_open,
};

static int veyecam_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct tegracam_device *tc_dev;
	struct veyecam *priv;
	int err;
	dev_info(dev, "probing v4l2 sensor\n");

	if (!IS_ENABLED(CONFIG_OF) || !client->dev.of_node)
		return -EINVAL;

	priv = devm_kzalloc(dev,
			sizeof(struct veyecam), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	tc_dev = devm_kzalloc(dev,
			sizeof(struct tegracam_device), GFP_KERNEL);
	if (!tc_dev)
		return -ENOMEM;

	priv->i2c_client = tc_dev->client = client;
	tc_dev->dev = dev;
	strncpy(tc_dev->name, "veyecam", sizeof(tc_dev->name));
	tc_dev->dev_regmap_config = &sensor_regmap_config;
	tc_dev->sensor_ops = &veyecam_common_ops;
	tc_dev->v4l2sd_internal_ops = &veyecam_subdev_internal_ops;
	tc_dev->tcctrl_ops = &veyecam_ctrl_ops;

	err = tegracam_device_register(tc_dev);
	if (err) {
		dev_err(dev, "tegra camera driver registration failed %d\n",err);
		return err;
	}
	priv->tc_dev = tc_dev;
	priv->s_data = tc_dev->s_data;
	priv->subdev = &tc_dev->s_data->subdev;
	tegracam_set_privdata(tc_dev, (void *)priv);

	err = veyecam_board_setup(priv);
	if (err) {
		dev_err(dev, "board setup failed\n");
		return err;
	}
    //make sure it is continues mode
    veyecam_write_reg(priv->s_data, 0x0b, 0xFF);
    
	err = tegracam_v4l2subdev_register(tc_dev, true);
	if (err) {
		dev_err(dev, "tegra camera subdev registration failed\n");
		return err;
	}

	dev_info(dev, "Detected VEYECAM sensor\n");

	return 0;
}

static int
veyecam_remove(struct i2c_client *client)
{
	struct camera_common_data *s_data = to_camera_common_data(&client->dev);
	struct veyecam *priv = (struct veyecam *)s_data->priv;

	tegracam_v4l2subdev_unregister(priv->tc_dev);
	tegracam_device_unregister(priv->tc_dev);
    VEYE_TRACE
	return 0;
}

static const struct i2c_device_id veyecam_id[] = {
	{ "veyecam", 0 },
	{ }
};

MODULE_DEVICE_TABLE(i2c, veyecam_id);

static struct i2c_driver veyecam_i2c_driver = {
	.driver = {
		.name = "veyecam",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(veyecam_of_match),
	},
	.probe = veyecam_probe,
	.remove = veyecam_remove,
	.id_table = veyecam_id,
};

module_i2c_driver(veyecam_i2c_driver);

MODULE_DESCRIPTION("Media Controller driver for CSONEPLUS VEYECAM");
MODULE_AUTHOR("xumm@csoneplus.com  Tianjin Zhonganyijia Tech Co.,Ltd.");
MODULE_LICENSE("GPL v2");
