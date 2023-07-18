/*
 * ds90ub954.c - TI DS90UB954 deserializer and DS90UB953 serializer driver
 *
 * Copyright (c) 2020, Institut of Embedded Systems ZHAW
 *
 * This program is for the DS90UB954 FDP Link III deserializer in connection
 * with the DS90UB953 serializer from Texas Instruments
 *
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

#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/media.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/regmap.h>
#include <linux/version.h>

#include "ds90ub954.h"

#define ENABLE_SYSFS_TP /* /sys/bus/i2c/devices/0-0018 */

static const struct of_device_id ds90ub954_of_match[] = {
	{
		.compatible = "ti,ds90ub954",
	},
	{/* sentinel */},
};

const struct regmap_config ds90ub954_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
};

const struct regmap_config ds90ub953_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
};

/* 4096x2160 */
static const u8 ds90ub95x_tp_reg_val[] = {
	/* Indirect Pattern Gen Registers */
	0xB0, 0x00,
	0xB1, TI954_REG_IA_PGEN_CTL,
	0xB2, (1<<TI954_PGEB_ENABLE),
	0xB1, TI954_REG_IA_PGEB_CFG,
	0xB2, 0x35,
	0xB1, TI954_REG_IA_PGEN_CSI_DI,
	0xB2, 0x2B,
	0xB1, TI954_REG_IA_PGEN_LINE_SIZE1,
	0xB2, 0x14,
	0xB1, TI954_REG_IA_PGEN_LINE_SIZE0,
	0xB2, 0x00,
	0xB1, TI954_REG_IA_PGEN_BAR_SIZE1,
	0xB2, 0x02,
	0xB1, TI954_REG_IA_PGEN_BAR_SIZE0,
	0xB2, 0x80,
	0xB1, TI954_REG_IA_PGEN_ACT_LPF1,
	0xB2, 0x08,
	0xB1, TI954_REG_IA_PGEN_ACT_LPF0,
	0xB2, 0x70,
	0xB1, TI954_REG_IA_PGEN_TOT_LPF1,
	0xB2, 0x08,
	0xB1, TI954_REG_IA_PGEN_TOT_LPF0,
	0xB2, 0x70,
	0xB1, TI954_REG_IA_PGEN_LINE_PD1,
	0xB2, 0x0B,
	0xB1, TI954_REG_IA_PGEN_LINE_PD0,
	0xB2, 0x93,
	0xB1, TI954_REG_IA_PGEN_VBP,
	0xB2, 0x21,
	0xB1, TI954_REG_IA_PGEN_VFP,
	0xB2, 0x0A,
};

/*------------------------------------------------------------------------------
 * DS90UB954 FUNCTIONS
 *----------------------------------------------------------------------------*/

static int ds90ub954_read(struct ds90ub954_priv *priv, unsigned int reg,
			  unsigned int *val)
{
	int err;
	err = regmap_read(priv->regmap, reg, val);
	if(err) {
		dev_err(&priv->client->dev,
			"Cannot read register 0x%02x (%d)!\n", reg, err);
	}
	return err;
}

static int ds90ub954_write(const struct ds90ub954_priv *priv, unsigned int reg,
			   unsigned int val)
{
	int err;

	err = regmap_write(priv->regmap, reg, val);
	if(err) {
		dev_err(&priv->client->dev,
			"Cannot write register 0x%02x (%d)!\n", reg, err);
	}
	return err;
}

static int ds90ub954_write_rx_port(struct ds90ub954_priv *priv, int rx_port,
				   int addr, int val)
{
	struct device *dev = &priv->client->dev;
	int err = 0;
	int port_reg = 0;

	/* rx_port = 0 -> choose rx_port 0
	 * rx_port = 1 -> choose rx_port 1
	 * rx_port = 2 -> choose rx_port 0 and 1 */
	if(rx_port > 2 || rx_port < 0) {
		dev_err(dev, "invalid port number %d. Cannot be selected\n",
			rx_port);
		err = -EINVAL;
		goto write_rx_port_err;
	}

	/* Check if port is selected, select port if needed */
	if(priv->sel_rx_port != rx_port) {
		/* Set RX_WRITE_PORT_1 if rx_port is 1,
		 * set RX_WRITE_PORT_0 if rx_port is 0,
		 * set RX_WRITE_PORT_0 & _1 if rx_port is 2 */
		if(rx_port == 2) {
			port_reg |= (0b11);
		} else {
			/* Setting RX_WRITE_PORT_1 or _0 */
			port_reg |= (1<<rx_port);
			/* Setting RX_READ_PORT to rx_port */
			port_reg |= (rx_port<<TI954_RX_READ_PORT);
		}

		err = ds90ub954_write(priv, TI954_REG_FPD3_PORT_SEL, port_reg);
		if(unlikely(err)) {
			dev_err(&priv->client->dev,
				"error writing register TI954_REG_FPD3_PORT_SEL (0x%02x)\n",
				TI954_REG_FPD3_PORT_SEL);
			goto write_rx_port_err;
		}
		priv->sel_rx_port = rx_port;
	}
	err = ds90ub954_write(priv, addr, val);
	if(unlikely(err)) {
		dev_err(&priv->client->dev, "error writing register (0x%02x)\n",
			addr);
		goto write_rx_port_err;
	}

write_rx_port_err:
	return err;
}

#ifdef DEBUG
static int ds90ub954_read_rx_port(struct ds90ub954_priv *priv, int rx_port,
				  int addr, int *val)
{
	struct device *dev = &priv->client->dev;
	int err = 0;
	int port_reg = 0;

	/* rx_port = 0 -> choose rx_port 0
	 * rx_port = 1 -> choose rx_port 1 */
	if(rx_port > 2 || rx_port < 0) {
		dev_err(dev, "invalid port number %d. Cannot be selected\n",
			rx_port);
		err = -EINVAL;
		goto read_rx_port_err;
	}

	/* Check if port is selected, select port if needed */
	if(priv->sel_rx_port != rx_port) {
		/* Setting RX_WRITE_PORT_1 or _0 */
		port_reg |= (1<<rx_port);
		/* Setting RX_READ_PORT to rx_port */
		port_reg |= (rx_port<<TI954_RX_READ_PORT);

		err = ds90ub954_write(priv, TI954_REG_FPD3_PORT_SEL, port_reg);
		if(unlikely(err)) {
			dev_err(&priv->client->dev,
				"error writing register TI954_REG_FPD3_PORT_SEL (0x%02x)\n",
				TI954_REG_FPD3_PORT_SEL);
			goto read_rx_port_err;
		}
		priv->sel_rx_port = rx_port;
	}
	err = ds90ub954_read(priv, addr, val);
	if(unlikely(err)) {
		dev_err(&priv->client->dev, "error read register (0x%02x)\n",
			addr);
		goto read_rx_port_err;
	}

read_rx_port_err:
	return err;
}

static int ds90ub954_read_ia_reg(struct ds90ub954_priv *priv, int reg, int *val,
				 int ia_config)
{
	int err = 0;

	/* ia_configs:
	 *	0000: CSI-2 Pattern Generator & Timing Registers
	 *	0001: FPD-Link III RX Port 0 Reserved Registers
	 *	0010: FPD-Link III RX Port 1 Reserved Registers
	 *	0101: FPD-Link III RX Shared Reserved Registers
	 *	0110: Simultaneous write to FPD-Link III RX Reserved */

	/* Check if bank is selected, select bank if needed */
	if(priv->sel_ia_config != ia_config) {
		err = ds90ub954_write(priv, TI954_REG_IND_ACC_CTL, ia_config);
		if(unlikely(err)) {
			dev_err(&priv->client->dev,
				"error writing register TI954_REG_IND_ACC_CTL (0x%02x)\n",
				TI954_REG_IND_ACC_CTL);
			goto read_ia_reg_err;
		}
		priv->sel_ia_config = ia_config;
	}
	err = ds90ub954_write(priv, TI954_REG_IND_ACC_ADDR, reg);
	if(unlikely(err)) {
		dev_err(&priv->client->dev,
			"error writing register TI954_REG_IND_ACC_ADDR (0x%02x)\n",
			TI954_REG_IND_ACC_ADDR);
		goto read_ia_reg_err;
	}

	err = ds90ub954_read(priv, TI954_REG_IND_ACC_DATA, val);
	if(unlikely(err)) {
		dev_err(&priv->client->dev,
			"error reading register TI954_REG_IND_ACC_DATA (0x%02x)\n",
			TI954_REG_IND_ACC_DATA);
		goto read_ia_reg_err;
	}

read_ia_reg_err:
	return err;
}
#endif

static int ds90ub954_disable_testpattern(struct ds90ub954_priv *priv)
{
	struct device *dev = &priv->client->dev;
	int err = 0;
	/* Indirect Pattern Gen Registers */
	err = ds90ub954_write(priv, 0xB0, 0x00);
	if(err)
		goto init_err;
	err = ds90ub954_write(priv, 0xB1, TI954_REG_IA_PGEN_CTL);
	if(err)
		goto init_err;
	err = ds90ub954_write(priv, 0xB2, (0<<TI954_PGEB_ENABLE));
	if(err)
		goto init_err;
init_err:
	dev_info(dev, "%s: disable test pattern failed\n", __func__);
	return err;
}

static int ds90ub954_init_testpattern(struct ds90ub954_priv *priv)
{
	struct device *dev = &priv->client->dev;
	int i, err = 0;
	for(i = 0; i < ARRAY_SIZE(ds90ub95x_tp_reg_val); i += 2) {

		err = ds90ub954_write(priv, ds90ub95x_tp_reg_val[i],
					    ds90ub95x_tp_reg_val[i+1]);
		if(unlikely(err)) {
			dev_info(dev, "%s: enable test pattern failed\n", __func__);
			return err;
		}
	}
	dev_info(dev, "%s: enable test pattern successful\n", __func__);
	return err;
}

#ifdef DEBUG
static int ds90ub954_debug_prints(struct ds90ub954_priv *priv)
{
	int i, val, ia_config = 0, err = 0;
	/* print CSI timing of tx port 0 */
	dev_info(dev, "%s: CSI timing\n", __func__);
	err = ds90ub954_read_ia_reg(priv, TI954_REG_IA_CSI0_TCK_PREP, &val,
					ia_config);
	if(unlikely(err))
		return err;
	dev_info(dev, "%s: TCK_PREP: 0x%02x\n", __func__, val);

	err = ds90ub954_read_ia_reg(priv, TI954_REG_IA_CSI0_TCK_ZERO, &val,
					ia_config);
	if(unlikely(err))
		return err;
	dev_info(dev, "%s: TCK_ZERO: 0x%02x\n", __func__, val);

	err = ds90ub954_read_ia_reg(priv, TI954_REG_IA_CSI0_TCK_TRAIL, &val,
					ia_config);
	if(unlikely(err))
		return err;
	dev_info(dev, "%s: TCK_TRAIL: 0x%02x\n", __func__, val);

	err = ds90ub954_read_ia_reg(priv, TI954_REG_IA_CSI0_TCK_POST, &val,
					ia_config);
	if(unlikely(err))
		return err;
	dev_info(dev, "%s: TCK_POST: 0x%02x\n", __func__, val);

	err = ds90ub954_read_ia_reg(priv, TI954_REG_IA_CSI0_THS_PREP, &val,
					ia_config);
	if(unlikely(err))
		return err;
	dev_info(dev, "%s: THS_PREP: 0x%02x\n", __func__, val);

	err = ds90ub954_read_ia_reg(priv, TI954_REG_IA_CSI0_THS_ZERO, &val,
					ia_config);
	if(unlikely(err))
		return err;
	dev_info(dev, "%s: THS_ZERO: 0x%02x\n", __func__, val);

	err = ds90ub954_read_ia_reg(priv, TI954_REG_IA_CSI0_THS_TRAIL, &val,
					ia_config);
	if(unlikely(err))
		return err;
	dev_info(dev, "%s: THS_TRAIL: 0x%02x\n", __func__, val);

	err = ds90ub954_read_ia_reg(priv, TI954_REG_IA_CSI0_THS_EXIT, &val,
					ia_config);
	if(unlikely(err))
		return err;
	dev_info(dev, "%s: THS_EXIT: 0x%02x\n", __func__, val);

	err = ds90ub954_read_ia_reg(priv, TI954_REG_IA_CSI0_TPLX, &val,
					ia_config);
	if(unlikely(err))
		return err;
	dev_info(dev, "%s: CSI0_TPLX: 0x%02x\n", __func__, val);

	/* measure refclk */
	for(i = 0; i < 5; i++) {
		err = ds90ub954_read(priv, TI954_REG_REFCLK_FREQ, &val);
		if(err)
			return err;
		dev_info(dev, "%s: REFCLK_FREQ measurement %d, %d\n", __func__,
			 i, val);
	}
	return err;
};
#endif

static int ds90ub954_init(struct ds90ub954_priv *priv, int rx_port)
{
	struct device *dev = &priv->client->dev;
	unsigned char dev_id, rev;
	int i, val;
	int err = 0;
	char id_code[TI954_RX_ID_LENGTH + 1];
	int ser_nr = 0;
	struct ds90ub953_priv *ds90ub953;

	/*----------------------------------------------------------------------
	 *  init deserializer
	 *--------------------------------------------------------------------*/
	dev_info(dev, "%s starting\n", __func__);

	/* Read device id of deserializer */
	err = ds90ub954_read(priv, TI954_REG_I2C_DEV_ID, &val);
	if(unlikely(err))
		goto init_err;

	dev_id = (unsigned char)val;

	/* Read revision ID of deserializer */
	err = ds90ub954_read(priv, TI954_REG_REVISION, &val);
	if(unlikely(err))
		goto init_err;

	rev = (unsigned char)val;

	/* Read FPD3_RX_ID3 registers */
	memset(id_code, 0, sizeof(id_code));
	for(i = 0; i < TI954_RX_ID_LENGTH; i++) {
		err = ds90ub954_read(priv, TI954_REG_FPD3_RX_ID0 + i, &val);
		if(err) {
			goto init_err;
		}
		id_code[i] = (char)val;
	}
	dev_info(dev, "%s: device ID: 0x%x, code:%s, revision: 0x%x\n",
		 __func__, dev_id, id_code, rev);

	/* disable BuiltIn Self Test */
	err = ds90ub954_write(priv, TI954_REG_BIST_CONTROL, 0);
	if(unlikely(err))
		goto init_err;

	/* set CSI speed (REFCLK 25 MHz)
	*  00 : 1.6 Gbps serial rate
	*  01 : Reserved
	*  10 : 800 Mbps serial rate
	*  11 : 400 Mbps serial rate */
	switch(priv->csi_lane_speed) {
	case 400:
		val=0x3;
		break;
	case 800:
		val=0x2;
		break;
	default:
		val=0x0;
		break;
	}

	err = ds90ub954_write(priv, TI954_REG_CSI_PLL_CTL,
				(val<<TI954_CSI_TX_SPEED));
	if(unlikely(err))
		goto init_err;
#ifdef DEBUG
	err = ds90ub954_debug_prints(priv);
	if(unlikely(err))
		goto init_err;
#endif
	/* set number of csi lanes */
	switch(priv->csi_lane_count) {
	case 1:
		val = TI954_CSI_1_LANE;
		break;
	case 2:
		val = TI954_CSI_2_LANE;
		break;
	case 3:
		val = TI954_CSI_3_LANE;
		break;
	default:
		val = TI954_CSI_4_LANE;
		break;
	}

	err = ds90ub954_write(priv, TI954_REG_CSI_CTL,
				(1<<TI954_CSI_ENABLE)|
				(priv->conts_clk<<TI954_CSI_CONTS_CLOCK)|
				(val<<TI954_CSI_LANE_COUNT)|
				(1<<TI954_CSI_CAL_EN));
	if(unlikely(err))
		goto init_err;

	msleep(500);

	/* check if test pattern should be turned on */
	if(priv->test_pattern == 1) {
		dev_info(dev, "%s: deserializer init testpattern\n", __func__);
		err = ds90ub954_init_testpattern(priv);
		if(unlikely(err)) {
			dev_info(dev,
				"%s: deserializer init testpattern failed\n",
				__func__);
		}
	}

	/* Setting PASS and LOCK to "all enabled receiver ports */
	val = 0b00111100;
	err = ds90ub954_write(priv, TI954_REG_RX_PORT_CTL, val);
	if(unlikely(err))
		goto init_err;

	/* for loop goes through each serializer */
	for( ; ser_nr < priv->num_ser; ser_nr++) {
		ds90ub953 = priv->ser[ser_nr];
		if(ds90ub953->initialized == 0) {
			continue;
		}
		rx_port = ds90ub953->rx_channel;

		dev_info(dev, "%s: start init of serializer rx_port %i\n",
			 __func__, rx_port);

		/* Get TI954_REG_RX_PORT_CTL and enable receiver rx_port */
		err = ds90ub954_read(priv, TI954_REG_RX_PORT_CTL, &val);
		if(unlikely(err))
			goto ser_init_failed;

		val |= (1<<(TI954_PORT0_EN+rx_port));
		err = ds90ub954_write(priv, TI954_REG_RX_PORT_CTL, val);
		if(unlikely(err))
			goto ser_init_failed;

		/* wait for receiver to calibrate link */
		msleep(400);

		/* enable csi forwarding */
		err = ds90ub954_read(priv, TI954_REG_FWD_CTL1, &val);
		if(unlikely(err))
			goto ser_init_failed;

		val &= (0xEF<<rx_port);
		err = ds90ub954_write(priv, TI954_REG_FWD_CTL1, val);
		if(unlikely(err))
			goto ser_init_failed;

		msleep(500);

		/* config back channel RX port [specific register] */
		err = ds90ub954_write_rx_port(priv, rx_port,
				TI954_REG_BCC_CONFIG,
				(TI954_BC_FREQ_50M<<TI954_BC_FREQ_SELECT)|
				(1<<TI954_BC_CRC_GENERAOTR_ENABLE)|
				(1<<TI954_BC_ALWAYS_ON)|
				(ds90ub953->i2c_pt<<TI954_I2C_PASS_THROUGH_ALL)|
				(1<<TI954_I2C_PASS_THROUGH));
		if(unlikely(err))
			goto ser_init_failed;

		 /* wait for back channel */
		for(i = 0; i < 50; i++) {
			msleep(10);
			err = ds90ub954_read(priv, TI954_REG_DEVICE_STS, &val);
			if(unlikely(err))
				goto ser_init_failed;
			dev_info(dev, "%s: DEVICE STS: 0x%02x, id=%d x 10ms\n",
				 __func__, val, i);
			if((val & 0xff) == 0xdf) {
				i = 0;
				dev_info(dev, "%s: backchannel is ready\n",
					 __func__);
				break;
			}
		}
		if(i) {
			dev_err(dev, "%s: Backchannel setup failed!\n", __func__);
			err = -EIO;
			goto ser_init_failed;
		}
#ifdef DEBUG
		/* check PORT_STS1 */
		for(i = 0; i < 2; i++) {
			err = ds90ub954_read_rx_port(priv, rx_port,
							TI954_REG_RX_PORT_STS1,
							&val);
			if(unlikely(err))
				goto ser_init_failed;
			dev_info(dev, "%s: RX_PORT_STS1 read %d, 0x%02x\n",
				 __func__, i, val);
		}

		/* check PORT_STS2 */
		for(i = 0; i < 2; i++) {
			err = ds90ub954_read_rx_port(priv, rx_port,
							TI954_REG_RX_PORT_STS2,
							&val);
			if(unlikely(err))
				goto ser_init_failed;
			dev_info(dev, "%s: RX_PORT_STS2 read %d, 0x%02x\n",
				 __func__, i, val);
		}
#endif
		/* setup i2c forwarding */
		err = ds90ub954_write_rx_port(priv, rx_port, TI954_REG_SER_ALIAS_ID,
				(ds90ub953->i2c_address<<TI954_SER_ALIAS_ID));
		if(unlikely(err))
			goto ser_init_failed;

		/* Serializer GPIO control */
		err = ds90ub954_write_rx_port(priv, rx_port, TI954_REG_BC_GPIO_CTL0,
				(ds90ub953->gpio0_oc<<TI954_BC_GPIO0_SEL) |
				(ds90ub953->gpio1_oc<<TI954_BC_GPIO1_SEL));
		if(err)
			dev_info(dev, "%s: could not set TI954_REG_BC_GPIO_CTL0\n",
				 __func__);
		else
			dev_info(dev, "%s: Successfully set TI954_REG_BC_GPIO_CTL0\n",
				 __func__);

		err = ds90ub954_write_rx_port(priv, rx_port, TI954_REG_BC_GPIO_CTL1,
				(ds90ub953->gpio2_oc<<TI954_BC_GPIO2_SEL) |
				(ds90ub953->gpio3_oc<<TI954_BC_GPIO3_SEL));
		if(err)
			dev_info(dev, "%s: could not set TI954_REG_BC_GPIO_CTL1\n",
				 __func__);
		else
			dev_info(dev, "%s: Successfully set TI954_REG_BC_GPIO_CTL1\n",
				 __func__);

		/* set i2c slave ids and aliases */
		for(i=0; (i < ds90ub953->i2c_alias_num) && (i < NUM_ALIAS); i++) {
			val = ds90ub953->i2c_slave[i];
			if(val == 0) {
				continue;
			}
			err = ds90ub954_write_rx_port(priv, rx_port,
						      TI954_REG_SLAVE_ID0+i,
						      (val<<TI954_ALIAS_ID0));
			if(unlikely(err))
				goto ser_init_failed;
			dev_info(dev, "%s: slave id %i: 0x%X\n", __func__, i, val);

			val = ds90ub953->i2c_alias[i];
			if(val == 0) {
				continue;
			}
			err = ds90ub954_write_rx_port(priv, rx_port,
						      TI954_REG_ALIAS_ID0+i,
						      (val<<TI954_ALIAS_ID0));
			if(unlikely(err))
				goto ser_init_failed;
			dev_info(dev, "%s: alias id %i: 0x%X\n", __func__, i, val);
		}

		/* set virtual channel id mapping */
		err = ds90ub954_write_rx_port(priv, rx_port,
					      TI954_REG_CSI_VC_MAP,
					      ds90ub953->vc_map);
		if(unlikely(err))
			goto ser_init_failed;
		else {
			val = ds90ub953->vc_map & 0b11;
			dev_info(dev, "%s: VC-ID 0 mapped to %i\n", __func__, val);
			val = ((ds90ub953->vc_map & 0b1100)>>2);
			dev_info(dev, "%s: VC-ID 1 mapped to %i\n", __func__, val);
			val = ((ds90ub953->vc_map & 0b110000)>>4);
			dev_info(dev, "%s: VC-ID 2 mapped to %i\n", __func__, val);
			val = ((ds90ub953->vc_map & 0b11000000)>>6);
			dev_info(dev, "%s: VC-ID 3 mapped to %i\n", __func__, val);
		}

		/* all rx_port specific registers set for rx_port X */
		dev_info(dev, "%s: init of deserializer rx_port %i successful\n",
			 __func__, rx_port);
		continue;
ser_init_failed:
		dev_err(dev, "%s: init deserializer rx_port %i failed\n",
			__func__, rx_port);
		dev_err(dev, "%s: deserializer rx_port %i is deactivated\n",
			__func__, rx_port);

		ds90ub953->initialized = 0;

		/* DISABLE RX PORT */
		err = ds90ub954_read(priv, TI954_REG_RX_PORT_CTL, &val);
		if(err)
			continue;
		val &= (0xFF^(1<<(TI954_PORT0_EN+rx_port)));
		err = ds90ub954_write(priv, TI954_REG_RX_PORT_CTL, val);
		if(err)
			continue;
		/* DISABLE CSI FORWARDING */
		err = ds90ub954_read(priv, TI954_REG_FWD_CTL1, &val);
		if(err)
			continue;
		val |= (1<<(TI954_FWD_PORT0_DIS+rx_port));
		err = ds90ub954_write(priv, TI954_REG_FWD_CTL1, val);
		if(err)
			continue;
		continue;
	}

	/* setup gpio forwarding, default all input */
	err = ds90ub954_write(priv, TI954_REG_GPIO_INPUT_CTL,
			      (1<<TI954_GPIO6_INPUT_EN)|
			      (1<<TI954_GPIO5_INPUT_EN)|
			      (1<<TI954_GPIO4_INPUT_EN)|
			      (1<<TI954_GPIO3_INPUT_EN)|
			      (1<<TI954_GPIO2_INPUT_EN)|
			      (1<<TI954_GPIO1_INPUT_EN)|
			      (1<<TI954_GPIO0_INPUT_EN));
	if(err)
		goto init_err;

	err = ds90ub954_write(priv, TI954_REG_GPIO0_PIN_CTL, 0);
	if(err)
		goto init_err;

	err = ds90ub954_write(priv, TI954_REG_GPIO1_PIN_CTL, 0);
	if(err)
		goto init_err;

	err = ds90ub954_write(priv, TI954_REG_GPIO2_PIN_CTL, 0);
	if(err)
		goto init_err;

	err = ds90ub954_write(priv, TI954_REG_GPIO3_PIN_CTL, 0);
	if(err)
		goto init_err;

	err = ds90ub954_write(priv, TI954_REG_GPIO4_PIN_CTL, 0);
	if(err)
		goto init_err;

	err = ds90ub954_write(priv, TI954_REG_GPIO5_PIN_CTL, 0);
	if(err)
		goto init_err;

	err = ds90ub954_write(priv, TI954_REG_GPIO6_PIN_CTL, 0);
	if(err)
		goto init_err;

init_err:
	return err;
}

static int ds90ub954_init_gpio(const struct ds90ub954_priv *priv)
{
	struct device *dev = &priv->client->dev;
	int err = 0;

	if(gpio_is_valid(priv->pass_gpio)) {
		err = gpio_request(priv->pass_gpio, "ds90ub954_pass_gpio");
		if(unlikely(err < 0)) {
			dev_err(dev, "unable to request pass_gpio (%d)\n", err);
			goto done;
		}
		err = gpio_direction_input(priv->pass_gpio);
		if(unlikely(err < 0)) {
			dev_err(dev, "unable to configure pass_gpio as input (%d)\n", err);
			goto done;
		}
	}

	if(gpio_is_valid(priv->lock_gpio)) {
		err = gpio_request(priv->lock_gpio, "ds90ub954_lock_gpio");
		if(unlikely(err < 0)) {
			dev_err(dev, "unable to request lock_gpio (%d)\n", err);
			goto done;
		}
		err = gpio_direction_input(priv->lock_gpio);
		if(unlikely(err < 0)) {
			dev_err(dev, "unable to configure lock_gpio as input (%d)\n", err);
			goto done;
		}
	}

	if(gpio_is_valid(priv->pdb_gpio)) {
		err = gpio_request(priv->pdb_gpio, "ds90ub954_pdb_gpio");
		if(unlikely(err < 0)) {
			dev_err(dev, "unable to request pdb_gpio (%d)\n", err);
			goto done;
		}
		err = gpio_direction_output(priv->pdb_gpio, 0);
		if(unlikely(err < 0)) {
			dev_err(dev, "unable to configure pdb_gpio as output (%d)\n", err);
			goto done;
		}
	}

done:
	return err;
}

static void ds90ub954_free_gpio(const struct ds90ub954_priv *priv)
{

	if(priv->pass_gpio >= 0) {
		gpio_free(priv->pass_gpio);
	}
	if(priv->lock_gpio >= 0) {
		gpio_free(priv->lock_gpio);
	}
	if(priv->pdb_gpio >= 0) {
		gpio_free(priv->pdb_gpio);
	}
}

static void ds90ub954_pwr_enable(const struct ds90ub954_priv *priv)
{
	if(priv->pdb_gpio >= 0) {
		gpio_set_value_cansleep(priv->pdb_gpio, 1);
	}
}

static void ds90ub954_pwr_disable(const struct ds90ub954_priv *priv)
{
	if(priv->pdb_gpio >= 0) {
		gpio_set_value_cansleep(priv->pdb_gpio, 0);
	}
}

static int ds90ub954_parse_dt(struct ds90ub954_priv *priv)
{
	struct device *dev = &priv->client->dev;
	struct device_node *np = dev->of_node;
	const struct of_device_id *match;
	int gpio;
	int err = 0;
	int val = 0;

	if(!np)
		return -ENODEV;

	dev_info(dev, "%s: deserializer:\n", __func__);
	match = of_match_device(ds90ub954_of_match, dev);
	if(!match) {
		dev_err(dev, "Failed to find matching dt id\n");
		return -ENODEV;
	}

	gpio = of_get_named_gpio(np, "pass-gpio", 0);
	if(gpio < 0) {
		if(gpio == -EPROBE_DEFER) {
			err = gpio;
			dev_err(dev, "pass-gpio read failed: (%d)\n", err);
			return err;
		}
		dev_info(dev, "pass-gpio not found, ignoring\n");
	}
	priv->pass_gpio = gpio;

	gpio = of_get_named_gpio(np, "lock-gpio", 0);
	if(gpio < 0) {
		if(gpio == -EPROBE_DEFER) {
			err = gpio;
			dev_err(dev, "lock-gpio read failed: (%d)\n", err);
			return err;
		}
		dev_info(dev, "lock-gpio not found, ignoring\n");
	}
	priv->lock_gpio = gpio;

	gpio = of_get_named_gpio(np, "pdb-gpio", 0);
	if(gpio < 0) {
		if(gpio == -EPROBE_DEFER) {
			err = gpio;
			dev_err(dev, "pdb-gpio read failed: (%d)\n", err);
			return err;
		}
		dev_info(dev, "pdb-gpio not found, ignoring\n");
	}
	priv->pdb_gpio = gpio;

	err = of_property_read_u32(np, "csi-lane-count", &val);
	if(err) {
		dev_info(dev, "%s: - csi-lane-count property not found\n", __func__);

		/* default value: 4 */
		priv->csi_lane_count = 4;
		dev_info(dev, "%s: - csi-lane-count set to default val: 4\n", __func__);
	} else {
		/* set csi-lane-count*/
		priv->csi_lane_count = val;
		dev_info(dev, "%s: - csi-lane-count %i\n", __func__, val);
	}

	err = of_property_read_u32(np, "csi-lane-speed", &val);
	if(err) {
		dev_info(dev, "%s: - csi-lane-speed property not found\n", __func__);

		/* default value: 4 */
		priv->csi_lane_speed = 1600;
		dev_info(dev, "%s: - csi-lane-speed set to default val: 4\n", __func__);
	} else {
		/* set csi-lane-speed*/
		priv->csi_lane_speed = val;
		dev_info(dev, "%s: - csi-lane-speed %i\n", __func__, val);
	}

	if(of_property_read_bool(np, "test-pattern")) {
		dev_info(dev, "%s: - test-pattern enabled\n", __func__);
		priv->test_pattern = 1;
	} else {
		/* default value: 0 */
		priv->test_pattern = 0;
		dev_info(dev, "%s: - test-pattern disabled\n", __func__);
	}

	if(of_property_read_bool(np, "continuous-clock")) {
		dev_info(dev, "%s: - continuous clock enabled\n", __func__);
		priv->conts_clk = 1;
	} else {
		/* default value: 0 */
		priv->conts_clk = 0;
		dev_info(dev, "%s: - discontinuous clock used\n", __func__);
	}

	return 0;

}

#ifdef ENABLE_SYSFS_TP
static ssize_t test_pattern_show_des(struct device *dev,
				     struct device_attribute *attr,
				     char *buf)
{
	struct ds90ub954_priv *priv;
	priv = dev_get_drvdata(dev);
	return snprintf(buf, PAGE_SIZE,
			"Test Pattern is set to: %i\n", priv->test_pattern);
}

static ssize_t test_pattern_set_des(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t count)
{
	struct ds90ub954_priv *priv;
	int testpat;

	priv = dev_get_drvdata(dev);
	sscanf(buf, "%d", &testpat);
	if(testpat > 1 || testpat < 0) {
		dev_info(dev,
			 "Invalid value: %i for test pattern (0/1)\n", testpat);
		return PAGE_SIZE;
	}
	if(testpat == 1) {
		dev_info(dev, "enabling testpattern for deserializer\n");
		priv->test_pattern = 1;
		ds90ub954_init_testpattern(priv);
	} else {
		dev_info(dev, "disabling testpattern for deserializer\n");
		ds90ub954_disable_testpattern(priv);
	}
	return PAGE_SIZE;
}
static DEVICE_ATTR(test_pattern_des, 0664,test_pattern_show_des,
		   test_pattern_set_des);
#endif

/*------------------------------------------------------------------------------
 * DS90UB953 FUNCTIONS
 *----------------------------------------------------------------------------*/

static int ds90ub953_read(struct ds90ub953_priv *priv, unsigned int reg,
			  unsigned int *val)
{
	int err;
	err = regmap_read(priv->regmap, reg, val);
	if(err) {
		dev_err(&priv->client->dev,
			"Cannot read subdev 0x%02x register 0x%02x (%d)!\n",
			priv->client->addr, reg, err);
	}
	return err;
}

static int ds90ub953_write(const struct ds90ub953_priv *priv, unsigned int reg,
			   unsigned int val)
{
	int err;
	err = regmap_write(priv->regmap, reg, val);
	if(err) {
		dev_err(&priv->client->dev,
			"Cannot write subdev 0x%02x register 0x%02x (%d)!\n",
			priv->client->addr, reg, err);
	}
	return err;
}

static int ds90ub953_disable_testpattern(struct ds90ub953_priv *priv)
{
	struct device *dev = &priv->client->dev;
	int err = 0;
	/* Indirect Pattern Gen Registers */
	err = ds90ub953_write(priv, 0xB0, 0x00);
	if(err)
		goto init_err;
	err = ds90ub953_write(priv, 0xB1, TI954_REG_IA_PGEN_CTL);
	if(err)
		goto init_err;
	err = ds90ub953_write(priv, 0xB2, (0<<TI954_PGEB_ENABLE));
	if(err)
		goto init_err;
init_err:
	dev_info(dev, "%s: disable test pattern failed\n", __func__);
	return err;
}

static int ds90ub953_init_testpattern(struct ds90ub953_priv *priv)
{
	struct device *dev = &priv->client->dev;
	int err = 0;
	int i;
	for(i = 0; i < ARRAY_SIZE(ds90ub95x_tp_reg_val); i += 2) {

		err = ds90ub953_write(priv, ds90ub95x_tp_reg_val[i],
					    ds90ub95x_tp_reg_val[i+1]);
		if(unlikely(err)) {
			dev_info(dev, "%s: enable test pattern failed\n", __func__);
			return err;
		}
	}
	dev_info(dev, "%s: enable test pattern successful\n", __func__);
	return err;
}

#ifdef ENABLE_SYSFS_TP
static ssize_t test_pattern_show_ser(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
	struct ds90ub953_priv *priv;
	priv = dev_get_drvdata(dev);
	return snprintf(buf, PAGE_SIZE,
			"Test Pattern is set to: %i\n", priv->test_pattern);
}

static ssize_t test_pattern_set_ser(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t count)
{
	struct ds90ub953_priv *priv;
	int testpat;

	priv = dev_get_drvdata(dev);

	sscanf(buf, "%d", &testpat);
	if(testpat > 1 || testpat < 0) {
		dev_info(dev,
			 "Invalid value: %i for test pattern (0/1)\n", testpat);
		return PAGE_SIZE;
	}
	if(testpat == 1) {
		dev_info(dev, "enabling testpattern for deserializer\n");
		priv->test_pattern = 1;
		ds90ub953_init_testpattern(priv);
	} else {
		dev_info(dev, "disabling testpattern for deserializer\n");
		ds90ub953_disable_testpattern(priv);
	}
	return PAGE_SIZE;
}
static DEVICE_ATTR(test_pattern_ser, 0664, test_pattern_show_ser,
		   test_pattern_set_ser);

#endif

static int ds90ub953_init(struct ds90ub953_priv *priv)
{
	struct device *dev = &priv->client->dev;
	int val, dev_id, i;
	int err = 0;
	char id_code[TI953_RX_ID_LENGTH + 1];

	dev_info(dev, "%s: start\n", __func__);

	err = ds90ub953_read(priv, TI953_REG_I2C_DEV_ID, &val);
	if(unlikely(err))
		goto init_err;

	dev_id = (unsigned char)val;

	memset(id_code, 0, sizeof(id_code));
	for(i = 0; i < TI953_RX_ID_LENGTH; i++) {
		err = ds90ub953_read(priv, TI953_REG_FPD3_RX_ID0 + i, &val);
		if(unlikely(err))
			goto init_err;
		id_code[i] = (char)val;
	}
	dev_info(dev, "%s: device ID: 0x%x, code:%s\n", __func__, dev_id, id_code);

	 /* set to csi lanes */
	switch(priv->csi_lane_count) {
	case 1:
		val = TI953_CSI_LANE_SEL1;
		break;
	case 2:
		val = TI953_CSI_LANE_SEL2;
		break;
	default:
		val = TI953_CSI_LANE_SEL4;
		break;
	}
	err = ds90ub953_write(priv, TI953_REG_GENERAL_CFG,
			      (1<<TI953_I2C_STRAP_MODE) |
			      (1<<TI953_CRC_TX_GEN_ENABLE) |
			      (val<<TI953_CSI_LANE_SEL) |
			      (priv->conts_clk<<TI953_CONTS_CLK));
	if(unlikely(err))
		goto init_err;

	//Set GPIO0 as output
	err = ds90ub953_write(priv, TI953_REG_GPIO_CTRL, 0x1E);

	/* set clock output frequency */
	err = ds90ub953_write(priv, TI953_REG_CLKOUT_CTRL0,
			      (priv->hs_clk_div<<TI953_HS_CLK_DIV) |
			      (priv->div_m_val<<TI953_DIV_M_VAL));
	if(unlikely(err))
		goto init_err;

	err = ds90ub953_write(priv, TI953_REG_CLKOUT_CTRL1,
			      priv->div_n_val<<TI953_DIV_N_VAL);
	if(unlikely(err))
		goto init_err;

	/* setup GPIOs to input/output */
	val = 0;
	if(priv->gpio0_oe)
		val |= 0b00010000;
	else
		val |= 0b00000001;

	if(priv->gpio1_oe)
		val |= 0b00100000;
	else
		val |= 0b00000010;

	if(priv->gpio2_oe)
		val |= 0b01000000;
	else
		val |= 0b00000100;

	if(priv->gpio3_oe)
		val |= 0b10000000;
	else
		val |= 0b00001000;

	err = ds90ub953_write(priv, TI953_REG_GPIO_CTRL, val);
	if(unlikely(err))
		goto init_err;

	err = ds90ub953_write(priv, TI953_REG_LOCAL_GPIO_DATA,
			      (0xf<<TI953_GPIO_RMTEN));
	if(unlikely(err))
		goto init_err;

	err = ds90ub953_write(priv, TI953_REG_BCC_CONFIG,
			      (0x1<<TI953_I2C_PASS_THROUGH_ALL) |
			      (0x1<<TI953_RX_PARITY_CHECKER_ENABLE));
	if(unlikely(err))
		goto init_err;

	/* check if test pattern should be turned on*/
	if(priv->test_pattern == 1) {
		dev_info(dev,"%s: serializer rx_port %i init testpattern\n",
			 __func__, priv->rx_channel);
		err = ds90ub953_init_testpattern(priv);
		if(unlikely(err))
			dev_info(dev,
				 "%s: serializer rx_port %i init testpattern failed\n",
				 __func__, priv->rx_channel);
	}

#ifdef ENABLE_SYSFS_TP
	/* device attribute on sysfs */
	dev_set_drvdata(dev, priv);
	err = device_create_file(dev, &dev_attr_test_pattern_ser);
	if(unlikely(err < 0))
		dev_err(dev, "serializer %i cant create device attribute %s\n",
			priv->rx_channel, dev_attr_test_pattern_ser.attr.name);
#endif
	dev_info(dev, "%s: successful\n", __func__);

init_err:
	return err;
}

static void ds90ub953_free(struct ds90ub954_priv *priv)
{
	int i;
	for(i = 0; i < priv->num_ser; i++) {
		if(priv->ser[i])
			i2c_unregister_device(priv->ser[i]->client);
	}
}

static int ds90ub953_regmap_init(struct ds90ub954_priv *priv, int ser_nr)
{
	struct regmap *new_regmap = NULL;
	struct device *dev = &priv->client->dev;
	int err = 0;

	/* setup now regmap */
	new_regmap = devm_regmap_init_i2c(priv->ser[ser_nr]->client,
					  &ds90ub953_regmap_config);
	if(IS_ERR_VALUE(priv->regmap)) {
		err = PTR_ERR(priv->regmap);
		dev_err(dev, "regmap init of subdevice failed (%d)\n", err);
		return err;
	}
	dev_info(dev, "%s init regmap done\n", __func__);

	priv->ser[ser_nr]->regmap = new_regmap;
	return err;
}

static int ds90ub953_alloc(struct ds90ub954_priv *priv, int ser_nr)
{
	struct ds90ub953_priv *priv_ser;
	struct device *dev = &priv->client->dev;

	priv_ser = devm_kzalloc(dev, sizeof(struct ds90ub953_priv), GFP_KERNEL);
	if(!priv)
		return -ENOMEM;

	priv->ser[ser_nr] = priv_ser;
	priv->ser[ser_nr]->initialized = 0;
	return 0;
}

static int ds90ub953_i2c_client(struct ds90ub954_priv *priv, int ser_nr,
				int addr)
{
	struct i2c_client *new_client = NULL;
	struct device *dev = &priv->client->dev;

	struct i2c_board_info *ser_board_info;
	ser_board_info = devm_kzalloc(dev, sizeof(struct i2c_board_info), GFP_KERNEL);
	ser_board_info->addr = addr;
//i2c_new_device ----> i2c_new_client_device 
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 4, 0)
	new_client = i2c_new_device(priv->client->adapter, ser_board_info);
#else
	new_client = i2c_new_client_device(priv->client->adapter, ser_board_info);
#endif
	
	if(!new_client) {
		dev_warn(dev, "failed to add i2c client\n");
		return -1;
	}

	priv->ser[ser_nr]->client = new_client;
	dev_info(dev, "%s init client done\n", __func__);
	return 0;
}

static int ds90ub953_parse_dt(struct i2c_client *client,
			      struct ds90ub954_priv *priv)
{
	struct device *dev = &client->dev;
	struct device_node *des = dev->of_node;
	struct device_node *ser;
	struct device_node *sers;
	struct of_phandle_args i2c_addresses;
	struct ds90ub953_priv *ds90ub953;
	int i = 0;

	u32 val = 0;
	int err = 0;
	int counter = 0;
	priv->num_ser = 0;

	/* get serializers device_node from dt */
	sers = of_get_child_by_name(des, "serializers");
	if(!sers) {
		dev_info(dev, "%s: no serializers found in device tree\n",
			 __func__);
		return 0;
	}

	dev_info(dev, "%s: parsing serializers device tree:\n", __func__);

	/* go through all serializers in list */
	for_each_child_of_node(sers, ser) {

		if(counter >= NUM_SERIALIZER) {
			dev_info(dev, "%s: too many serializers found in device tree\n",
				 __func__);
			break;
		}

		/* allocate memory for serializer */
		err = ds90ub953_alloc(priv, counter);
		if(err) {
			dev_info(dev, "%s: - allocating ds90ub953 failed\n",
				 __func__);
			goto next;
		}
		ds90ub953 = priv->ser[counter];

		/* get rx-channel */
		err = of_property_read_u32(ser, "rx-channel", &val);
		if(err) {
			dev_info(dev, "%s: - rx-channel property not found\n",
				 __func__);
			/* default value: 0 */
			ds90ub953->rx_channel = 0;
			dev_info(dev, "%s: rx-channel set to default val: 0\n",
				 __func__);
		} else {
			/* set rx-channel*/
			ds90ub953->rx_channel = val;
			dev_info(dev,"%s: - serializer rx-channel: %i\n",
				 __func__, val);
		}

		if(of_property_read_bool(ser, "test-pattern")) {
			dev_info(dev, "%s: - test-pattern enabled\n", __func__);
			ds90ub953->test_pattern = 1;
		} else {
			/* default value: 0 */
			ds90ub953->test_pattern = 0;
			dev_info(dev,"%s: -test-pattern disabled\n", __func__);
		}

		err = of_property_read_u32(ser, "csi-lane-count", &val);
		if(err) {
			dev_info(dev, "%s: - csi-lane-count property not found\n",
				 __func__);
			/* default value: 4 */
			ds90ub953->csi_lane_count = 4;
			dev_info(dev, "%s: csi-lane-count set to default val: 4\n",
				 __func__);
		} else {
			/* set csi-lane-count*/
			ds90ub953->csi_lane_count = val;
			dev_info(dev, "%s: - csi-lane-count %i\n", __func__, val);
		}

		/* GPIO output enable */
		err = of_property_read_u32(ser, "gpio0-output-enable", &val);
		if(err) {
			dev_info(dev, "%s: - gpio0-output-enable property not found\n",
				 __func__);
			/* default value: 0 */
			ds90ub953->gpio0_oe = 0;
			dev_info(dev, "%s: gpio0-output-enable to default val: 0\n",
				 __func__);
		} else {
			/* set gpio0-output-enable*/
			ds90ub953->gpio0_oe = val;
			dev_info(dev, "%s: - gpio0-output-enable %i\n",
				 __func__, val);
		}

		err = of_property_read_u32(ser, "gpio1-output-enable", &val);
		if(err) {
			dev_info(dev, "%s: - gpio1-output-enable property not found\n",
				 __func__);

			/* default value: 0 */
			ds90ub953->gpio1_oe = 0;
			dev_info(dev, "%s: gpio1-output-enable to default val: 0\n",
				 __func__);
		} else {
			/* set gpio1-output-enable*/
			ds90ub953->gpio1_oe = val;
			dev_info(dev, "%s: - gpio1-output-enable %i\n",
				 __func__, val);
		}

		err = of_property_read_u32(ser, "gpio2-output-enable", &val);
		if(err) {
			dev_info(dev, "%s: - gpio2-output-enable property not found\n",
				 __func__);
			/* default value: 0 */
			ds90ub953->gpio2_oe = 0;
			dev_info(dev, "%s: gpio2-output-enable to default val: 0\n",
				 __func__);
		} else {
			/* set gpio2-output-enable*/
			ds90ub953->gpio2_oe = val;
			dev_info(dev,"%s: - gpio2-output-enable %i\n", __func__,
				 val);
		}

		err = of_property_read_u32(ser, "gpio3-output-enable", &val);
		if(err) {
			dev_info(dev, "%s: - gpio3-output-enable property not found\n",
				 __func__);
			/* default value: 0 */
			ds90ub953->gpio3_oe = 0;
			dev_info(dev, "%s: gpio3-output-enable to default val: 0\n",
				 __func__);
		} else {
			/* set gpio3-output-enable*/
			ds90ub953->gpio3_oe = val;
			dev_info(dev, "%s: - gpio3-output-enable %i\n",
				 __func__, val);
		}

		/* GPIO output control */
		err = of_property_read_u32(ser, "gpio0-control", &val);
		if(err) {
			dev_info(dev, "%s: - gpio0-control property not found\n",
				 __func__);
			/* default value: 0b1000 */
			ds90ub953->gpio0_oc = 0b1000;
			dev_info(dev, "%s: gpio0-control to default val: 0b1000\n",
				 __func__);
		} else {
			/* set gpio0-control*/
			ds90ub953->gpio0_oc = val;
			dev_info(dev,"%s: - gpio0-control %i\n",
				 __func__, val);
		}

		err = of_property_read_u32(ser, "gpio1-control", &val);
		if(err) {
			dev_info(dev, "%s: - gpio1-control property not found\n",
				 __func__);

			/* default value: 0b1000 */
			ds90ub953->gpio1_oc = 0b1000;
			dev_info(dev, "%s: gpio1-control to default val: 0b1000\n",
				 __func__);
		} else {
			/* set gpio1-control*/
			ds90ub953->gpio1_oc = val;
			dev_info(dev, "%s: - gpio1-control %i\n",
				 __func__, val);
		}

		err = of_property_read_u32(ser, "gpio2-control", &val);
		if(err) {
			dev_info(dev, "%s: - gpio2-control property not found\n",
				 __func__);
			/* default value: 0b1000 */
			ds90ub953->gpio2_oc = 0b1000;
			dev_info(dev, "%s: gpio2-control to default val: 0b1000\n",
				 __func__);
		} else {
			/* set gpio2-control*/
			ds90ub953->gpio2_oc = val;
			dev_info(dev, "%s: - gpio2-control %i\n",
				 __func__, val);
		}

		err = of_property_read_u32(ser, "gpio3-control", &val);
		if(err) {
			dev_info(dev, "%s: - gpio3-control property not found\n",
				 __func__);
			/* default value: 0b1000 */
			ds90ub953->gpio3_oc = 0b1000;
			dev_info(dev, "%s: gpio3-control to default val: 0b1000\n",
				 __func__);
		} else {
			/* set gpio3-control*/
			ds90ub953->gpio3_oc = val;
			dev_info(dev, "%s: - gpio3-control %i\n",
				 __func__, val);
		}

		err = of_property_read_u32(ser, "hs-clk-div", &val);
		if(err) {
			dev_info(dev, "%s: - hs-clk-div property not found\n",
				 __func__);

			/* default value: 0x2 */
			ds90ub953->hs_clk_div = 0x2;
			dev_info(dev, "%s: - hs-clk-div set to default val: 0x2 (div by 4)\n",
				 __func__);
		} else {
			switch(val) {
			case 1:
				ds90ub953->hs_clk_div = 0b000;
				break;
			case 2:
				ds90ub953->hs_clk_div = 0b001;
				break;
			case 4:
				ds90ub953->hs_clk_div = 0b010;
				break;
			case 8:
				ds90ub953->hs_clk_div = 0b011;
				break;
			case 16:
				ds90ub953->hs_clk_div = 0b100;
				break;
			default:
				ds90ub953->hs_clk_div = 0b010;
				dev_info(dev, "%s: - %i no valid value for hs-clk-div\n",
					 __func__, val);
				break;
			}
			dev_info(dev,"%s: - hs-clk-div set to val: %i (div by %i)\n",
				 __func__, ds90ub953->hs_clk_div, val);
		}

		err = of_property_read_u32(ser, "div-m-val", &val);
		if(err) {
			dev_info(dev, "%s: - div-m-val property not found\n",
				 __func__);
			/* default value: 1 */
			ds90ub953->div_m_val = 1;
			dev_info(dev, "%s: - div-m-val set to default val: 1\n",
				 __func__);
		} else {
			/* set div-m-val*/
			ds90ub953->div_m_val = val;
			dev_info(dev, "%s: - div-m-val %i\n", __func__, val);
		}

		err = of_property_read_u32(ser, "div-n-val", &val);
		if(err) {
			dev_info(dev, "%s: - div-n-val property not found\n",
				 __func__);
			/* default value: 0x28 */
			ds90ub953->div_n_val = 0x28;
			dev_info(dev, "%s: - div-n-val set to default val: 0x28\n",
				 __func__);
		} else {
			/* set div-n-val*/
			ds90ub953->div_n_val = val;
			dev_info(dev, "%s: - div-n-val %i\n", __func__, val);
		}

		/* get i2c address */
		err = of_property_read_u32(ser, "i2c-address", &val);
		if(err) {
			dev_info(dev, "%s: - i2c-address not found\n", __func__);
			ds90ub953->i2c_address = 0x18;
			dev_info(dev, "%s: - i2c-address set to default val: 0x18\n",
				 __func__);
		} else {
			dev_info(dev, "%s: - i2c-address: 0x%X \n", __func__, val);
			ds90ub953->i2c_address=val;
		}

		err = ds90ub953_i2c_client(priv, counter, val);
		if(err) {
			dev_info(dev, "%s: - ds90ub953_i2c_client failed\n",
				 __func__);
			goto next;
		}

		err = ds90ub953_regmap_init(priv, counter);
		if(err) {
			dev_info(dev, "%s: - ds90ub953_regmap_init failed\n",
				 __func__);
			goto next;
		}

		/* get i2c-slave addresses*/
		err = of_parse_phandle_with_args(ser, "i2c-slave", "list-cells",
						 0, &i2c_addresses);
		if(err) {
			dev_info(dev, "%s: - reading i2c-slave addresses failed\n",
				 __func__);
			ds90ub953->i2c_alias_num = 0;
		} else {
			ds90ub953->i2c_alias_num = i2c_addresses.args_count;
			/* writting i2c slave addresses into array*/
			for(i = 0; (i < i2c_addresses.args_count) &&
							(i<NUM_ALIAS) ; i++) {
				ds90ub953->i2c_slave[i] = i2c_addresses.args[i];
			}
		}

		/* get slave-aliases */
		err = of_parse_phandle_with_args(ser, "slave-alias",
						 "list-cells", 0, &i2c_addresses);
		if(err) {
			dev_info(dev, "%s: - reading i2c slave-alias addresses failed\n",
				 __func__);
			ds90ub953->i2c_alias_num = 0;
		} else {
			dev_info(dev, "%s: - num of slave alias pairs: %i\n",
				 __func__, i2c_addresses.args_count);
			/* writting i2c alias addresses into array*/
			for(i=0; (i<i2c_addresses.args_count) && (i<NUM_ALIAS);
			    i++) {
				ds90ub953->i2c_alias[i] = i2c_addresses.args[i];
				dev_info(dev, "%s: - slave addr: 0x%X, alias addr: 0x%X\n",
					 __func__, ds90ub953->i2c_slave[i],
					 ds90ub953->i2c_alias[i]);
			}
		}

		if(of_property_read_bool(ser, "continuous-clock")) {
			dev_info(dev, "%s: - continuous clock enabled\n",
				 __func__);
			ds90ub953->conts_clk = 1;
		} else {
			/* default value: 0 */
			ds90ub953->conts_clk = 0;
			dev_info(dev, "%s: - discontinuous clock used\n",
				 __func__);
		}

		if(of_property_read_bool(ser, "i2c-pass-through-all")) {
			dev_info(dev, "%s: - i2c-pass-through-all enabled\n",
				 __func__);
			ds90ub953->i2c_pt = 1;
		} else {
			/* default value: 0 */
			ds90ub953->i2c_pt = 0;
			dev_info(dev, "%s: - i2c-pass-through-all disabled\n",
				 __func__);
		}

		err = of_property_read_u32(ser, "virtual-channel-map", &val);
		if(err) {
			dev_info(dev, "%s: - virtual-channel-map property not found\n",
				 __func__);
			ds90ub953->vc_map = 0xE4;
			dev_info(dev, "%s: - virtual-channel-map set to default val: 0xE4\n",
				 __func__);
		} else {
			/* set vc_map*/
			ds90ub953->vc_map = val;
			dev_info(dev, "%s: - virtual-channel-map 0x%x\n", __func__, val);
		}

		/* all initialization of this serializer complete */
		ds90ub953->initialized = 1;
		priv->num_ser += 1;
		dev_info(dev, "%s: serializer %i successfully parsed\n", __func__,
			 counter);
next:
		counter +=1;
	}
	dev_info(dev, "%s: done\n", __func__);
	return 0;

}

/*------------------------------------------------------------------------------
 * PROBE FUNCTION
 *----------------------------------------------------------------------------*/

static int ds90ub954_probe(struct i2c_client *client,
			   const struct i2c_device_id *id)
{
	struct ds90ub954_priv *priv;
	struct device *dev = &client->dev;
	int err;
	int i = 0;

	dev_info(dev, "%s: start\n", __func__);

	priv = devm_kzalloc(dev, sizeof(struct ds90ub954_priv), GFP_KERNEL);
	if(!priv)
		return -ENOMEM;

	priv->client = client;
	i2c_set_clientdata(client, priv);

	/* force to select the rx port the first time */
	priv->sel_rx_port = -1;

	/* force to set ia config the first time */
	priv->sel_ia_config = -1;

	err = ds90ub954_parse_dt(priv);
	if(unlikely(err < 0)) {
		dev_err(dev, "%s: error parsing device tree\n", __func__);
		goto err_parse_dt;
	}

	err = ds90ub954_init_gpio(priv);
	if(unlikely(err < 0)) {
		dev_err(dev, "%s: error initializing gpios\n", __func__);
		goto err_init_gpio;
	}

	priv->regmap = devm_regmap_init_i2c(client, &ds90ub954_regmap_config);
	if(IS_ERR_VALUE(priv->regmap)) {
		err = PTR_ERR(priv->regmap);
		dev_err(dev, "%s: regmap init failed (%d)\n", __func__, err);
		goto err_regmap;
	}

	ds90ub953_parse_dt(client, priv);

	/* turn on deserializer */
	ds90ub954_pwr_enable(priv);

	msleep(6); // wait for sensor to start

	/* init deserializer */
	err = ds90ub954_init(priv, 0);
	if(unlikely(err)) {
		dev_err(dev, "%s: error initializing ds90ub954\n", __func__);
		goto err_regmap;
	}
	dev_info(dev, "%s: init ds90ub954_done\n", __func__);

	msleep(500);

	/* init serializers */
	for( ; i<priv->num_ser; i++) {
		/* check if serializer is initialized */
		if(priv->ser[i]->initialized == 0)
			continue;
		/*init serializer*/
		err = ds90ub953_init(priv->ser[i]);
		if(err) {
			dev_info(dev,
				"serializer %i init_serializer failed\n", i);
			continue;
		}
	}

	msleep(500);

#ifdef ENABLE_SYSFS_TP
	/* device attribute on sysfs */
	dev_set_drvdata(dev, priv);
	err = device_create_file(dev, &dev_attr_test_pattern_des);
	if(unlikely(err < 0))
		dev_err(dev, "deserializer cant create device attribute %s\n",
			dev_attr_test_pattern_des.attr.name);
#endif

	return 0;

err_regmap:
	ds90ub953_free(priv);
	ds90ub954_pwr_disable(priv);
	ds90ub954_free_gpio(priv);
err_init_gpio:
err_parse_dt:
	devm_kfree(dev, priv);
	return err;
}

static int ds90ub954_remove(struct i2c_client *client)
{
	struct ds90ub954_priv *priv = dev_get_drvdata(&client->dev);

	ds90ub953_free(priv);
	ds90ub954_pwr_disable(priv);
	ds90ub954_free_gpio(priv);

	dev_info(&client->dev, "ds90ub954 removed\n");
	return 0;
}

static const struct i2c_device_id ds90ub954_id[] = {
	{"ti,ds90ub954", 0},
	{/* sentinel */},
};

static struct i2c_driver ds90ub954_driver = {
	.driver =
	{
		.name = "i2c-ds90ub954",
		.of_match_table = of_match_ptr(ds90ub954_of_match),
	},
	.probe = ds90ub954_probe,
	.remove = ds90ub954_remove,
	.id_table = ds90ub954_id,
};

MODULE_DEVICE_TABLE(of, ds90ub954_of_match);

MODULE_DEVICE_TABLE(i2c, ds90ub954_id);

module_i2c_driver(ds90ub954_driver);

MODULE_AUTHOR("Philipp Huber <hubp@zhaw.ch>");
MODULE_AUTHOR("Simone Schwizer <sczr@zhaw.ch>");
MODULE_DESCRIPTION("i2c ds90ub954 driver");
MODULE_LICENSE("GPL v2");
