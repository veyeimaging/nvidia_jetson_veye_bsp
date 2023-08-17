#ifndef _VEYE_MVCAM_H_
#define _VEYE_MVCAM_H_
//typedef unsigned long u32;

#include <media/camera_common.h>
/* mv series registers begin */
#define BaseCommon 0x0000
#define ImageAcqu 0x0400
#define ImageFeature 0x0800
#define ImageSigProc 0x0c00
#define IOCtrl 0x1000
#define FPGACmd 0x1400
#define FPGABlock 0x1800

#define Manufacturer_Name 0x0000
#define Model_Name 0x0004
#define Sensor_Name 0x0008
#define Product_Info 0x000C
#define Device_Version 0x0010
#define System_reset 0x0014
#define Param_save_to_flash 0x0018
#define System_reboot 0x001C
#define Time_stamp 0x0020
#define Error_code 0x0024
#define Format_cap 0x0028


#define Image_Acquisition 0x400
#define Trigger_Mode 0x404
#define Trigger_Source 0x408
#define Trigger_Num 0x40C
#define Trigger_Inerval 0x410
#define Trigger_Software 0x414
#define Trigger_Count 0x418
#define I2C_Addr 0x41C
#define I2C_Port_Sel 0x420
#define Reserved2 0x424
#define User_overlay_enable 0x428
#define User_overlay_zone0 0x42C
#define User_overlay_zone1 0x430
#define User_overlay_zone2 0x434
#define User_overlay_zone3 0x438
#define User_define_zone0 0x43C
#define User_define_zone1 0x440
#define User_define_zone2 0x444
#define User_define_zone3 0x448
#define Slave_mode 0x460

#define Test_Image_Selector 0x800
#define Pixel_Format 0x804
#define Sensor_Width 0x808
#define Sensor_Height 0x80C
#define MaxFrame_Rate 0x810
#define Framerate 0x814
#define ROI_Width 0x818
#define ROI_Height 0x81C
#define ROI_Offset_X 0x820
#define ROI_Offset_Y 0x824
#define Image_Direction 0x828
#define Data_shift 0x82C
#define Black_Level 0x830
#define ReadOut_Mode 0x834
#define Lane_Num 0x83C
#define MIPI_DataRate 0x840
    
#define ISP_module_ctrl 0xC00
#define Exposure_Mode 0xC04
#define Target_Brightness 0xC08
#define Exposure_Time_Source 0xC0C
#define ME_Time 0xC10
#define AE_MAX_Time 0xC14
#define Exp_Time 0xC18
#define Gain_Mode 0xC1C
#define Manual_Gain 0xC20
#define AG_Max_Gain 0xC24
#define Cur_Gain 0xC28
#define AAROIOffsetX 0xC2C
#define AAROIOffsetY 0xC30
#define AAROIWidth 0xC34
#define AAROIHeight 0xC38
#define WB_Mode 0xC3C
#define MWB_Rgain 0xC40
#define MWB_Bgain 0xC44
#define AWBROIOffsetX 0xC48
#define AWBROIOffsetY 0xC4C
#define AWBROIWidth 0xC50
#define AWBROIHeight 0xC54
#define AWB_Rgain 0xC58
#define AWB_Bgain 0xC5C
#define Gamma 0xC60
#define DPC_Start 0xC64
#define DPC_Status 0xC68
#define DPC_Count 0xC6C
#define AAROI_enable 0xC80

#define Trigger_Delay 0x1000
#define Trigger_Activation 0x1004
#define Trigger_Filter_Enable 0x1008
#define Trigger_Filter_Width 0x100C
#define Trigger_Exp_Delay 0x1010
#define GPIOIN_Status 0x1014

#define GPIO1_OutSelect 0x1020
#define GPIO1_Useroutput 0x1024
#define GPIO1_Reverse 0x1028
#define GPIO1_OutStatus 0x102C

#define GPIO2_OutSelect 0x1030
#define GPIO2_Useroutput 0x1034
#define GPIO2_Reverse 0x1038
#define GPIO2_OutStatus 0x103C
/* register ends*/

#define MVCAM_MAX_CTRLS 40

/* user define v4l2 controls*/
#define V4L2_CID_VEYE_MV_BASE					(V4L2_CID_USER_BASE + 0x1000)
#define V4L2_CID_VEYE_MV_TRIGGER_MODE			(V4L2_CID_VEYE_MV_BASE + 1)
#define V4L2_CID_VEYE_MV_TRIGGER_SRC			(V4L2_CID_VEYE_MV_BASE + 2)
#define V4L2_CID_VEYE_MV_SOFT_TRGONE			(V4L2_CID_VEYE_MV_BASE + 3)

#define V4L2_CID_VEYE_MV_FRAME_RATE				(V4L2_CID_VEYE_MV_BASE + 4)
#define V4L2_CID_VEYE_MV_ROI_X		        	(V4L2_CID_VEYE_MV_BASE + 5)
#define V4L2_CID_VEYE_MV_ROI_Y  				(V4L2_CID_VEYE_MV_BASE + 6)

enum enum_TriggerMode{
    Image_Continues = 0,
    Image_Trigger = 1,
    Image_Speed_Trigger = 2,
    Image_trigger_mode_num,
};

enum enum_TriggerSrc{
    Trg_Soft = 0,
    Trg_Hard = 1,
    Trg_Hard_src_num,
};

#define I2C_READ_RETRY_COUNT 1
#define I2C_WRITE_RETRY_COUNT 1

/* device id list of mv series */
#define MV_MIPI_IMX178M 0x0178
#define MV_MIPI_IMX296M 0x0296
#define MV_MIPI_SC130M  0x0130
#define MV_MIPI_IMX265M 0x0265
#define MV_MIPI_IMX264M 0x0264
#define MV_MIPI_IMX287M 0x0287
#define RAW_MIPI_SC132M 0x8132
#define RAW_MIPI_IMX462M 0x8462
#define RAW_MIPI_AR0234M 0x8234

/* MV mipi clk is 1.5Gbps */
#define MVCAM_DEFAULT_LINK_FREQ     1500000000
//pixel_rate = link_freq * 2 * nr_of_lanes / bits_per_sample
#define MV_CAM_PIXEL_RATE	750000000 //1.5Gbps*2*2/8=750M

#define MV_CAM_DEF_FPS      22U

#define MV_IMX178M_ROI_W_MIN    376U
#define MV_IMX178M_ROI_H_MIN    320U
#define MV_SC130M_ROI_W_MIN    64U
#define MV_SC130M_ROI_H_MIN    64U
#define MV_IMX296M_ROI_W_MIN    80U
#define MV_IMX296M_ROI_H_MIN    64U
#define MV_IMX265M_ROI_W_MIN    264U
#define MV_IMX265M_ROI_H_MIN    64U
#define MV_IMX264M_ROI_W_MIN    264U
#define MV_IMX264M_ROI_H_MIN    64U
#define RAW_SC132M_ROI_W_MIN    64U
#define RAW_SC132M_ROI_H_MIN    64U
#define MV_IMX287M_ROI_W_MIN    264U
#define MV_IMX287M_ROI_H_MIN    64U
#define RAW_IMX462M_ROI_W_MIN    368U
#define RAW_IMX462M_ROI_H_MIN    304U
#define RAW_AR0234M_ROI_W_MIN    64U
#define RAW_AR0234M_ROI_H_MIN    64U

#define MV_CAM_ROI_W_ALIGN  8U
#define MV_CAM_ROI_H_ALIGN  4U


#define MV_MIPI_DATA_LANS 2

enum mipi_datatype {
    IMAGE_DT_YUV420_8 = 0x18,
	IMAGE_DT_YUV420_10,

	IMAGE_DT_YUV420CSPS_8 = 0x1C,
	IMAGE_DT_YUV420CSPS_10,
	IMAGE_DT_YUV422_8,
	IMAGE_DT_YUV422_10,
	IMAGE_DT_RGB444,
	IMAGE_DT_RGB555,
	IMAGE_DT_RGB565,
	IMAGE_DT_RGB666,
	IMAGE_DT_RGB888,

	IMAGE_DT_RAW6 = 0x28,
	IMAGE_DT_RAW7,
	IMAGE_DT_RAW8,
	IMAGE_DT_RAW10,
	IMAGE_DT_RAW12,
	IMAGE_DT_RAW14,
};

enum mv_datatype_index{
    MV_DT_Mono8 = 0,
    MV_DT_Mono10 = 1,
    MV_DT_Mono12 = 2,
    MV_DT_Mono14 = 3,
    MV_DT_UYVY = 4,
};

enum yuv_order {
	YUV_ORDER_YUYV = 0,
	YUV_ORDER_YVYU = 1,
	YUV_ORDER_UYVY = 2,
	YUV_ORDER_VYUY = 3,
};

enum bayer_order {
    //Carefully ordered so that an hflip is ^1,
    //and a vflip is ^2.
    BAYER_ORDER_BGGR = 0,
    BAYER_ORDER_GBRG = 1,
    BAYER_ORDER_GRBG = 2,
    BAYER_ORDER_RGGB = 3,
    BAYER_ORDER_GRAY = 4,
};

#endif

