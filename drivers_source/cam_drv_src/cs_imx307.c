/*
 * veye327.c - veye327 sensor driver
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
#include "cs_imx307.h"

#define CREATE_TRACE_POINTS
//#include <trace/events/veye327.h>

//#define CS307_FUSE_ID_ADDR	0x3382
//#define CS307_FUSE_ID_SIZE	6
//#define CS307_FUSE_ID_STR_SIZE	(CS307_FUSE_ID_SIZE * 2)

/* cs_imx307 sensor register address */
//#define CS307_MODEL_ID_ADDR		0x01


//#define DEBUG_PRINTK
#ifndef DEBUG_PRINTK
#define debug_printk(s , ... )
#define VEYE_TRACE 
#else
#define debug_printk printk
#define VEYE_TRACE printk("%s %s %d \n",__FILE__,__FUNCTION__,__LINE__);
#endif


static const struct of_device_id cs_imx307_of_match[] = {
	{ .compatible = "nvidia,cs_imx307",},
	{ },
};
MODULE_DEVICE_TABLE(of, cs_imx307_of_match);

static const u32 ctrl_cid_list[] = {
	/*TEGRA_CAMERA_CID_GAIN,
	TEGRA_CAMERA_CID_EXPOSURE,
    TEGRA_CAMERA_CID_FRAME_RATE,
	TEGRA_CAMERA_CID_FUSE_ID,
	TEGRA_CAMERA_CID_HDR_EN,*/
	TEGRA_CAMERA_CID_SENSOR_MODE_ID,
};

struct cs_imx307 {
	struct i2c_client	*i2c_client;
	struct v4l2_subdev	*subdev;
	//u8 fuse_id[VEYE327_FUSE_ID_SIZE];
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

static inline void cs_imx307_get_frame_length_regs(cs307_reg *regs,
				u32 frame_length)
{
	/*regs->addr = VEYE327_FRAME_LENGTH_ADDR_MSB;
	regs->val = (frame_length >> 16) & 0x01;

	(regs + 1)->addr = VEYE327_FRAME_LENGTH_ADDR_MID;
	(regs + 1)->val = (frame_length >> 8) & 0xff;

	(regs + 2)->addr = VEYE327_FRAME_LENGTH_ADDR_LSB;
	(regs + 2)->val = (frame_length) & 0xff;*/
    VEYE_TRACE
    return ;
}

static inline void cs_imx307_get_coarse_time_regs_shs1(cs307_reg *regs,
				u32 coarse_time)
{
	/*regs->addr = VEYE327_COARSE_TIME_SHS1_ADDR_MSB;
	regs->val = (coarse_time >> 16) & 0x01;

	(regs + 1)->addr = VEYE327_COARSE_TIME_SHS1_ADDR_MID;
	(regs + 1)->val = (coarse_time >> 8) & 0xff;

	(regs + 2)->addr = VEYE327_COARSE_TIME_SHS1_ADDR_LSB;
	(regs + 2)->val = (coarse_time) & 0xff;*/
    VEYE_TRACE
    return ;
}

static inline void cs_imx307_get_coarse_time_regs_shs2(cs307_reg *regs,
				u32 coarse_time)
{
	/*regs->addr = VEYE327_COARSE_TIME_SHS2_ADDR_MSB;
	regs->val = (coarse_time >> 16) & 0x01;

	(regs + 1)->addr = VEYE327_COARSE_TIME_SHS2_ADDR_MID;
	(regs + 1)->val = (coarse_time >> 8) & 0xff;

	(regs + 2)->addr = VEYE327_COARSE_TIME_SHS2_ADDR_LSB;
	(regs + 2)->val = (coarse_time) & 0xff;*/
    VEYE_TRACE
    return;

}

static inline void cs_imx307_get_gain_reg(cs307_reg *regs,
				u8 gain)
{
	/*regs->addr = VEYE327_GAIN_ADDR;
	regs->val = (gain) & 0xff;*/
    VEYE_TRACE
    return;
}

//static int test_mode;
//module_param(test_mode, int, 0644);
static inline int cs_imx307_read_reg(struct camera_common_data *s_data,
				u16 addr, u8 *val)
{
	int err = 0;
	u32 reg_val = 0;
	err = regmap_read(s_data->regmap, addr, &reg_val);
	*val = reg_val & 0xFF;
    VEYE_TRACE
	return err;
}

static int cs_imx307_write_reg(struct camera_common_data *s_data,
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

static int cs_imx307_write_table(struct cs_imx307 *priv,
				const cs307_reg table[])
{
	struct camera_common_data *s_data = priv->s_data;
	return regmap_util_write_table_8(s_data->regmap,
					 table,
					 NULL, 0,
					 CS307_TABLE_WAIT_MS,
					 CS307_TABLE_END);
}

static int cs_imx307_set_group_hold(struct tegracam_device *tc_dev, bool val)
{
	/*struct camera_common_data *s_data = tc_dev->s_data;
	struct device *dev = tc_dev->dev;
	int err;

	err = veye327_write_reg(s_data,
				VEYE327_GROUP_HOLD_ADDR, val);
	if (err) {
		dev_dbg(dev,
			"%s: Group hold control error\n", __func__);
		return err;
	}*/
    VEYE_TRACE
	return 0;
}
#if 0
static int cs_imx307_set_gain(struct tegracam_device *tc_dev, s64 val)
{
    #if 0
	struct camera_common_data *s_data = tc_dev->s_data;
	struct cs_imx307 *priv = (struct cs_imx307 *)tc_dev->priv;
	struct device *dev = tc_dev->dev;
	const struct sensor_mode_properties *mode =
		&s_data->sensor_props.sensor_modes[s_data->mode_prop_idx];
	cs307_reg reg_list[1];
	int err;
	u8 gain;

	/* translate value */
	gain = (u8) (val * 160 / (48 * mode->control_properties.gain_factor));
	dev_dbg(dev, "%s:  gain reg: %d\n",  __func__, gain);

	cs_imx307_get_gain_reg(reg_list, gain);

	err = cs_imx307_write_reg(priv->s_data, reg_list[0].addr,
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

static int cs_imx307_set_coarse_time(struct cs_imx307 *priv, s64 val)
{
    
	struct camera_common_data *s_data = priv->s_data;
	const struct sensor_mode_properties *mode =
		&s_data->sensor_props.sensor_modes[s_data->mode];
	struct device *dev = &priv->i2c_client->dev;
	cs307_reg reg_list[3];
	int err;
	u32 coarse_time_shs1;
	u32 reg_shs1;
	int i = 0;

	coarse_time_shs1 = mode->signal_properties.pixel_clock.val *
		val / mode->image_properties.line_length /
		mode->control_properties.exposure_factor;

	if (priv->frame_length == 0)
		priv->frame_length = CS307_MIN_FRAME_LENGTH;

	reg_shs1 = priv->frame_length - coarse_time_shs1 - 1;

	dev_dbg(dev, "%s: coarse1:%d, shs1:%d, FL:%d\n", __func__,
		 coarse_time_shs1, reg_shs1, priv->frame_length);

	cs_imx307_get_coarse_time_regs_shs1(reg_list, reg_shs1);

	for (i = 0; i < 3; i++) {
		err = cs_imx307_write_reg(priv->s_data, reg_list[i].addr,
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
static int cs_imx307_set_coarse_time_hdr(struct cs_imx307 *priv, s64 val)
{
	struct device *dev = &priv->i2c_client->dev;
	struct camera_common_data *s_data = priv->s_data;
	const struct sensor_mode_properties *mode =
		&s_data->sensor_props.sensor_modes[s_data->mode];
	cs307_reg reg_list_shs1[3];
	cs307_reg reg_list_shs2[3];
	u32 coarse_time_shs1;
	u32 coarse_time_shs2;
	u32 reg_shs1;
	u32 reg_shs2;
	int err;
	int i = 0;

	if (priv->frame_length == 0)
		priv->frame_length = CS307_MIN_FRAME_LENGTH;

	priv->last_wdr_et_val = val;

	/*WDR, update SHS1 as short ET, and SHS2 is 16x of short*/
	coarse_time_shs1 = mode->signal_properties.pixel_clock.val *
		val / mode->image_properties.line_length /
		mode->control_properties.exposure_factor / 16;
	if (coarse_time_shs1 < CS307_MIN_SHS1_1080P_HDR)
		coarse_time_shs1 = CS307_MIN_SHS1_1080P_HDR;
	if (coarse_time_shs1 > CS307_MAX_SHS1_1080P_HDR)
		coarse_time_shs1 = CS307_MAX_SHS1_1080P_HDR;

	coarse_time_shs2 = (coarse_time_shs1 - CS307_MIN_SHS1_1080P_HDR) * 16 +
				CS307_MIN_SHS2_1080P_HDR;

	reg_shs1 = priv->frame_length - coarse_time_shs1 - 1;
	reg_shs2 = priv->frame_length - coarse_time_shs2 - 1;

	cs_imx307_get_coarse_time_regs_shs1(reg_list_shs1, reg_shs1);
	cs_imx307_get_coarse_time_regs_shs2(reg_list_shs2, reg_shs2);

	dev_dbg(dev, "%s: coarse1:%d, shs1:%d, coarse2:%d, shs2: %d, FL:%d\n",
		__func__,
		 coarse_time_shs1, reg_shs1,
		 coarse_time_shs2, reg_shs2,
		 priv->frame_length);

	for (i = 0; i < 3; i++) {
		err = cs_imx307_write_reg(priv->s_data, reg_list_shs1[i].addr,
			 reg_list_shs1[i].val);
		if (err)
			goto fail;

		err = cs_imx307_write_reg(priv->s_data, reg_list_shs2[i].addr,
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


static int cs_imx307_set_frame_rate(struct tegracam_device *tc_dev, s64 val)
{
    //debug_printk("veye327_set_frame_rate want set rate %d\n",val);
    #if 0
	struct camera_common_data *s_data = tc_dev->s_data;
	struct cs_imx307 *priv = (struct cs_imx307 *)tc_dev->priv;
	struct device *dev = tc_dev->dev;
	cs307_reg reg_list[3];
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
	if (priv->frame_length > CS307_MAX_FRAME_LENGTH)
		priv->frame_length = CS307_MAX_FRAME_LENGTH;

	dev_dbg(dev, "%s: val: %lld, , frame_length: %d\n", __func__,
		val, priv->frame_length);

	cs_imx307_get_frame_length_regs(reg_list, priv->frame_length);

	for (i = 0; i < 3; i++) {
		err = cs_imx307_write_reg(priv->s_data, reg_list[i].addr,
			 reg_list[i].val);
		if (err)
			goto fail;
	}
    
	/* check hdr enable ctrl */
    /*
	control.id = TEGRA_CAMERA_CID_HDR_EN;
	err = camera_common_g_ctrl(s_data, &control);
	if (err < 0) {
		dev_err(dev, "could not find device ctrl.\n");
		return err;
	}

	hdr_en = switch_ctrl_qmenu[control.value];
	if ((hdr_en == SWITCH_ON) && (priv->last_wdr_et_val != 0)) {
		err = cs_imx307_set_coarse_time_hdr(priv, priv->last_wdr_et_val);
		if (err)
			dev_dbg(dev,
			"%s: error coarse time SHS1 SHS2 override\n", __func__);
	}
    */
	return 0;

fail:
	dev_dbg(dev, "%s: FRAME_LENGTH control error\n", __func__);
	return err;
    #endif
    VEYE_TRACE
    return 0;
}

static int cs_imx307_set_exposure(struct tegracam_device *tc_dev, s64 val)
{
    #if 0
	struct camera_common_data *s_data = tc_dev->s_data;
	struct cs_imx307 *priv = (struct cs_imx307 *)tc_dev->priv;
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
		err = cs_imx307_set_coarse_time_hdr(priv, val);
		if (err)
			dev_dbg(dev,
			"%s: error coarse time SHS1 SHS2 override\n", __func__);
	} else {
		err = cs_imx307_set_coarse_time(priv, val);
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
static int cs_imx307_fill_string_ctrl(struct tegracam_device *tc_dev,
				struct v4l2_ctrl *ctrl)
{
    #if 0
	struct cs_imx307 *priv = tc_dev->priv;
	int i;

	switch (ctrl->id) {
	case TEGRA_CAMERA_CID_FUSE_ID:
		for (i = 0; i < CS307_FUSE_ID_SIZE; i++)
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

static struct tegracam_ctrl_ops cs_imx307_ctrl_ops = {
	.numctrls = ARRAY_SIZE(ctrl_cid_list),
	.ctrl_cid_list = ctrl_cid_list,
	//.string_ctrl_size = {0, CS307_FUSE_ID_STR_SIZE},
    //delete for jetpack 4.5
	//.set_gain = cs_imx307_set_gain,
	//.set_exposure = cs_imx307_set_exposure,
	//.set_frame_rate = cs_imx307_set_frame_rate,
	.set_group_hold = cs_imx307_set_group_hold,
	.fill_string_ctrl = cs_imx307_fill_string_ctrl,
};

static int cs_imx307_power_on(struct camera_common_data *s_data)
{
	int err = 0;
	struct camera_common_power_rail *pw = s_data->power;
	struct camera_common_pdata *pdata = s_data->pdata;
	struct device *dev = s_data->dev;

    pw->state = SWITCH_ON;
    VEYE_TRACE
	return 0;
    
	dev_dbg(dev, "%s: power on\n", __func__);
	if (pdata && pdata->power_on) {
		err = pdata->power_on(pw);
		if (err)
			dev_err(dev, "%s failed.\n", __func__);
		else
			pw->state = SWITCH_ON;
		return err;
	}

	/*exit reset mode: XCLR */
	if (pw->reset_gpio) {
		gpio_set_value(pw->reset_gpio, 0);
		usleep_range(30, 50);
		gpio_set_value(pw->reset_gpio, 1);
		usleep_range(30, 50);
	}

	pw->state = SWITCH_ON;
    VEYE_TRACE
	return 0;

}

static int cs_imx307_power_off(struct camera_common_data *s_data)
{
	int err = 0;
	struct camera_common_power_rail *pw = s_data->power;
	struct camera_common_pdata *pdata = s_data->pdata;
	struct device *dev = s_data->dev;

    pw->state = SWITCH_OFF;
    VEYE_TRACE
	return 0;
    
	dev_dbg(dev, "%s: power off\n", __func__);

	if (pdata && pdata->power_off) {
		err = pdata->power_off(pw);
		if (!err)
			goto power_off_done;
		else
			dev_err(dev, "%s failed.\n", __func__);
		return err;
	}
	/* enter reset mode: XCLR */
	usleep_range(1, 2);
	if (pw->reset_gpio)
		gpio_set_value(pw->reset_gpio, 0);

power_off_done:
	pw->state = SWITCH_OFF;
    VEYE_TRACE
	return 0;
}

static int cs_imx307_power_get(struct tegracam_device *tc_dev)
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

static int cs_imx307_power_put(struct tegracam_device *tc_dev)
{
	struct camera_common_data *s_data = tc_dev->s_data;
	struct camera_common_power_rail *pw = s_data->power;

	if (unlikely(!pw))
		return -EFAULT;
    VEYE_TRACE
	return 0;
}

static struct camera_common_pdata *cs_imx307_parse_dt(struct tegracam_device *tc_dev)
{
	struct device *dev = tc_dev->dev;
	struct device_node *np = dev->of_node;
	struct camera_common_pdata *board_priv_pdata;
	const struct of_device_id *match;
	//struct camera_common_pdata *ret = NULL;
	int err;
	//int gpio;

	if (!np)
		return NULL;

	match = of_match_device(cs_imx307_of_match, dev);
	if (!match) {
		dev_err(dev, "Failed to find matching dt id\n");
		return NULL;
	}

	board_priv_pdata = devm_kzalloc(dev,
					sizeof(*board_priv_pdata), GFP_KERNEL);
	if (!board_priv_pdata)
		return NULL;

	/*gpio = of_get_named_gpio(np, "reset-gpios", 0);
	if (gpio < 0) {
		if (gpio == -EPROBE_DEFER)
			ret = ERR_PTR(-EPROBE_DEFER);
		dev_err(dev, "reset-gpios not found \n");
		goto error;
	}
	board_priv_pdata->reset_gpio = (unsigned int)gpio;*/
    //no reset gpio
    board_priv_pdata->reset_gpio = 0;
    
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

static int cs_imx307_set_mode(struct tegracam_device *tc_dev)
{
	struct cs_imx307 *priv = (struct cs_imx307 *)tegracam_get_privdata(tc_dev);
	struct camera_common_data *s_data = tc_dev->s_data;
	struct device *dev = tc_dev->dev;
	struct device_node *np = dev->of_node;
	bool limit_analog_gain = false;
	const struct of_device_id *match;
	int err;

	match = of_match_device(cs_imx307_of_match, dev);
	if (!match) {
		dev_err(dev, "Failed to find matching dt id\n");
		return -EINVAL;
	}

	limit_analog_gain = of_property_read_bool(np, "limit_analog_gain");
    
	err = cs_imx307_write_table(priv, mode_table[s_data->mode_prop_idx]);
	if (err)
		return err;

	/*if (limit_analog_gain) {
		err = veye327_write_reg(priv->s_data,
			CS307_ANALOG_GAIN_LIMIT_ADDR,
			CS307_ANALOG_GAIN_LIMIT_VALUE);
		if (err)
			return err;
	}*/
    VEYE_TRACE
	return 0;
}

static int cs_imx307_start_streaming(struct tegracam_device *tc_dev)
{
	struct cs_imx307 *priv = (struct cs_imx307 *)tegracam_get_privdata(tc_dev);
	int err;

	/*if (test_mode) {
		err = veye327_write_table(priv,
			mode_table[CS307_MODE_TEST_PATTERN]);
		if (err)
			return err;
	}*/

	err = cs_imx307_write_table(priv,
		mode_table[CS307_MODE_START_STREAM]);
	if (err)
		return err;
    VEYE_TRACE
	return 0;
}

static int cs_imx307_stop_streaming(struct tegracam_device *tc_dev)
{
	//struct camera_common_data *s_data = tc_dev->s_data;
	struct cs_imx307 *priv = (struct cs_imx307 *)tegracam_get_privdata(tc_dev);
	int err;

	err = cs_imx307_write_table(priv, mode_table[CS307_MODE_STOP_STREAM]);
	if (err)
		return err;

	/* SW_RESET will have no ACK */
	//veye327_write_reg(s_data, CS307_SW_RESET_ADDR, 0x01);

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


static struct camera_common_sensor_ops cs_imx307_common_ops = {
	.numfrmfmts = ARRAY_SIZE(cs_imx307_frmfmt),
	.frmfmt_table = cs_imx307_frmfmt,
	.power_on = cs_imx307_power_on,
	.power_off = cs_imx307_power_off,
	.write_reg = cs_imx307_write_reg,
	.read_reg = cs_imx307_read_reg,
	.parse_dt = cs_imx307_parse_dt,
	.power_get = cs_imx307_power_get,
	.power_put = cs_imx307_power_put,
	.set_mode = cs_imx307_set_mode,
	.start_streaming = cs_imx307_start_streaming,
	.stop_streaming = cs_imx307_stop_streaming,
};

static int cs_imx307_fuse_id_setup(struct cs_imx307 *priv)
{
    //读一下id试试？
    # if 0
	int err;
	int i;
	struct camera_common_data *s_data = priv->s_data;
	struct device *dev = s_data->dev;
	u8 bak = 0;

	for (i = 0; i < CS307_FUSE_ID_SIZE; i++) {
		err |= cs_imx307_read_reg(s_data,
			CS307_FUSE_ID_ADDR + i, &bak);
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

static int cs_imx307_board_setup(struct cs_imx307 *priv)
{
	struct camera_common_data *s_data = priv->s_data;
	struct device *dev = s_data->dev;
    struct camera_common_pdata *pdata = s_data->pdata;
	int err = 0;
    u8 reg_val[2];
    u16 cameraid = 0;
	dev_dbg(dev, "%s++\n", __func__);

	err = camera_common_mclk_enable(s_data);
	if (err) {
		dev_err(dev,
			"Error %d turning on mclk\n", err);
		return err;
	}

	err = cs_imx307_power_on(s_data);
	if (err) {
		dev_err(dev,
			"Error %d during power on sensor\n", err);
		return err;
	}

	err = cs_imx307_fuse_id_setup(priv);
	if (err) {
		dev_err(dev,
			"Error %d reading fuse id data\n", err);
		goto err_power_on;
	}
	/* Probe sensor model id registers */
	err = cs_imx307_read_reg(s_data, PRODUCTID_L, &reg_val[0]);
	if (err) {
		dev_err(dev, "%s: error during i2c read probe (%d)\n",
			__func__, err);
		goto err_reg_probe;
	}
     err = cs_imx307_read_reg(s_data, PRODUCTID_H, &reg_val[1]);
    if (err) {
		dev_err(dev, "%s: error during i2c read probe (%d)\n",
			__func__, err);
		goto err_reg_probe;
	}
    cameraid = ((u16)reg_val[1]<<8) + reg_val[0];
	dev_err(dev,"read sensor id %04x \n", cameraid);
	if (cameraid == CS_MIPI_IMX307) 
    {
        err = 0;
        dev_err(dev, " camera id is cs-mipi-imx307\n");
    }
    else
    {
        err = -ENODEV;
		dev_err(dev, "%s: invalid sensor model id: %d\n",
			__func__, cameraid);
    }
    VEYE_TRACE
err_reg_probe:
	cs_imx307_power_off(s_data);

err_power_on:
	if (pdata->mclk_name)
        camera_common_mclk_disable(s_data);

	return err;
}

static int cs_imx307_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	dev_dbg(&client->dev, "%s:\n", __func__);
    VEYE_TRACE
	return 0;
}

static const struct v4l2_subdev_internal_ops cs_imx307_subdev_internal_ops = {
	.open = cs_imx307_open,
};

static int cs_imx307_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct tegracam_device *tc_dev;
	struct cs_imx307 *priv;
	int err;
	dev_info(dev, "probing v4l2 sensor\n");
    VEYE_TRACE
	if (!IS_ENABLED(CONFIG_OF) || !client->dev.of_node)
		return -EINVAL;

	priv = devm_kzalloc(dev,
			sizeof(struct cs_imx307), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	tc_dev = devm_kzalloc(dev,
			sizeof(struct tegracam_device), GFP_KERNEL);
	if (!tc_dev)
		return -ENOMEM;

	priv->i2c_client = tc_dev->client = client;
	tc_dev->dev = dev;
	strncpy(tc_dev->name, "csx307", sizeof(tc_dev->name));
	tc_dev->dev_regmap_config = &sensor_regmap_config;
	tc_dev->sensor_ops = &cs_imx307_common_ops;
	tc_dev->v4l2sd_internal_ops = &cs_imx307_subdev_internal_ops;
	tc_dev->tcctrl_ops = &cs_imx307_ctrl_ops;

	err = tegracam_device_register(tc_dev);
	if (err) {
		dev_err(dev, "tegra camera driver registration failed %d\n",err);
		return err;
	}
	priv->tc_dev = tc_dev;
	priv->s_data = tc_dev->s_data;
	priv->subdev = &tc_dev->s_data->subdev;
	tegracam_set_privdata(tc_dev, (void *)priv);

	err = cs_imx307_board_setup(priv);
	if (err) {
		dev_err(dev, "board setup failed\n");
		return err;
	}

	err = tegracam_v4l2subdev_register(tc_dev, true);
	if (err) {
		dev_err(dev, "tegra camera subdev registration failed\n");
		return err;
	}

	dev_info(dev, "Detected CS307 sensor\n");

	return 0;
}

static int
cs_imx307_remove(struct i2c_client *client)
{
	struct camera_common_data *s_data = to_camera_common_data(&client->dev);
	struct cs_imx307 *priv = (struct cs_imx307 *)s_data->priv;

	tegracam_v4l2subdev_unregister(priv->tc_dev);
	tegracam_device_unregister(priv->tc_dev);
    VEYE_TRACE
	return 0;
}

static const struct i2c_device_id cs_imx307_id[] = {
	{ "csx307", 0 },
	{ }
};

MODULE_DEVICE_TABLE(i2c, cs_imx307_id);

static struct i2c_driver cs_imx307_i2c_driver = {
	.driver = {
		.name = "csx307",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(cs_imx307_of_match),
	},
	.probe = cs_imx307_probe,
	.remove = cs_imx307_remove,
	.id_table = cs_imx307_id,
};

module_i2c_driver(cs_imx307_i2c_driver);

MODULE_DESCRIPTION("Media Controller driver for CSONEPLUS CS-MIPI-IMX307");
MODULE_AUTHOR("xumm@csoneplus.com  Tianjin Zhonganyijia Tech Co.,Ltd.");
MODULE_LICENSE("GPL v2");
