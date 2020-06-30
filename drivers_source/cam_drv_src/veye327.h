/*
 * veye327.h - veye327 sensor mode tables
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
#ifndef __VEYE327_I2C_TABLES__
#define __VEYE327_I2C_TABLES__

#include <media/camera_common.h>
#include <linux/miscdevice.h>

#define VEYE327_TABLE_WAIT_MS	0
#define VEYE327_TABLE_END	1
#define VEYE327_MAX_RETRIES	3
#define VEYE327_WAIT_MS_STOP	1
#define VEYE327_WAIT_MS_START	30
#define VEYE327_WAIT_MS_STREAM	210
#define VEYE327_GAIN_TABLE_SIZE 255

/* #define INIT_ET_INSETTING 1 */

#define veye327_reg struct reg_8

static veye327_reg veye327_start[] = {
	{VEYE327_TABLE_WAIT_MS, VEYE327_WAIT_MS_START},
	{VEYE327_TABLE_WAIT_MS, VEYE327_WAIT_MS_STREAM},
	{VEYE327_TABLE_END, 0x00 }
};

static veye327_reg veye327_stop[] = {
	{VEYE327_TABLE_WAIT_MS, VEYE327_WAIT_MS_STOP},
	{VEYE327_TABLE_END, 0x00 }
};

static veye327_reg veye327_reg_1920x1080_crop_30fps[] = {
	{VEYE327_TABLE_END, 0x00}
};


enum {
	VEYE327_MODE_1920X1080_CROP_30FPS,
	VEYE327_MODE_START_STREAM,
	VEYE327_MODE_STOP_STREAM,
	//VEYE327_MODE_TEST_PATTERN
};

static veye327_reg *mode_table[] = {
	[VEYE327_MODE_1920X1080_CROP_30FPS] = veye327_reg_1920x1080_crop_30fps,
	[VEYE327_MODE_START_STREAM] = veye327_start,
	[VEYE327_MODE_STOP_STREAM] = veye327_stop,
	//[VEYE327_MODE_TEST_PATTERN] = tp_colorbars,
};

static const int veye327_30fps[] = {
	30,
};

/*
 * WARNING: frmfmt ordering need to match mode definition in
 * device tree!
 */
static const struct camera_common_frmfmt veye327_frmfmt[] = {
	{{1920, 1080}, veye327_30fps, 1, 0,
			VEYE327_MODE_1920X1080_CROP_30FPS},
	/* Add modes with no device tree support after below */
};
#endif /* __VEYE327_I2C_TABLES__ */
