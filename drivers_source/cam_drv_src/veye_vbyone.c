/*
 * thcv242a.c - Thine THCV242A deserializer and THCV241A serializer driver
 *
 * Copyright (c) 2023, www.veye.cc, TIANJIN DATA IMAGING TECHNOLOGY CO.,LTD
 *
 * This program is for the THCV242A V-by-ONE deserializer in connection
 * with the SHA241 serializer from Thine
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
#include "veye_vbyone.h"

const struct regmap_config thcv242a_regmap_config = {
	.reg_bits = 16,
	.val_bits = 8,
};

const struct regmap_config thcv241a_regmap_config_orig = {
	.reg_bits = 16,
	.val_bits = 8,
};

const struct regmap_config thcv241a_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
};

/*------------------------------------------------------------------------------
 * thcv242a FUNCTIONS
 *----------------------------------------------------------------------------*/
/*
static int thcv242a_read(struct thcv242a_priv *priv, unsigned int reg,
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
*/

static int thcv242a_write(const struct thcv242a_priv *priv, unsigned int reg,
			   unsigned int val)
{
	int err;

	err = regmap_write(priv->regmap, reg, val);
	if(err) {
		dev_err(&priv->client->dev,
			"Cannot write register 0x%02x (%d)!\n", reg, err);
	}
    //dev_err(&priv->client->dev,"W242 0x%x : 0x%x\n", reg,val);
	return err;
}

/*
static int thcv241a_read(struct thcv241a_priv *priv, unsigned int reg,
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
*/

static int thcv241a_write(const struct thcv241a_priv *priv, unsigned int reg,
			   unsigned int val)
{
	int err;
	err = regmap_write(priv->regmap, reg, val);
	if(err) {
		dev_err(&priv->client->dev,
			"Cannot write subdev 0x%02x register 0x%02x (%d)!\n",
			priv->client->addr, reg, err);
	}
    //dev_err(&priv->client->dev,"W241 0x%x : 0x%x\n", reg,val);
	return err;
}
/*
void print_i2c_client_info(struct i2c_client *client) {
    if (client) {
        printk("i2c bus: %s\n", dev_name(&client->dev));

        // Additional information you want to print
    } else {
        printk("Invalid i2c_client structure.\n");
    }
}*/

/*
void print_regmap_info(struct regmap *regmap) {
    if (regmap) {
        unsigned int reg_stride = regmap_get_reg_stride(regmap);
        unsigned int val_bytes = regmap_get_val_bytes(regmap);
        
        printk("Register address length: %u bytes\n", reg_stride);
        printk("Register value length: %u bytes\n", val_bytes);

        // Additional information you want to print
    } else {
        printk("Invalid regmap structure.\n");
    }
}*/

static int thcv242a_init_pre(struct thcv242a_priv *priv)
{
    int err = 0;
    //unsigned int val;
    struct device *dev = &priv->client->dev;
    dev_info(dev, "%s: begin \n", __func__);
    
    err |= thcv242a_write(priv,R_2WIREPT1_PASS_ADRIN0,priv->ser[0]->i2c_address);//0x34
    err |= thcv242a_write(priv,0x0004,0x03);
    err |= thcv242a_write(priv,0x0010,0x10);
    err |= thcv242a_write(priv,0x1704,0x01);
    err |= thcv242a_write(priv,0x0102,0x02);
    
    err |= thcv242a_write(priv,0x0103,0x02);
    err |= thcv242a_write(priv,0x0104,0x02);
    err |= thcv242a_write(priv,0x0105,0x02);
    err |= thcv242a_write(priv,0x0100,0x03);
    err |= thcv242a_write(priv,0x010F, 0x25);
    err |= thcv242a_write(priv,0x010A, 0x15);
    err |= thcv242a_write(priv,0x0031, 0x02);
    
    dev_info(dev, "%s:  successfully \n", __func__);
    
    //err |= thcv242a_read(priv,0x010A, &val);
    //dev_info(dev, "thcv242a_read 0x010A val is %x ,should be 0x15\n",val );
    
    //err |= thcv242a_read(priv,0x0031, &val);
    //dev_info(dev, "thcv242a_read 0x31 val is %x ,should be 0x2\n",val );
    return err;
}

static int thcv241a_regmap_update(struct thcv242a_priv *priv, int ser_nr);

static int thcv241a_init(struct thcv242a_priv *despriv,struct thcv241a_priv *serpriv)
{
    int err = 0;
    //unsigned int val = 0;
    unsigned int io_type = 0;
    unsigned int io_dir = 0;
    struct device *dev = &despriv->client->dev;
    dev_info(dev, "%s: begin \n", __func__);
    
    //dev_info(dev, "=============des reg map info============\n");
    //print_i2c_client_info(despriv->client);
    //print_regmap_info(despriv->regmap);
    //dev_info(dev, "=============des reg map info==END==========\n");
    
    //dev_info(dev, "=============ser reg map info============\n");
    //print_i2c_client_info(serpriv->client);
    //print_regmap_info(serpriv->regmap);
    //dev_info(dev, "=============ser reg map info==END==========\n");
    
    err |= thcv242a_write(despriv,R_2WIREPT_WA_DATA_BYTE, 0x10);
    
    //err |= thcv242a_read(despriv,0x0032, &val);
    //dev_info(dev, "thcv242a_read 0x0032 val is %x ,should be 0x10\n",val );
    
    err |= thcv241a_write(serpriv,0x00FE, 0x11);
    
   // err |= thcv241a_read(serpriv,0x00FE, &val);
    //dev_info(dev, "thcv241a_read 0x00FE val is %x ,should be 0x11\n",val );
    
    
    err |= thcv242a_write(despriv,R_2WIREPT_WA_DATA_BYTE, 0x00);
    //change to 8bit add and 8bit val
    err |= thcv241a_regmap_update(despriv,0);
    dev_info(dev, "%s: change 241 addr to 8bit addr \n", __func__);
    
    err |= thcv241a_write(serpriv,0xF3,0x00);
    err |= thcv241a_write(serpriv,0xF2,0x22);
    
    //err |= thcv241a_read(serpriv,0xF2, &val);
    //dev_info(dev, "thcv241a_read 0xF2 val is %x ,should be 0x22\n",val );
    
    err |= thcv241a_write(serpriv,0xF0,0x03);
    
    //err |= thcv241a_read(serpriv,0xF0, &val);
    //dev_info(dev, "thcv241a_read 0xF0 val is %x ,should be 0x3\n",val );
    
    err |= thcv241a_write(serpriv,0xFF,0x19);
    err |= thcv241a_write(serpriv,0xF6,0x15);
    err |= thcv241a_write(serpriv,0xC9,0x05);
    err |= thcv241a_write(serpriv,0xCA,0x05);
    err |= thcv241a_write(serpriv,0xFE,0x21);
    err |= thcv241a_write(serpriv,0x76,0x10);
    err |= thcv241a_write(serpriv,0x0F,0x01);
    
    if(serpriv->csi_lane_speed == 1500){
        err |= thcv241a_write(serpriv,0x11,0x29);
        err |= thcv241a_write(serpriv,0x12,0xAA);
        err |= thcv241a_write(serpriv,0x13,0xAA);
        err |= thcv241a_write(serpriv,0x14,0xAA);
        err |= thcv241a_write(serpriv,0x15,0x43);
    }else if(serpriv->csi_lane_speed == 1188){
        err |= thcv241a_write(serpriv,0x11,0x2C);
        err |= thcv241a_write(serpriv,0x12,0x00);
        err |= thcv241a_write(serpriv,0x13,0x00);
        err |= thcv241a_write(serpriv,0x14,0x00);
        err |= thcv241a_write(serpriv,0x15,0x44);
    }else{
        dev_info(dev, "thcv241a_init csi lan speed not supported\n" );
    }
    err |= thcv241a_write(serpriv,0x16,0x01);
    err |= thcv241a_write(serpriv,0x00,0x00);
    err |= thcv241a_write(serpriv,0x01,0x00);
    err |= thcv241a_write(serpriv,0x02,0x00);
    err |= thcv241a_write(serpriv,0x55,0x00);
    err |= thcv241a_write(serpriv,0x04,0x00);
    err |= thcv241a_write(serpriv,0x2B,0x04);
    err |= thcv241a_write(serpriv,0x2F,0x00);
    err |= thcv241a_write(serpriv,0x2D,0x11);
    err |= thcv241a_write(serpriv,0x2C,0x01);
    err |= thcv241a_write(serpriv,0x05,0x01);
    err |= thcv241a_write(serpriv,0x06,0x01);
    err |= thcv241a_write(serpriv,0x27,0x00);
    err |= thcv241a_write(serpriv,0x1D,0x00);
    err |= thcv241a_write(serpriv,0x1E,0x00);
    
    if(despriv->trgin_gpio_mode){
        io_type |= 0x1; //polling
        io_dir &= 0xFE;// output bit0-->0
    }
    if(despriv->out1_gpio_mode){
        io_type |= 0x4;//polling
        io_dir |= 0x4;// input
    }else{
        io_dir |= 0x4;//Inconsistent with the datasheet, consistent with the tool.
    }
    if(despriv->out2_gpio_mode){
        io_type |= 0x8;//polling
        io_dir |= 0x8;// input
    }
    io_dir |= 0x20;

    err |= thcv241a_write(serpriv,R_GPIO_TYP,io_type);
    
    err |= thcv241a_write(serpriv,R_GPIO_OEN,io_dir);
    
    err |= thcv241a_write(serpriv,R_GPIO_CMOSEN,0x0F);//CMOS mode
    
    dev_info(dev, "Set 241 IO config 0x%x: 0x%x; 0x%x:0x%x; 0x%x:0x%x \n",R_GPIO_TYP,io_type,R_GPIO_OEN,io_dir,R_GPIO_CMOSEN,0x0F );
    
    dev_info(dev, "%s:  successfully \n", __func__);
    
   // err |= thcv241a_read(serpriv,0x3F, &val);
   // dev_info(dev, "thcv241a_read 0x3F val is %x ,should be 0xF\n",val );
    return err;
}

static int thcv242a_init_post(struct thcv242a_priv *priv)
{
    int err = 0;
    unsigned int io_mode = 0;
    struct thcv241a_priv * serpriv = priv->ser[0];
    struct device *dev = &priv->client->dev;
    dev_info(dev, "%s: begin \n", __func__);
    err |= thcv242a_write(priv,0x0010,0x11);
    err |= thcv242a_write(priv,0x1010,0xA1);
    err |= thcv242a_write(priv,0x1011,0x06);
    err |= thcv242a_write(priv,0x1012,0x00);
    err |= thcv242a_write(priv,0x1021,0x20);
    err |= thcv242a_write(priv,0x1022,0x02);
    err |= thcv242a_write(priv,0x1023,0x11);
    err |= thcv242a_write(priv,0x1024,0x00);
    err |= thcv242a_write(priv,0x1025,0x00);
    err |= thcv242a_write(priv,0x1026,0x00);
    err |= thcv242a_write(priv,0x1027,0x07);
    err |= thcv242a_write(priv,0x1028,0x02);
    err |= thcv242a_write(priv,0x1030,0x00);
    err |= thcv242a_write(priv,0x1100,0x01);
    err |= thcv242a_write(priv,0x1101,0x01);
    err |= thcv242a_write(priv,0x1102,0x01);
    err |= thcv242a_write(priv,0x1600,0x1A);
    err |= thcv242a_write(priv,0x1605,0x29);
    err |= thcv242a_write(priv,0x1606,0x44);
    err |= thcv242a_write(priv,0x161F,0x00);
    if(serpriv->csi_lane_speed == 1500){
        err |= thcv242a_write(priv,0x1609,0x0E);
        err |= thcv242a_write(priv,0x160A,0x18);
        err |= thcv242a_write(priv,0x160B,0x0C);
        err |= thcv242a_write(priv,0x160D,0x11);
        err |= thcv242a_write(priv,0x160E,0x06);
        err |= thcv242a_write(priv,0x160F,0x09);
        err |= thcv242a_write(priv,0x1610,0x05);
        err |= thcv242a_write(priv,0x1611,0x1A);
        err |= thcv242a_write(priv,0x1612,0x0D);
    }else if(serpriv->csi_lane_speed == 1188){
        err |= thcv242a_write(priv,0x1609,0x0B);
        err |= thcv242a_write(priv,0x160A,0x12);
        err |= thcv242a_write(priv,0x160B,0x0A);
        err |= thcv242a_write(priv,0x160D,0x0E);
        err |= thcv242a_write(priv,0x160E,0x03);
        err |= thcv242a_write(priv,0x160F,0x07);
        err |= thcv242a_write(priv,0x1610,0x04);
        err |= thcv242a_write(priv,0x1611,0x14);
        err |= thcv242a_write(priv,0x1612,0x0B);
    }else{
        dev_info(dev, "thcv241a_init csi lan speed not supported\n" );
    }
    
    err |= thcv242a_write(priv,0x1703,0x01);
    err |= thcv242a_write(priv,0x1704,0x11);
    
    if(priv->out1_gpio_mode){
        io_mode |= 0x4; //Through GPo Mode
    }
    if(priv->out2_gpio_mode){
        io_mode |= 0x40;//Through GPo Mode
    }
    err |= thcv242a_write(priv,R_GPIO23_MODE,io_mode);
    dev_info(dev, "write 242 --- 0x1003:  0x%x \n", io_mode);
    if(priv->trgin_gpio_mode){
        io_mode |= 0x3; //Through GPI Mode
    }
    err |= thcv242a_write(priv,R_GPIO01_MODE,io_mode);
    dev_info(dev, "write 242 --- 0x1004:  0x%x \n", io_mode);
    
    err |= thcv242a_write(priv,0x001B,0x18);
    err |= thcv242a_write(priv,R_2WIREPT_WA_DATA_BYTE,0x10);
    err |= thcv242a_write(priv,R_2WIREPT1_PASS_ADR000,priv->cam_i2c_address);
    err |= thcv242a_write(priv,R_2WIREPT1_PASS_ADR001,priv->cam_i2c_address);
    
    err |= thcv242a_write(priv,R_2WIREPT_WA_DATA_BYTE,priv->cam_i2c_pt_setting);
    dev_info(dev, "%s:  successfully \n", __func__);
    return err;
}

static int thcv242a_init_gpio(const struct thcv242a_priv *priv)
{
    return 0;
}

static void thcv242a_free_gpio(const struct thcv242a_priv *priv)
{

}

static void thcv242a_pwr_enable(const struct thcv242a_priv *priv)
{
	
}

static void thcv242a_pwr_disable(const struct thcv242a_priv *priv)
{
	
}

static int thcv242a_parse_dt(struct thcv242a_priv *priv)
{
	struct device *dev = &priv->client->dev;
	struct device_node *np = dev->of_node;

	int err = 0;
	int val = 0;
    
	if(!np)
		return -ENODEV;

	dev_info(dev, "%s: deserializer:\n", __func__);

	err = of_property_read_u32(np, "csi-lane-count", &val);
	if(err) {
		dev_info(dev, "%s: - csi-lane-count property not found\n", __func__);

		/* default value: 2 */
		priv->csi_lane_count = 2;
		dev_info(dev, "%s: - csi-lane-count set to default val: 4\n", __func__);
	} else {
		/* set csi-lane-count*/
		priv->csi_lane_count = val;
		dev_info(dev, "%s: - csi-lane-count %i\n", __func__, val);
	}

    err = of_property_read_u32(np, "coax-num", &val);
	if(err) {
		dev_info(dev, "%s: - coax-num property not found\n", __func__);

		priv->coax_num = 1;
		dev_info(dev, "%s: - coax_num set to default val: 1\n", __func__);
	} else {
		priv->coax_num = val;
		dev_info(dev, "%s: - coax_num %i\n", __func__, val);
	}

    err = of_property_read_u32(np, "cam-i2c-pt-setting", &val);
	if(err) {
		dev_info(dev, "%s: - cam-i2c-pt-setting not found\n", __func__);

		priv->cam_i2c_pt_setting = 0x13;
		dev_info(dev, "%s: - cam_i2c_pt_setting set to default val: 0x12\n", __func__);
	} else {
		priv->cam_i2c_pt_setting = val;
		dev_info(dev, "%s: - cam_i2c_pt_setting %i\n", __func__, val);
	}

    err = of_property_read_u32(np, "camera-i2c-address", &val);
    if(err) {
        dev_info(dev, "%s: - camera-i2c-address not found\n", __func__);
        priv->cam_i2c_address = 0x3b;
        dev_info(dev, "%s: - camera-i2c-address set to default val: 0x18\n",
             __func__);
    } else {
        dev_info(dev, "%s: - camera-i2c-address: 0x%X \n", __func__, val);
        priv->cam_i2c_address=val;
    }
    
    err = of_property_read_u32(np, "trgin-gpio-mode", &val);
    if (err) {
       dev_info(dev, "Failed to read trgin-gpio: %d\n", err);
        priv->trgin_gpio_mode = GPIO_MODE_NO_USE;
    }else{
        priv->trgin_gpio_mode = val;
    }
    
    err = of_property_read_u32(np, "out1-gpio-mode", &val);
    if (err) {
       dev_info(dev, "Failed to read out1-gpio: %d\n", err);
        priv->out1_gpio_mode = GPIO_MODE_NO_USE;
    }else{
        priv->out1_gpio_mode = val;
    }
    
    err = of_property_read_u32(np, "out2-gpio-mode", &val);
    if (err) {
       dev_info(dev, "Failed to read trgin-gpio: %d\n", err);
        priv->out2_gpio_mode = GPIO_MODE_NO_USE;
    }else{
        priv->out2_gpio_mode = val;
    }
    
	return 0;

}

/*------------------------------------------------------------------------------
 * THCV241A FUNCTIONS
 *----------------------------------------------------------------------------*/

static void thcv241a_free(struct thcv242a_priv *priv)
{
	i2c_unregister_device(priv->ser[0]->client);
}

//orignal 16bit addr,8bit val
static int thcv241a_regmap_init(struct thcv242a_priv *priv, int ser_nr)
{
	struct regmap *new_regmap = NULL;
	struct device *dev = &priv->client->dev;
	int err = 0;

	/* setup now regmap */
	new_regmap = devm_regmap_init_i2c(priv->ser[ser_nr]->client,
					  &thcv241a_regmap_config_orig);
	if(IS_ERR_VALUE(priv->regmap)) {
		err = PTR_ERR(priv->regmap);
		dev_err(dev, "regmap init of subdevice failed (%d)\n", err);
		return err;
	}
	dev_info(dev, "%s init regmap done\n", __func__);

	priv->ser[ser_nr]->regmap = new_regmap;
	return err;
}

//update to addr,8bit val
static int thcv241a_regmap_update(struct thcv242a_priv *priv, int ser_nr)
{
    struct regmap *new_regmap = NULL;
	struct device *dev = &priv->client->dev;
	int err = 0;
    
    if(priv->ser[ser_nr]->regmap){
        regmap_exit(priv->ser[ser_nr]->regmap);
    }
	/* setup now regmap */
	new_regmap = devm_regmap_init_i2c(priv->ser[ser_nr]->client,
					  &thcv241a_regmap_config);
	if(IS_ERR_VALUE(priv->regmap)) {
		err = PTR_ERR(priv->regmap);
		dev_err(dev, "regmap init of subdevice failed (%d)\n", err);
		return err;
	}
	priv->ser[ser_nr]->regmap = new_regmap;
	dev_info(dev, "%s regmap done\n", __func__);
	return err;
}

static int thcv241a_alloc(struct thcv242a_priv *priv, int ser_nr)
{
	struct thcv241a_priv *priv_ser;
	struct device *dev = &priv->client->dev;

	priv_ser = devm_kzalloc(dev, sizeof(struct thcv241a_priv), GFP_KERNEL);
	if(!priv)
		return -ENOMEM;

	priv->ser[ser_nr] = priv_ser;
	priv->ser[ser_nr]->initialized = 0;
	return 0;
}

static int thcv241a_i2c_client(struct thcv242a_priv *priv, int ser_nr,
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

static int thcv241a_parse_dt(struct i2c_client *client,
			      struct thcv242a_priv *priv)
{
	struct device *dev = &client->dev;
	struct device_node *des = dev->of_node;
	struct device_node *ser;
	struct thcv241a_priv *thcv241a;

	u32 val = 0;
	int err = 0;

	/* get serializers device_node from dt */
	ser = of_get_child_by_name(des, "serializer");
	if(!ser) {
		dev_info(dev, "%s: no serializer found in device tree\n",
			 __func__);
		return 0;
	}

	dev_info(dev, "%s: parsing serializers device tree:\n", __func__);

    /* allocate memory for serializer */
    err = thcv241a_alloc(priv, 0);
    if(err) {
        dev_info(dev, "%s: - allocating thcv241a failed\n",
             __func__);
        goto ERR;
    }
    thcv241a = priv->ser[0];

    /* get i2c address */
    err = of_property_read_u32(ser, "i2c-address", &val);
    if(err) {
        dev_info(dev, "%s: - i2c-address not found\n", __func__);
        thcv241a->i2c_address = 0x34;
        dev_info(dev, "%s: - i2c-address set to default val: 0x18\n",
             __func__);
    } else {
        dev_info(dev, "%s: - i2c-address: 0x%X \n", __func__, val);
        thcv241a->i2c_address=val;
    }

    err = of_property_read_u32(ser, "csi-lane-count", &val);
    if(err) {
        dev_info(dev, "%s: - csi-lane-count property not found\n",
             __func__);
        /* default value: 2 */
        thcv241a->csi_lane_count = 2;
        dev_info(dev, "%s: csi-lane-count set to default val: 2\n",
             __func__);
    } else {
        /* set csi-lane-count*/
        thcv241a->csi_lane_count = val;
        dev_info(dev, "%s: - csi-lane-count %i\n", __func__, val);
    }

    err = of_property_read_u32(ser, "csi-lane-speed", &val);
    if(err) {
        dev_info(dev, "%s: - csi-lane-speed property not found\n",
             __func__);
        thcv241a->csi_lane_speed = 1500;
        dev_info(dev, "%s: csi-lane-speed set to default val: 2\n",
             __func__);
    } else {
        /* set csi-lane-count*/
        thcv241a->csi_lane_speed = val;
        dev_info(dev, "%s: - csi-lane-speed %i\n", __func__, val);
    }
    if(thcv241a->csi_lane_speed != 1500 && thcv241a->csi_lane_speed != 1188)
    {
        dev_err(dev, "%s: - csi-lane-speed %i not supported,will exit!\n", __func__, val);
        goto ERR;
    }
    
    err = thcv241a_i2c_client(priv, 0,  thcv241a->i2c_address);
    if(err) {
        dev_info(dev, "%s: - thcv241a_i2c_client failed\n",
             __func__);
        goto ERR;
    }

    err = thcv241a_regmap_init(priv, 0);
    if(err) {
        dev_info(dev, "%s: - thcv241a_regmap_init failed\n",
             __func__);
        goto ERR;
    }

    /* all initialization of this serializer complete */
    thcv241a->initialized = 1;
    dev_info(dev, "%s: serializer %i successfully parsed\n", __func__,0);

	return 0;
ERR:
	return -1;
}

/*------------------------------------------------------------------------------
 * PROBE FUNCTION
 *----------------------------------------------------------------------------*/

static int thcv242a_probe(struct i2c_client *client,
			   const struct i2c_device_id *id)
{
	struct thcv242a_priv *priv;
	struct device *dev = &client->dev;
	int err;

	dev_info(dev, "%s: start\n", __func__);

	priv = devm_kzalloc(dev, sizeof(struct thcv242a_priv), GFP_KERNEL);
	if(!priv)
		return -ENOMEM;

	priv->client = client;
	i2c_set_clientdata(client, priv);

	err = thcv242a_parse_dt(priv);
	if(unlikely(err < 0)) {
		dev_err(dev, "%s: error parsing device tree\n", __func__);
		goto err_parse_dt;
	}

	err = thcv242a_init_gpio(priv);
	if(unlikely(err < 0)) {
		dev_err(dev, "%s: error initializing gpios\n", __func__);
		goto err_init_gpio;
	}

	priv->regmap = devm_regmap_init_i2c(client, &thcv242a_regmap_config);
	if(IS_ERR_VALUE(priv->regmap)) {
		err = PTR_ERR(priv->regmap);
		dev_err(dev, "%s: regmap init failed (%d)\n", __func__, err);
		goto err_regmap;
	}

	thcv241a_parse_dt(client, priv);

	/* turn on deserializer */
	thcv242a_pwr_enable(priv);

	//msleep(6); // wait for sensor to start

	/* init deserializer */
	err = thcv242a_init_pre(priv);
	if(unlikely(err)) {
		dev_err(dev, "%s: error initializing thcv242a pre\n", __func__);
		goto err_regmap;
	}
    msleep(6);
    /*init serializer*/
    err = thcv241a_init(priv,priv->ser[0]);

	if(unlikely(err)) {
		dev_err(dev, "%s: error initializing thcv242a\n", __func__);
		goto err_regmap;
	}
    msleep(6);
	/* init deserializer */
	err = thcv242a_init_post(priv);
	if(unlikely(err)) {
		dev_err(dev, "%s: error initializing thcv242a post\n", __func__);
		goto err_regmap;
	}

	return 0;

err_regmap:
	thcv241a_free(priv);
	thcv242a_pwr_disable(priv);
	thcv242a_free_gpio(priv);
err_init_gpio:
err_parse_dt:
	devm_kfree(dev, priv);
	return err;
}

static int thcv242a_remove(struct i2c_client *client)
{
	struct thcv242a_priv *priv = dev_get_drvdata(&client->dev);

	thcv241a_free(priv);
	thcv242a_pwr_disable(priv);
	thcv242a_free_gpio(priv);

	dev_info(&client->dev, "thcv242a removed\n");
    return 0;
}

static const struct of_device_id veye_vbyone_dt_ids[] = {
	{.compatible = "veye,vbyone"},
	{/* sentinel */}
};
static const struct i2c_device_id veye_vbyone_id[] = {
	{"veye,vbyone", 0},
	{/* sentinel */},
};

static struct i2c_driver thcv242a_driver = {
	.driver = {
		.name = "veye_vbyone",
		.of_match_table = veye_vbyone_dt_ids,
	},
	.probe = thcv242a_probe,
	.remove = thcv242a_remove,
	.id_table = veye_vbyone_id,
};

MODULE_DEVICE_TABLE(of, veye_vbyone_dt_ids);
MODULE_DEVICE_TABLE(i2c, veye_vbyone_id);
module_i2c_driver(thcv242a_driver);

MODULE_AUTHOR("Xu Mengmeng <xumm@csoneplus.com>");
MODULE_DESCRIPTION("V-by-ONE driver from VEYE IMAGING");
MODULE_LICENSE("GPL v2");
