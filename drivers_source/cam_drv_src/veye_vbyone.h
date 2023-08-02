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

#ifndef I2C_THCV241A_H
#define I2C_THCV241A_H

#include <linux/i2c.h>

/*------------------------------------------------------------------------------
 * Deserializer registers
 *----------------------------------------------------------------------------*/
#define R_2WIREPT_WA_DATA_BYTE 0x0032
#define R_2WIREPT1_PASS_ADR000 0x0040
#define R_2WIREPT1_PASS_ADR001 0x0041

#define R_2WIREPT1_PASS_ADRIN0 0x0050

#define R_GPIO23_MODE 0x1003
#define R_GPIO01_MODE 0x1004

/*------------------------------------------------------------------------------
 * Serializer registers
 *----------------------------------------------------------------------------*/

#define R_GPIO_TYP 0x3D
#define R_GPIO_OEN 0x3E
#define R_GPIO_CMOSEN 0x3F


/*------------------------------------------------------------------------------
 * DEFINES
 *----------------------------------------------------------------------------*/

#define NUM_SERIALIZER 1

#define GPIO_MODE_NO_USE 0
#define GPIO_MODE_POLLING 1
#define GPIO_MODE_I2C_CTL 2

struct thcv241a_priv {
	struct i2c_client *client;
	struct regmap *regmap;
	//struct thcv242a_priv *parent;
	
	int i2c_address;
	int csi_lane_count;
	int csi_lane_speed;
	
	int initialized;
};


struct thcv242a_priv {
	struct i2c_client *client;
	struct regmap *regmap;
    
	struct thcv241a_priv *ser[NUM_SERIALIZER]; //serializers

	int csi_lane_count;
	int coax_num;
	int cam_i2c_pt_setting;
    int cam_i2c_address;
    
    int trgin_gpio_mode; // 0: no use ;1 : polling
	int out1_gpio_mode; // 0: no use ;1 : polling
	int out2_gpio_mode; // 0: no use ;1 : polling
    
};

#endif /* I2C_DS90UB954_H */
