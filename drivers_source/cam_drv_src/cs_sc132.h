/*
 * sc132.h - sc132 sensor mode tables
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
#ifndef __CSSC132_I2C_TABLES__
#define __CSSC132_I2C_TABLES__

#include <media/camera_common.h>
#include <linux/miscdevice.h>

#define SC132_TABLE_WAIT_MS	0
#define SC132_TABLE_END	1
#define SC132_MAX_RETRIES	3
#define SC132_WAIT_MS_STOP	1
#define SC132_WAIT_MS_START	30
#define SC132_WAIT_MS_CMD	5
#define SC132_WAIT_MS_STREAM	5

#define SC132_GAIN_TABLE_SIZE 255

/* #define INIT_ET_INSETTING 1 */

#define sc132_reg struct reg_8

typedef enum 
{
	CS_MIPI_IMX307 = 0x0037,	
	CS_LVDS_IMX307 = 0x0038,	
	CS_USB_IMX307 = 0x0039,	
	CS_MIPI_SC132 = 0x0132,	
	CS_LVDS_SC132 = 0x0133,	
	CS_USB_SC132 = 0x0134
}ENProductID;

typedef enum
{
    deviceID = 0x00,
    HardWare = 0x01,
    LoadingDone = 0x02,

    Csi2_Enable = 0x03,
    Fpga_CAP_L = 0x04,
    Fpga_CAP_H = 0x05,
    
    TriggerMode = 0x10,
    SlaveMode = 0x11,
    TrigDly_H = 0x12,
    TrigDly_M = 0x13,
    TrigDly_U = 0x14,
    TrigDly_L = 0x15,
    VerTotalTime_H = 0x16,
    VerTotalTime_L = 0x17,
    HorTotalTime_H = 0x18,
    HorTotalTime_L = 0x19,
    
    //for arm part
    ARM_VER_L = 0x0100,
    ARM_VER_H = 0x0101,
    PRODUCTID_L = 0x0102,
    PRODUCTID_H = 0x0103,
    SYSTEM_RESET = 0x0104,
    PARAM_SAVE = 0x0105,
    VIDEOFMT_CAP = 0x0106, 
    
    VIDEOFMT_NUM = 0x0107,

    FMTCAP_WIDTH_L = 0x0108,
    FMTCAP_WIDTH_H = 0x0109,
    FMTCAP_HEIGHT_L = 0x010A,
    FMTCAP_HEIGHT_H = 0x010B,
    FMTCAP_FRAMRAT_L = 0x010C,
    FMTCAP_FRAMRAT_H = 0x010D,
    
    FMT_WIDTH_L = 0x0180,
    FMT_WIDTH_H = 0x0181,
    FMT_HEIGHT_L = 0x0182,
    FMT_HEIGHT_H = 0x0183,
    FMT_FRAMRAT_L = 0x0184,
    FMT_FRAMRAT_H = 0x0185,
    IMAGE_DIR = 0x0186,
    SYSTEM_REBOOT = 0x0187,
    NEW_FMT_FRAMRAT_MODE = 0x0188,
    NEW_FMT_FRAMRAT_L = 0x0189,
    NEW_FMT_FRAMRAT_H = 0x018A,
    
    EXP_FRM_MODE = 0x020F,
    AE_MODE = 0x0210,
    EXP_TIME_L = 0x0211,
    EXP_TIME_M = 0x0212,
    EXP_TIME_H = 0x0213,
    EXP_TIME_E = 0x0214,

    AGAIN_NOW_DEC = 0x0215,
    AGAIN_NOW_INTER = 0x0216,
    DGAIN_NOW_DEC = 0x0217,
    DGAIN_NOW_INTER = 0x0218,

    AE_SPEED = 0x0219,
    AE_TARGET = 0x021A,
    AE_MAXTIME_L = 0x021B,
    AE_MAXTIME_M = 0x021C,
    AE_MAXTIME_H = 0x021D,
    AE_MAXTIME_E = 0x021E,
    AE_MAXGAIN_DEC = 0x021F,
    AE_MAXGAIN_INTER = 0x0220,
    //ISP cap
    ISP_CAP_L = 0x0200,
    ISP_CAP_M = 0x0201,
    ISP_CAP_H = 0x0202,
    ISP_CAP_E = 0x0203,
    POWER_HZ = 0x0204,
}ECAMERA_REG;

static sc132_reg sc132_start[] = {
	//{SC132_TABLE_WAIT_MS, SC132_WAIT_MS_START},
    {Csi2_Enable,0x01},
	{SC132_TABLE_WAIT_MS, SC132_WAIT_MS_STREAM},
	{SC132_TABLE_END, 0x00 }
};

static sc132_reg sc132_stop[] = {
	//{SC132_TABLE_WAIT_MS, SC132_WAIT_MS_STOP},
    {Csi2_Enable,0x00},
    {SC132_TABLE_WAIT_MS, SC132_WAIT_MS_CMD},
	{SC132_TABLE_END, 0x00 }
};

//video format capbility
//will check the format if supported later
/* Stream and Control Info Struct */
typedef struct _isp_stream_info {
	uint16_t width;
	uint16_t height;
	uint8_t frame_rate;
} ISP_STREAM_INFO;

struct cs_imx307_private {
	u16 module_id;
	int num_streams;
	ISP_STREAM_INFO *stream_info;
};

static sc132_reg sc132_reg_1280x1080_45fps[] = {
    {FMT_WIDTH_L,0x00},
    {FMT_WIDTH_H,0x5},
    {SC132_TABLE_WAIT_MS, SC132_WAIT_MS_CMD},
    {FMT_HEIGHT_L,0x38},
    {FMT_HEIGHT_H,0x4},
    {SC132_TABLE_WAIT_MS, SC132_WAIT_MS_CMD},
    {FMT_FRAMRAT_L,0x2D},
    {FMT_FRAMRAT_H,0x00},
    {SC132_TABLE_WAIT_MS, SC132_WAIT_MS_STREAM},
	{SC132_TABLE_END, 0x00}
};

static sc132_reg sc132_reg_1080x1280_45fps[] = {
    {FMT_WIDTH_L,0x38},
    {FMT_WIDTH_H,0x4},
    {SC132_TABLE_WAIT_MS, SC132_WAIT_MS_CMD},
    {FMT_HEIGHT_L,0x00},
    {FMT_HEIGHT_H,0x5},
    {SC132_TABLE_WAIT_MS, SC132_WAIT_MS_CMD},
    {FMT_FRAMRAT_L,0x2D},
    {FMT_FRAMRAT_H,0x00},
    {SC132_TABLE_WAIT_MS, SC132_WAIT_MS_STREAM},
	{SC132_TABLE_END, 0x00}
};

static sc132_reg sc132_reg_1280x720_crop_60fps[] = {
    {FMT_WIDTH_L,0x00},
    {FMT_WIDTH_H,0x5},
    {SC132_TABLE_WAIT_MS, SC132_WAIT_MS_CMD},
    {FMT_HEIGHT_L,0xD0},
    {FMT_HEIGHT_H,0x2},
    {SC132_TABLE_WAIT_MS, SC132_WAIT_MS_CMD},
    {FMT_FRAMRAT_L,0x3C},
    {FMT_FRAMRAT_H,0x00},
    {SC132_TABLE_WAIT_MS, SC132_WAIT_MS_STREAM},
	{SC132_TABLE_END, 0x00}
};

static sc132_reg sc132_reg_720x1280_crop_60fps[] = {
    {FMT_WIDTH_L,0xD0},
    {FMT_WIDTH_H,0x2},
    {SC132_TABLE_WAIT_MS, SC132_WAIT_MS_CMD},
    {FMT_HEIGHT_L,0x00},
    {FMT_HEIGHT_H,0x5},
    {SC132_TABLE_WAIT_MS, SC132_WAIT_MS_CMD},
    {FMT_FRAMRAT_L,0x3C},
    {FMT_FRAMRAT_H,0x00},
    {SC132_TABLE_WAIT_MS, SC132_WAIT_MS_STREAM},
	{SC132_TABLE_END, 0x00}
};

static sc132_reg sc132_reg_640x480_crop_120fps[] = {
    {FMT_WIDTH_L,0x80},
    {FMT_WIDTH_H,0x2},
    {SC132_TABLE_WAIT_MS, SC132_WAIT_MS_CMD},
    {FMT_HEIGHT_L,0xE0},
    {FMT_HEIGHT_H,0x1},
    {SC132_TABLE_WAIT_MS, SC132_WAIT_MS_CMD},
    {FMT_FRAMRAT_L,0x78},
    {FMT_FRAMRAT_H,0x00},
    {SC132_TABLE_WAIT_MS, SC132_WAIT_MS_STREAM},
	{SC132_TABLE_END, 0x00}
};
static sc132_reg sc132_reg_480x640_crop_120fps[] = {
    {FMT_WIDTH_L,0xE0},
    {FMT_WIDTH_H,0x1},
    {SC132_TABLE_WAIT_MS, SC132_WAIT_MS_CMD},
    {FMT_HEIGHT_L,0x80},
    {FMT_HEIGHT_H,0x2},
    {SC132_TABLE_WAIT_MS, SC132_WAIT_MS_CMD},
    {FMT_FRAMRAT_L,0x78},
    {FMT_FRAMRAT_H,0x00},
    {SC132_TABLE_WAIT_MS, SC132_WAIT_MS_STREAM},
	{SC132_TABLE_END, 0x00}
};

enum {
	SC132_MODE_1280X1080_45FPS,
    SC132_MODE_1080X1280_45FPS,
    SC132_MODE_1280X720_CROP_60FPS,
    SC132_MODE_720X1280_CROP_60FPS,
    SC132_MODE_640X480_CROP_120FPS,
    SC132_MODE_480X640_CROP_120FPS,
	SC132_MODE_START_STREAM,
	SC132_MODE_STOP_STREAM,
	//IMX307_MODE_TEST_PATTERN
};

static sc132_reg *mode_table[] = {
	[SC132_MODE_1280X1080_45FPS] = sc132_reg_1280x1080_45fps,
    [SC132_MODE_1080X1280_45FPS] = sc132_reg_1080x1280_45fps,
    [SC132_MODE_1280X720_CROP_60FPS] = sc132_reg_1280x720_crop_60fps,
    [SC132_MODE_720X1280_CROP_60FPS] = sc132_reg_720x1280_crop_60fps,
    [SC132_MODE_640X480_CROP_120FPS] = sc132_reg_640x480_crop_120fps,
    [SC132_MODE_480X640_CROP_120FPS] = sc132_reg_480x640_crop_120fps,
	[SC132_MODE_START_STREAM] = sc132_start,
	[SC132_MODE_STOP_STREAM] = sc132_stop,
	//[IMX307_MODE_TEST_PATTERN] = tp_colorbars,
};

static const int sc132_45fps[] = {
	45,
};
static const int sc132_60fps[] = {
	60,
};
static const int sc132_120fps[] = {
	120,
};
/*
 * WARNING: frmfmt ordering need to match mode definition in
 * device tree!
 */
static const struct camera_common_frmfmt cs_sc132_frmfmt[] = {
	{{1280, 1080}, sc132_45fps, 1, 0,
	SC132_MODE_1280X1080_45FPS},
    {{1080, 1280}, sc132_45fps, 1, 0,
	SC132_MODE_1080X1280_45FPS},
    {{1280, 720}, sc132_60fps, 1, 0,
	SC132_MODE_1280X720_CROP_60FPS},
    {{720, 1280}, sc132_60fps, 1, 0,
	SC132_MODE_720X1280_CROP_60FPS},
    {{640, 480}, sc132_120fps, 1, 0,
	SC132_MODE_640X480_CROP_120FPS},
    {{480, 640}, sc132_120fps, 1, 0,
	SC132_MODE_480X640_CROP_120FPS},
	/* Add modes with no device tree support after below */
};


#endif /* __CSIMX307_I2C_TABLES__ */
