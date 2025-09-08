#ifndef _VEYE_GXCAM_H_
#define _VEYE_GXCAM_H_
//typedef unsigned long u32;

/*
    register list that support directly read
    Model Name 
    FormatCap
    ReadModeCap
    LaneNumCap
    Camera name
    Camera name
    Camera name
    Camera name
    Camera name
    Camera name
    Camera name
    Camera name
    I2C ADDR
    Pixel Format
    Sensor Width
    Sensor Height
    MaxFrame Rate
    Framerate
    ROI Width
    ROI Height
    ROI Offset X
    ROI Offset Y
    MIPI Data rate
    Min Width
    Min Height
*/

/* GX series registers begin, the same as mv serial */
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
#define LaneNum_Cap 0x0034
#define CameraModel0 0x0038
#define CameraModel1 0x003C
#define CameraModel2 0x0040 
#define CameraModel3 0x0044 
#define CameraModel4 0x0048 
#define CameraModel5 0x004C 
#define CameraModel6 0x0050 
#define CameraModel7 0x0054 

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
#define Sync_Role 0x460

#define Test_Image_Selector 0x800
#define Pixel_Format 0x804
#define Sensor_Width 0x808
#define Sensor_Height 0x80C
#define MaxFrame_Rate 0x810
//#define Framerate 0x814 //discard
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
#define MIN_ROI_Width 0x844
#define MIN_ROI_Height 0x848U
#define Framerate_ex 0x850//10000*fps

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


/* user define v4l2 controls*/
#define V4L2_CID_VEYE_GX_BASE					(V4L2_CID_USER_BASE + 0x1100)
#define V4L2_CID_VEYE_GX_TRIGGER_MODE			(V4L2_CID_VEYE_GX_BASE + 1)
#define V4L2_CID_VEYE_GX_TRIGGER_SRC			(V4L2_CID_VEYE_GX_BASE + 2)
#define V4L2_CID_VEYE_GX_SOFT_TRGONE			(V4L2_CID_VEYE_GX_BASE + 3)
#define V4L2_CID_VEYE_GX_SYNC_ROLE				(V4L2_CID_VEYE_GX_BASE + 4)
#define V4L2_CID_VEYE_GX_FRAME_RATE				(V4L2_CID_VEYE_GX_BASE + 5)
//#define V4L2_CID_VEYE_GX_ROI_X		        	(V4L2_CID_VEYE_GX_BASE + 5)
//#define V4L2_CID_VEYE_GX_ROI_Y  				(V4L2_CID_VEYE_GX_BASE + 6)

enum enum_TriggerMode{
    Video_Streaming_mode = 0,
    Normal_Trigger_mode = 1,
    Rolling_Shutter_MultiFrame_Trigger_mode = 2,
    Pulse_trigger_mode = 3,
    Sync_mode = 4,
    Trigger_mode_num,
};

enum enum_TriggerSrc{
    Trg_Soft = 0,
    Trg_Hard = 1,
    Trg_src_num,
};

enum enum_TriggerSyncRole{
    Sync_Master = 0,
    Sync_Slave = 1,
    Sync_role_num,
};
#define I2C_READ_RETRY_COUNT 1
#define I2C_WRITE_RETRY_COUNT 1

/* GX mipi datarate is 1.188Gbps */
#define GXCAM_DEFAULT_MIPI_DATARATE     1188000000
/* GX mipi clk is 742.5Mhz */
#define GXCAM_DEFAULT_LINK_FREQ     742500000
//pixel_rate = link_freq * 2 * nr_of_lanes / bits_per_sample
#define GXCAM_PIXEL_RATE	594000000 //1.188Gbps*2*2/8=594M

#define GXCAM_MIPI_DATA_LANS 2

#define GXCAM_DEF_FPS 60

#define GX_CAM_ROI_W_ALIGN  8U
#define GX_CAM_ROI_H_ALIGN  4U

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
    MV_DT_YUYV = 5,
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

