/*
 * VEYECAM.h - VEYECAM sensor mode tables
 *
 * Copyright (c) 2016-2017, NVIDIA CORPORATION.  All rights reserved.
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

#ifndef __VEYECAM_I2C_TABLES__
#define __VEYECAM_I2C_TABLES__

#include <media/camera_common.h>
#include <linux/miscdevice.h>

#define VEYECAM_TABLE_WAIT_MS	0
#define VEYECAM_TABLE_END	1
#define VEYECAM_MAX_RETRIES	3
#define VEYECAM_WAIT_MS_STOP	1
#define VEYECAM_WAIT_MS_START	30
#define VEYECAM_WAIT_MS_STREAM	210
#define VEYECAM_GAIN_TABLE_SIZE 255

/* #define INIT_ET_INSETTING 1 */

#define veyecam_reg struct reg_8

static veyecam_reg VEYECAM_start[] = {
	{VEYECAM_TABLE_WAIT_MS, VEYECAM_WAIT_MS_START},
	{VEYECAM_TABLE_WAIT_MS, VEYECAM_WAIT_MS_STREAM},
	{VEYECAM_TABLE_END, 0x00 }
};

static veyecam_reg VEYECAM_stop[] = {
	{VEYECAM_TABLE_WAIT_MS, VEYECAM_WAIT_MS_STOP},
	{VEYECAM_TABLE_END, 0x00 }
};

static veyecam_reg veyecam_reg_1920x1080_crop_30fps[] = {
	{VEYECAM_TABLE_END, 0x00}
};


enum {
	VEYECAM_MODE_1920X1080_CROP_30FPS,
	VEYECAM_MODE_START_STREAM,
	VEYECAM_MODE_STOP_STREAM,
	//VEYECAM_MODE_TEST_PATTERN
};

static veyecam_reg *mode_table[] = {
	[VEYECAM_MODE_1920X1080_CROP_30FPS] = veyecam_reg_1920x1080_crop_30fps,
	[VEYECAM_MODE_START_STREAM] = VEYECAM_start,
	[VEYECAM_MODE_STOP_STREAM] = VEYECAM_stop,
	//[VEYECAM_MODE_TEST_PATTERN] = tp_colorbars,
};

static const int veyecam_30fps[] = {
	30,
};

/*
 * WARNING: frmfmt ordering need to match mode definition in
 * device tree!
 */
static const struct camera_common_frmfmt veyecam_frmfmt[] = {
	{{1920, 1080}, veyecam_30fps, 1, 0,
			VEYECAM_MODE_1920X1080_CROP_30FPS},
	/* Add modes with no device tree support after below */
};
#endif /* __VEYECAM_I2C_TABLES__ */
