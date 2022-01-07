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
    VEYE-MIPI-IMX335
*/

#ifndef __VEYE_IMX335_I2C_TABLES__
#define __VEYE_IMX335_I2C_TABLES__

#include <media/camera_common.h>
#include <linux/miscdevice.h>

#define VEYECAM_TABLE_WAIT_MS	0
#define VEYECAM_TABLE_END	1
#define VEYECAM_MAX_RETRIES	3
#define VEYECAM_WAIT_MS_STOP	1
#define VEYECAM_WAIT_MS_START	30
#define VEYECAM_WAIT_MS_STREAM	310//200-->300
#define VEYECAM_GAIN_TABLE_SIZE 255


/*registers*/
#define Model_Name 0x0004
#define Video_Mode 0x414 
/* #define INIT_ET_INSETTING 1 */

struct reg_32 {
	u32 addr;
	u32 val;
};

#define veyecam_reg struct reg_32

static veyecam_reg VEYECAM_start[] = {
	{VEYECAM_TABLE_WAIT_MS, VEYECAM_WAIT_MS_START},
	{VEYECAM_TABLE_WAIT_MS, VEYECAM_WAIT_MS_STREAM},
	{VEYECAM_TABLE_END, 0x00 }
};

static veyecam_reg VEYECAM_stop[] = {
	{VEYECAM_TABLE_WAIT_MS, VEYECAM_WAIT_MS_STOP},
	{VEYECAM_TABLE_END, 0x00 }
};

static veyecam_reg veyecam_reg_2592x1944_20fps[] = {
    {Video_Mode,0x1},
    {VEYECAM_TABLE_WAIT_MS, VEYECAM_WAIT_MS_STREAM},
	{VEYECAM_TABLE_END, 0x00}
};

static veyecam_reg veyecam_reg_2592x1944_12_5fps[] = {
    {Video_Mode,0x2},
    {VEYECAM_TABLE_WAIT_MS, VEYECAM_WAIT_MS_STREAM},
	{VEYECAM_TABLE_END, 0x00}
};
static veyecam_reg veyecam_reg_2560x1440_25fps[] = {
    {Video_Mode,0x3},
    {VEYECAM_TABLE_WAIT_MS, VEYECAM_WAIT_MS_STREAM},
	{VEYECAM_TABLE_END, 0x00}
};
static veyecam_reg veyecam_reg_2560x1440_30fps[] = {
    {Video_Mode,0x4},
    {VEYECAM_TABLE_WAIT_MS, VEYECAM_WAIT_MS_STREAM},
	{VEYECAM_TABLE_END, 0x00}
};

enum {
	VEYECAM_MODE_2592x1944_20FPS,
    VEYECAM_MODE_2592x1944_12_5FPS,
    VEYECAM_MODE_2560x1440_25FPS,
    VEYECAM_MODE_2560x1440_30FPS,
	VEYECAM_MODE_START_STREAM,
	VEYECAM_MODE_STOP_STREAM,
	//VEYECAM_MODE_TEST_PATTERN
};

static veyecam_reg *mode_table[] = {
	[VEYECAM_MODE_2592x1944_20FPS] = veyecam_reg_2592x1944_20fps,
    [VEYECAM_MODE_2592x1944_12_5FPS] = veyecam_reg_2592x1944_12_5fps,
    [VEYECAM_MODE_2560x1440_25FPS] = veyecam_reg_2560x1440_25fps,
    [VEYECAM_MODE_2560x1440_30FPS] = veyecam_reg_2560x1440_30fps,
	[VEYECAM_MODE_START_STREAM] = VEYECAM_start,
	[VEYECAM_MODE_STOP_STREAM] = VEYECAM_stop,
	//[VEYECAM_MODE_TEST_PATTERN] = tp_colorbars,
};

static const int veyecam_20fps[] = {
	20,
};
static const int veyecam_12_5fps[] = {
	13,
};
static const int veyecam_25fps[] = {
	25,
};

static const int veyecam_30fps[] = {
	30,
};

/*
 * WARNING: frmfmt ordering need to match mode definition in
 * device tree!
 */
static const struct camera_common_frmfmt veyecam_frmfmt[] = {
	{{2592, 1944}, veyecam_20fps, 1, 0,
			VEYECAM_MODE_2592x1944_20FPS},
    {{2592, 1944}, veyecam_12_5fps, 1, 0,
			VEYECAM_MODE_2592x1944_12_5FPS},
    {{2560, 1440}, veyecam_25fps, 1, 0,
			VEYECAM_MODE_2560x1440_25FPS},
    {{2560, 1440}, veyecam_30fps, 1, 0,
			VEYECAM_MODE_2560x1440_30FPS},
	/* Add modes with no device tree support after below */
};
#endif /* __VEYECAM_I2C_TABLES__ */
