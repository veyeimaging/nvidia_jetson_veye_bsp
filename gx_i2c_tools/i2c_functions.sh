#!/bin/bash
#
#This script is a part of mv_mipi_i2c.sh

######################reglist###################################

#0x0000
Manufacturer_Name=0x0000;
Model_Name=0x0004;
Sensor_Name=0x0008;
Serial_Num=0x000C;
Device_Version=0x0010;
System_reset=0x0014;
Param_save_to_flash=0x0018;
System_reboot=0x001C;
Time_stamp=0x0020;
Error_code=0x0024;
Format_Cap=0x0028;
ReadMode_Cap=0x002C;
WorkMode_Cap=0x0030;
LaneNum_Cap=0x0034;
CameraModel0=0x0038;
CameraModel1=0x003C;
CameraModel2=0x0040;
CameraModel3=0x0044;
CameraModel4=0x0048;
CameraModel5=0x004C;
CameraModel6=0x0050;
CameraModel7=0x0054;
Temp_K=0x0058;
VideoModeCap=0x005C;
VideoModeNum=0x0060;
VidoeMode_WH1=0x0064;
VideoMode_Param1=0x0068;
VidoeMode_WH2=0x006C;
VideoMode_Param2=0x0070;
VidoeMode_WH3=0x0074;
VideoMode_Param3=0x0078;
VidoeMode_WH4=0x007C;
VideoMode_Param4=0x0080;
VidoeMode_WH5=0x0084;
VideoMode_Param5=0x0088;
VidoeMode_WH6=0x008C;
VideoMode_Param6=0x0090;
VidoeMode_WH7=0x0094;
VideoMode_Param7=0x0098;
VidoeMode_WH8=0x009C;
VideoMode_Param8=0x00A0;
#0x0400
Image_Acquisition=0x400;
Work_Mode=0x404;
Trigger_Source=0x408;
Trigger_Num=0x40C;
Trigger_Inerval=0x410;
Trigger_Software=0x414;
Trigger_Count=0x418;
I2C_Addr=0x41C;
Nondiscontinuous_mode=0x44C;
Slave_mode=0x460;
Sensor_Frame_Count=0x464;
Out_Frame_Count=0x468;
Trigger_Cycle_Min=0x46C;
Day_night_mode=0x474;
IRCUT_dir=0x478;
Day_night_Trigger_pin_polarity=0x47C;
IRCUT_Timer=0x480;

#0x0800
Test_Image_Selector=0x800;
Pixel_Format=0x804;
Sensor_Width=0x808;
Sensor_Height=0x80C;
MaxFrame_Rate=0x810;
ROI_Width=0x818;
ROI_Height=0x81C;
ROI_Offset_X=0x820;
VideoMode=0x82C;
ROI_Offset_Y=0x824;
Image_Direction=0x828;
ReadMode=0x838;
Lane_Num=0x83C;
MIPI_DataRate=0x840;
MIN_ROI_Width=0x844;
MIN_ROI_Height=0x848;
MinFrame_Rate=0x84C;
FrameRate_Ex=0x850;

#0x0c00

Exposure_Mode=0xC04;
Target_Brightness=0xC08;
AE_Strategy=0xC0C;
ME_Time=0xC10;
AE_MAX_Time=0xC14;
Exp_Time=0xC18;
Manual_Gain=0xC20;
AG_Max_Gain=0xC24;
Cur_Gain=0xC28;
WB_Mode=0xC3C;
AWB_Color_temp_min=0xC40;
AWB_Color_temp_max=0xC44;
MWB_Rgain=0xC48;
MWB_Bgain=0xC4C;
Color_Temp=0xC50;
Cur_Rgain=0xC54;
Cur_Bgain=0xC58;
AE_Min_Time=0xC5C;
Gamma=0xC60;
Anti_flicker_enable=0xC94;
Anti_flicker_Freq=0xC98;
WDR_Option=0xC9C;
WDR_strength=0xCA0;
Sharppen_strength=0xCA4;
Denoise_strength_2D=0xCA8;
Denoise_strength_3D=0xCAC;
Saturation=0xCB0;
Contrast=0xCB4;
Hue=0xCB8;
SlowShutter=0xCBC;
SlowShutter_GainTh=0xCC0;
LDC=0xCC4;
LSC=0xCC8;
Dehaze_strength=0xCCC;
Gamma_Selection=0xCD0;
DRC_strength=0xCD4;
#0x1000
Trigger_Delay=0x1000;
Trigger_Activation=0x1004;
Exposure_Delay=0x1010;
GPIO1_OutSelect=0x1020;
GPIO1_Reverse=0x1028;

# 全局默认总线（单相机模式用）
# I2C_DEV
I2C_DEV=10;

I2C_ADDR=0x3b;

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
I2C_VREAD="$SCRIPT_DIR/i2c_vread"
I2C_VWRITE="$SCRIPT_DIR/i2c_vwrite"

case "$1" in
    -r)
        if [[ ! -f "$I2C_VREAD" ]] || [[ ! -x "$I2C_VREAD" ]]; then
            echo "❌ Error: Required tool not found or not executable: $I2C_VREAD" >&2
            exit 1
        fi
        ;;
    -w)
        if [[ ! -f "$I2C_VWRITE" ]] || [[ ! -x "$I2C_VWRITE" ]]; then
            echo "❌ Error: Required tool not found or not executable: $I2C_VWRITE" >&2
            exit 1
        fi
        ;;
    *)
        ;;
esac


i2c_read() 
{
    local reg=$1
    ./i2c_vread $I2C_DEV $I2C_ADDR "$reg" 2>/dev/null | tail -n1
}

i2c_write() 
{
    local reg=$1
    local val=$2
    ./i2c_vwrite $I2C_DEV $I2C_ADDR "$reg" "$val"
}

read_manufacturer()
{
    local Manufacturer=0;
    typeset -i Manufacturer;
	Manufacturer=$(i2c_read $Manufacturer_Name);
    case $Manufacturer in
    "1447385413")
        #VEYE
        printf "Read Manufacturer_Name is VEYE\n";
    ;;
    *)
     printf "Manufacturer %8x not recognized\n" $Manufacturer;
     ;;
    esac
}

read_model() 
{
    local model=$(i2c_read $Model_Name)
    case $model in
        "1634")
            #IMX662
            MODEL_NAME="GX-MIPI-IMX662"
            export MODEL_NAME
            printf "Read Model_Name is GX-MIPI-IMX662\n";
            ;;
        *)
            printf "model 0x%08X not recognized\n" "$model"
            return 1
            ;;
    esac
    return 0
}

read_sensorname()
{
    local model=0;
    typeset -i model;
    #printf "************************************\n";
	model=$(i2c_read $Sensor_Name);
    case $model in
    "1634")
        #IMX662
        printf "Read Sensor_Name is IMX662-AAQR\n";
    ;;
    *)
     printf " model %8x not recognized\n" $model;
     ;;
    esac
    #printf "************************************\n";
}

read_version()
{
    local version=0;
    typeset -i version;
	version=$(i2c_read $Device_Version);
    printf "Read version is C %02x.%02x and L %02x.%02x\n" $(($version>>24&0xFF)) $(($version>>16 &0xFF)) $(($version>>8&0xFF)) $(($version&0xFF));
    # shellcheck disable=SC2004
    printf -v VERSION "C_%02x.%02x_L_%02x.%02x" $(($version>>24&0xFF)) $(($version>>16 &0xFF)) $(($version>>8&0xFF)) $(($version&0xFF));
    #printf $VERSION
}

write_factoryparam() 
{
    local value
    if [ -n "$1" ]; then
        value="$1"
    else
        
        value=1
    fi

    i2c_write $System_reset "$value"
    printf "factoryparam, all param will reset \n";
}

write_paramsave()
{
	i2c_write $Param_save_to_flash 1
    printf "all param saved to flash\n";
}

write_reboot()
{
	i2c_write $System_reboot 1
    printf "system reboot!\n";
}

read_serialno()
{
    local serialno=0;
    typeset -i serialno;
	serialno=$(i2c_read $Serial_Num);
    hex_val=$(printf "%08x" $(( serialno & 0xffffffff )))
    printf "Read Serial_Num is 0x%s\n" "$hex_val"
}

write_serialno()
{
    local res=$1;
	i2c_write $Serial_Num $res
    printf "Write Serial_Num is 0x%08x \n" "$res";
}


read_timestamp()
{
    local timestamp=0;
    typeset -i timestamp;
	timestamp=$(i2c_read $Time_stamp);
    printf "Read Time_stamp is %d \n" $timestamp;
}

read_errcode()
{
    local errcode=0;
    typeset -i errcode;
	errcode=$(i2c_read $Error_code);
    printf "Read Error_code is 0x%x \n" $errcode;
}


read_fmtcap()
{
    local fmtcap=0;
    typeset -i fmtcap;
	fmtcap=$(i2c_read $Format_Cap);
    printf "Read Format_Cap is 0x%x \n" $fmtcap;
}

read_videomodecap()
{
    local cap=0;
    typeset -i cap;
	cap=$(i2c_read $VideoModeCap);
    printf "Read VideoModeCap is 0x%x \n" $cap;
}

read_videomodenum()
{
    local num=0;
    typeset -i num;
	num=$(i2c_read $VideoModeNum);
    printf "Read VideoModeNum is 0x%x \n" $num;
}

read_videomodewh1()
{
    local wh1=0;
    typeset -i wh1;
	wh1=$(i2c_read $VidoeMode_WH1);
    printf "Read VidoeMode_WH1 is 0x%x \n" $wh1;
}

read_videomodeparam1()
{
    local param1=0;
    typeset -i param1;
	param1=$(i2c_read $VideoMode_Param1);
    printf "Read VideoMode_Param1 is 0x%x \n" $param1;
}

read_videomodewh2()
{
    local wh2=0;
    typeset -i wh2;
	wh2=$(i2c_read $VidoeMode_WH2);
    printf "Read VidoeMode_WH2 is 0x%x \n" $wh2;
}

read_videomodeparam2()
{
    local param2=0;
    typeset -i param2;
	param2=$(i2c_read $VideoMode_Param2);
    printf "Read VideoMode_Param2 is 0x%x \n" $param2;
}

read_videomodewh3()
{
    local wh3=0;
    typeset -i wh3;
	wh3=$(i2c_read $VidoeMode_WH3);
    printf "Read VidoeMode_WH3 is 0x%x \n" $wh3;
}

read_videomodeparam3()
{
    local param3=0;
    typeset -i param3;
	param3=$(i2c_read $VideoMode_Param3);
    printf "Read VideoMode_Param3 is 0x%x \n" $param3;
}

read_videomodewh4()
{
    local wh4=0;
    typeset -i wh4;
	wh4=$(i2c_read $VidoeMode_WH4);
    printf "Read VidoeMode_WH4 is 0x%x \n" $wh4;
}

read_videomodeparam4()
{
    local param4=0;
    typeset -i param4;
	param4=$(i2c_read $VideoMode_Param4);
    printf "Read VideoMode_Param4 is 0x%x \n" $param4;
}

read_videomodewh5()
{
    local wh5=0;
    typeset -i wh5;
	wh5=$(i2c_read $VidoeMode_WH5);
    printf "Read VidoeMode_WH5 is 0x%x \n" $wh5;
}

read_videomodeparam5()
{
    local param5=0;
    typeset -i param5;
	param5=$(i2c_read $VideoMode_Param5);
    printf "Read VideoMode_Param5 is 0x%x \n" $param5;
}

read_videomodewh6()
{
    local wh6=0;
    typeset -i wh6;
	wh6=$(i2c_read $VidoeMode_WH6);
    printf "Read VidoeMode_WH6 is 0x%x \n" $wh6;
}

read_videomodeparam6()
{
    local param6=0;
    typeset -i param6;
	param6=$(i2c_read $VideoMode_Param6);
    printf "Read VideoMode_Param6 is 0x%x \n" $param6;
}

read_videomodewh7()
{
    local wh7=0;
    typeset -i wh7;
	wh7=$(i2c_read $VidoeMode_WH7);
    printf "Read VidoeMode_WH7 is 0x%x \n" $wh7;
}

read_videomodeparam7()
{
    local param7=0;
    typeset -i param7;
	param7=$(i2c_read $VideoMode_Param7);
    printf "Read VideoMode_Param7 is 0x%x \n" $param7;
}

read_videomodewh8()
{
    local wh8=0;
    typeset -i wh8;
	wh8=$(i2c_read $VidoeMode_WH8);
    printf "Read VidoeMode_WH8 is 0x%x \n" $wh8;
}

read_videomodeparam8()
{
    local param8=0;
    typeset -i param8;
	param8=$(i2c_read $VideoMode_Param8);
    printf "Read VideoMode_Param8 is 0x%x \n" $param8;
}

write_imgacq()
{
    local acq=$1;
	i2c_write $Image_Acquisition "$acq"
    printf "Write Image_Acquisition is %d \n" "$acq";
}

read_workmode()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $Work_Mode);
    printf "Read Work_Mode is %d \n" $value;
}
write_workmode()
{
    local workmode=$1;
	i2c_write $Work_Mode "$workmode"
    printf "Write  Work_Mode is %d \n" "$workmode";
}

read_trgsrc()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $Trigger_Source);
    printf "Read Trigger_Source is %d \n" $value;
}

write_trgsrc()
{
    local trgsrc=$1;
	i2c_write $Trigger_Source "$trgsrc"
    printf "Write Trigger_Source is %d \n" "$trgsrc";
}

read_trgnum()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $Trigger_Num);
    printf "Read Trigger_Num is %d \n" $value;
}

write_trgnum()
{
    local trgnum=$1;
	i2c_write $Trigger_Num "$trgnum"
    printf "Write Trigger_Num is %d \n" "$trgnum";
}

read_trginterval()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $Trigger_Inerval);
    printf "Read Trigger_Inerval is %d us \n" $value;
}

write_trginterval()
{
    local trginterval=$1;
	i2c_write $Trigger_Inerval "$trginterval"
    printf "Write Trigger_Inerval is %d us \n" "$trginterval";
}

write_trgone()
{
	i2c_write $Trigger_Software 1
    printf "Write trigger one \n" ;
}

read_trgcount()
{
    local value=0;
    local trg_lost=0;
    local trg_count=0;
    typeset -i value;
    typeset -i trg_lost;
    typeset -i trg_count;
	value=$(i2c_read $Trigger_Count);
    trg_lost=$((value>>16));
    trg_count=$((value&0xFFFF));
    printf "Trigger stats: success=%d, lost=%d\n" $trg_count $trg_lost;
}
write_trgcount()
{
    local res=0;
	i2c_write  $Trigger_Count 1
    printf "Write clear trigger count\n";
}

read_i2caddr()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $I2C_Addr);
    printf "Read I2C_Addr is 0x%02x \n" $value;
}
write_i2caddr()
{
    local i2caddr=$1;
	i2c_write $I2C_Addr "$i2caddr"
    printf "Write I2C_Addr is 0x%02x \n" "$i2caddr";
}

read_nondiscontinuousmode()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $Nondiscontinuous_mode);
    printf "Read Nondiscontinuous_mode is %d \n" $value;
}

write_nondiscontinuousmode()
{
    local clkmode=$1;
	i2c_write $Nondiscontinuous_mode "$clkmode"
    printf "Write  Nondiscontinuous_mode is %d \n" "$clkmode";
}

read_slavemode()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $Slave_mode);
    printf "Read Slave_mode is %d \n" $value;
}

write_slavemode()
{
    local slmode=$1;
	i2c_write $Slave_mode "$slmode"
    printf "Write Slave_mode is %d \n" "$slmode";
}



read_framecount()
{
    local sns_count=0;
    local out_count=0;
    typeset -i sns_count;
    typeset -i out_count;
	sns_count=$(i2c_read $Sensor_Frame_Count);
    out_count=$(i2c_read $Out_Frame_Count);
    printf "Read Sensor_Frame_Count is %d , Out_Frame_Count is %d\n" $sns_count $out_count;
}

read_triggercyclemin()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $Trigger_Cycle_Min);
    printf "Read Trigger_Cycle_Min is %d \n" $value;
}

read_daynightmode()
{
    local value=0;
    typeset -i value;
    value=$(i2c_read $Day_night_mode);
    printf "Read Day_night_mode is 0x%x \n" $value;
}

write_daynightmode()
{
    local dnmode=$1;
    i2c_write $Day_night_mode "$dnmode"
    printf "Write Day_night_mode is 0x%x \n" "$dnmode";
}


read_ircutdir()
{
    local value=0;
    typeset -i value;
    value=$(i2c_read $IRCUT_dir);
    printf "Read IRCUT_dir  is 0x%x \n" $value;
}

write_ircutdir()
{
    local ircutdir=$1;
    i2c_write $IRCUT_dir "$ircutdir"
    printf "Write IRCUT_dir is 0x%x \n" "$ircutdir";
}

read_pinpolarity()
{
    local value=0;
    typeset -i value;
    value=$(i2c_read $Day_night_Trigger_pin_polarity);
    printf "Read Day_night_Trigger_pin_polarity  is 0x%x \n" $value;
}

write_pinpolarity()
{
    local polarity=$1;
    i2c_write $Day_night_Trigger_pin_polarity "$polarity"
    printf "Write Day_night_Trigger_pin_polarity is 0x%x \n" "$polarity";
}

read_ircuttimer()
{
    local value=0;
    typeset -i value;
    value=$(i2c_read $IRCUT_Timer);
    printf "Read IRCUT_Timer  is 0x%x \n" $value;
}

write_ircuttimer()
{
    local ircuttimer=$1;
    i2c_write $IRCUT_Timer "$ircuttimer"
    printf "Write IRCUT_Timer is 0x%x \n" "$ircuttimer";
}


read_testimg()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $Test_Image_Selector);
    printf "Read Test_Image_Selector is %d \n" $value;
}
write_testimg()
{
    local testimg=$1;
	i2c_write $Test_Image_Selector "$testimg"
    printf "Write Test_Image_Selector is %d \n" "$testimg";
}

read_pixelformat()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $Pixel_Format);
    printf "Read Pixel_Format is %d \n" $value;
}
write_pixelformat()
{
    local piformat=$1;
	i2c_write $Pixel_Format "$piformat"
    printf "Write Pixel_Format is %d \n" "$piformat";
}

read_maxwh()
{
    local width=0;
    local height=0;
    typeset -i width;
    typeset -i height;
    width=$(i2c_read $Sensor_Width);
    height=$(i2c_read $Sensor_Height);
    printf "Read Sensor_Width is %d Sensor_Height is %d \n" $width $height;
}

read_maxfps()
{
    local value=0;
    local maxfps=0;
	value=$(i2c_read $MaxFrame_Rate);
    maxfps=$(awk -v x="$value" 'BEGIN {printf "%.2f\n",x/100}')
    printf "Read MaxFrame_Rate is %.2f fps\n" "$maxfps";
}


read_curwh()
{
   # local x=0;
   # local y=0;
    local width=0;
    local height=0;
  #  typeset -i x;
  #  typeset -i y;
    typeset -i width;
    typeset -i height;
    
  #  x=$(i2c_read $ROI_Offset_X);
  #  y=$(i2c_read $ROI_Offset_Y);
    width=$(i2c_read $ROI_Width);
    height=$(i2c_read $ROI_Height);
    printf "ROI_Width is %d,ROI_Height is %d \n" $width $height;
   # printf "Read ROI_Offset_X is %d, ROI_Offset_Y is %d, ROI_Width is %d,ROI_Height is %d \n" $x $y $width $height;
}

read_imgdir()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $Image_Direction);
    printf "Read Image_Direction is %d \n" $value;
}
write_imgdir()
{
    local imgdir=$1;
	i2c_write $Image_Direction "$imgdir"
    printf "Write Image_Direction is %d \n" "$imgdir";
}

read_videomode()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $VideoMode);
    printf "Read VideoMode is %d \n" $value;
}

write_videomode()
{
    local value=$1;
    i2c_write $VideoMode "$value"
    printf "Write VideoMode is %d \n" "$value";
}


read_readmode()
{
    local value=0;
    typeset -i value;
    value=$(i2c_read $ReadMode);
    printf "Read ReadMode is 0x%x \n" $value;
}

#write_readmode()
#{
 #   local readmode=$1;
 #   i2c_write $ReadMode $readmode
 #   printf "Write ReadMode is 0x%x \n" $readmode;
#}

read_lanenum()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $Lane_Num);
    printf "Read Lane_Num is %d \n" $value;
}

read_mipidatarate()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $MIPI_DataRate);
    printf "Read MIPI_DataRate is %d kbps \n" $value;
}

read_minwh()
{
    local width=0;
    local height=0;
    typeset -i width;
    typeset -i height;
    width=$(i2c_read $MIN_ROI_Width);
    height=$(i2c_read $MIN_ROI_Height);
    printf "Read MIN_ROI_Width is %d MIN_ROI_Height is %d\n" $width $height;
}

read_minfps()
{
    local minframe=0
    minframe=$(i2c_read $MinFrame_Rate)
    local result
    result=$(echo "scale=3; $minframe / 10000" | bc -l)

    printf "Read MinFrame_Rate is %.3f fps\n" "$result"
}

read_fps()
{
    local value=0;
    local fps=0;
    typeset -i value;
	value=$(i2c_read $FrameRate_Ex);
    fps=$(awk -v x="$value" 'BEGIN {printf "%.4f\n",x/10000}')
    printf "Read FrameRate_Ex is %.04f fps\n" "$fps";
}
write_fps()
{
    local fps=$1;
    local reg_value=0;
    reg_value=$(awk -v x="$fps" 'BEGIN {printf ("%.0f\n",x*10000)}')
	i2c_write $FrameRate_Ex "$reg_value"
    printf "Write FrameRate_Ex is %.04f fps \n" "$fps";
}

read_expmode()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $Exposure_Mode);
    printf "Read Exposure_Mode is %d \n" $value;
}
write_expmode()
{
    local expmode=$1;
	i2c_write $Exposure_Mode "$expmode"
    printf "Write Exposure_Mode is %d \n" "$expmode";
}

read_aetarget()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $Target_Brightness);
    printf "Read Target_Brightness is %d \n" $value;
}
write_aetarget()
{
    local aetarget=$1;
	i2c_write $Target_Brightness "$aetarget"
    printf "Write Target_Brightness is %d \n" "$aetarget";
}

read_aestrategy()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $AE_Strategy);
    printf "Read  AE_Strategy is %d  \n" $value;
}

write_aestrategy()
{
    local aestrategy=$1;
	i2c_write $AE_Strategy "$aestrategy"
    printf "Write  AE_Strategy is %d  \n" "$aestrategy";
}

read_metime()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $ME_Time);
    printf "Read ME_Time time is %d us \n" $value;
}

write_metime()
{
    local metime=$1;
	i2c_write $ME_Time "$metime"
    printf "Write manual exp time is %d us \n" "$metime";
}

read_aemaxtime()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $AE_MAX_Time);
    printf "Read AE_MAX_Time is %.3f us\n" $value;
}
write_aemaxtime()
{
    local aemaxtime=$1;
	i2c_write $AE_MAX_Time "$aemaxtime"
    printf "Write AE_MAX_Time is %.3f us\n" "$aemaxtime";
}

read_exptime()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $Exp_Time);
    printf "Read Exp_Time is %d us\n" $value;
}

read_mgain()
{
    local value=0;
    local gain=0;
	value=$(i2c_read $Manual_Gain);
    gain=$(awk -v x="$value" 'BEGIN {printf "%.1f\n",x/10}')
    printf "Read Manual_Gain is %.1f dB \n" "$gain";
}

write_mgain()
{
    local mgain=$1;
    local reg_value=0;
    reg_value=$(awk -v x="$mgain" 'BEGIN {printf ("%d\n",x*10)}')
	i2c_write $Manual_Gain "$reg_value"
    printf "Write mgain is %.01f dB (reg: %d)\n" "$mgain" "$reg_value"
}

read_aemaxgain()
{
    local value=0;
    local gain=0;
	value=$(i2c_read $AG_Max_Gain);
    gain=$(awk -v x="$value" 'BEGIN {printf "%.1f\n",x/10}')
    printf "Read AG_Max_Gain is %.1f dB \n" "$gain";
}

write_aemaxgain()
{
    local aemaxgain=$1;
    local reg_value=0;
    reg_value=$(awk -v x="$aemaxgain" 'BEGIN {printf ("%d\n",x*10)}')
	i2c_write $AG_Max_Gain "$reg_value"
    printf "Write AG_Max_Gain is %.1f dB (reg: %d)\n" "$aemaxgain" "$reg_value"
}

read_curgain()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $Cur_Gain);
    gain=$(awk -v x="$value" 'BEGIN {printf "%.1f\n",x/10}')
    printf "Read Cur_Gain is %.1f dB \n" "$gain";
    
}

read_wbmode()
{
    local wbmode=0;
    typeset -i wbmode;
    wbmode=$(i2c_read $WB_Mode);
    printf "Read WB_Mode is %d  \n" $wbmode;
}

write_wbmode()
{
    local wbmode=$1;
    i2c_write $WB_Mode "$wbmode"
    printf "Write  WB_Mode is %d  \n" "$wbmode";
}

read_awbcolortempmin()
{
    local awb_color_tempmin=0;
    typeset -i awb_color_tempmin;
    awb_color_tempmin=$(i2c_read $AWB_Color_temp_min);
    printf "Read  AWB_Color_temp_min is %d  \n" $awb_color_tempmin;
}

write_awbcolortempmin()
{
    local value=$1;
    i2c_write $AWB_Color_temp_min "$value"
    printf "Write  AWB_Color_temp_min is %d  \n" "$value";
}

read_awbcolortempmax()
{
    local awb_color_tempmax=0;
    typeset -i awb_color_tempmax;
    awb_color_tempmax=$(i2c_read $AWB_Color_temp_max);
    printf "Read  AWB_Color_temp_max is %d  \n" $awb_color_tempmax;

}

write_awbcolortempmax()
{
    local value=$1;
    i2c_write $AWB_Color_temp_max "$value"
    printf "Write  AWB_Color_temp_max is %d  \n" "$value";

}

read_mwbrgain()
{
    local mwb_rgain=0;
    typeset -i mwb_rgain;
    mwb_rgain=$(i2c_read $MWB_Rgain);
    printf "Read MWB_Rgain is %f \n" $mwb_rgain;

}

write_mwbrgain()
{
    local value=$1;
    i2c_write $MWB_Rgain "$value"
    printf "Write  MWB_Rgain is %d  \n" "$value";

}

read_mwbbgain()
{
    local mwb_bgain=0;
    typeset -i mwb_bgain;
    mwb_bgain=$(i2c_read $MWB_Bgain);
    printf "Read MWB_Bgain is %f  \n" $mwb_bgain;

}

write_mwbbgain()
{
    local value=$1;
    i2c_write $MWB_Bgain "$value";
    printf "Write  MWB_Bgain is %d  \n" "$value";

}

read_colortemp()
{
    local color_temp=0;
    typeset -i color_temp;
    color_temp=$(i2c_read $Color_Temp);
    printf "Read  Color_Temp is %d k \n" $color_temp;

}

read_currgain()
{
    local cur_rgain=0;
    typeset -i cur_rgain;
    cur_rgain=$(i2c_read $Cur_Rgain);
    printf "Read Cur_Rgain is %d \n" $cur_rgain;
}

read_curbgain()
{
    local cur_bgain=0;
    typeset -i cur_bgain;
    cur_bgain=$(i2c_read $Cur_Bgain);
    printf "Read Cur_Bgain is %d \n" $cur_bgain;
}

read_aemintime()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $AE_Min_Time);
    printf "Read AE_Min_Time is %d us\n" $value;
}
write_aemintime()
{
    local value=$1;
	i2c_write $AE_Min_Time "$value"
    printf "Write AE_Min_Time is %d us\n" "$value";
}

read_gamma_index()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $Gamma_Selection);
    #gamma=$(awk -v x="$value" 'BEGIN {printf "%.2f\n",x/100}')
    printf "Read Gamma_Selection is %d \n" $value;
}

write_gamma_index()
{
    local gamma_index=$1;
    #reg_val=$(awk -v x="$gamma" 'BEGIN {printf ("%.0f\n",x*100)}')
	i2c_write $Gamma_Selection "$gamma_index"
    printf "Write Gamma_Selection %.d  \n" "$gamma_index";
}


write_antiflicker()
{
    local enable=$1;
    local freq=$2;
    i2c_write $Anti_flicker_enable "$enable"
    i2c_write $Anti_flicker_Freq "$freq"
    printf "Write  Anti_flicker_enable is %d Anti_flicker_Freq is %d \n" "$enable" "$freq";
}

read_antiflicker()
{
    local enable=0;
    local freq=0;
    enable=$(i2c_read $Anti_flicker_enable);
    freq=$(i2c_read $Anti_flicker_Freq);
    printf "Read  Anti_flicker_enable is %d  Anti_flicker_Freq is %d  \n" "$enable" "$freq" ;
}

write_slowshutter()
{
    local enable=$1;
    local gainth=$2;
    local gainth_scaled
    gainth_scaled=$(awk "BEGIN {printf \"%.0f\", $gainth * 10}")
    i2c_write $SlowShutter "$enable"
    i2c_write $SlowShutter_GainTh "$gainth_scaled"
    printf "Write  SlowShutter enable is %d SlowShutter_GainTh is %.1f dB\n" "$enable" "$gainth";
}

read_slowshutter()
{
    local enable=0
    local gainth_raw=0
    local gainth=0
    enable=$(i2c_read $SlowShutter)
    gainth_raw=$(i2c_read $SlowShutter_GainTh)
    gainth=$(awk "BEGIN {printf \"%.1f\", $gainth_raw / 10.0}")
    printf "Read  SlowShutter enable is %d  SlowShutter_GainTh is %.1f dB\n" "$enable" "$gainth" ;
}

#WDR_Option/WDR_strength
write_wdrparam()
{
    local enable=$1;
    local strength=$2;
    i2c_write $WDR_Option "$enable"
    i2c_write $WDR_strength "$strength"
    printf "Write  WDR_Option enable is %d  WDR_strength is %d \n" "$enable" "$strength";
}

read_wdrparam()
{
    local enable=0;
    local strength=0;
    enable=$(i2c_read $WDR_Option);
    strength=$(i2c_read $WDR_strength);    
    printf "Read  WDR_Option enable is %d WDR_strength is %d  \n" "$enable" "$strength" ;
}

#Sharppen_strength
write_sharppen()
{
    local sharpen=$1;
    i2c_write $Sharppen_strength "$sharpen"
    printf "Write  Sharppen_strength is %d   \n" "$sharpen";
}

read_sharppen()
{
    local sharpen=0;
    sharpen=$(i2c_read $Sharppen_strength);
    printf "Read  Sharppen_strength is %d   \n" "$sharpen";
}

#Denoise_strength_2D
write_denoise2d()
{
    local str2D=$1;
    i2c_write $Denoise_strength_2D "$str2D"
    printf "Write Denoise_strength_2D is  %d   \n" "$str2D";
}

read_denoise2d()
{
    local res=0;
    res=$(i2c_read $Denoise_strength_2D);
    printf "Read Denoise_strength_2D is  %d   \n" "$res";
}

#Denoise_strength_3D
write_denoise3d()
{
    local str3D=$1;
    i2c_write $Denoise_strength_3D "$str3D"
    printf "Write  Denoise_strength_3D is %d   \n" "$str3D";
}

read_denoise3d()
{
    local res=0;
    res=$(i2c_read $Denoise_strength_3D);
    printf "Read  Denoise_strength_3D  is %d   \n" "$res";
}
#Saturation ,Contrast, Hue

write_saturation()
{
    local sat=$1;
    i2c_write $Saturation "$sat"
    printf "Write  Satur is %d  \n" "$sat";
}
read_saturation()
{
    local Satur=0;
    Satur=$(i2c_read $Saturation);
    printf "Read  Satur is %d \n" "$Satur";
}

write_contrast()
{
    local cont=$1;
    i2c_write $Contrast "$cont"
    printf "Write Contrast is %d  \n" "$cont";
}
read_contrast()
{
    local Cont=0;
    Cont=$(i2c_read $Contrast);
    printf "Read  Contrast  is %d \n" "$Cont";
}

write_hue()
{
    local hu=$1;
    i2c_write $Hue "$hu"
    printf "Write  Hue is %d \n" "$hu";
}
read_hue()
{
    local Hu=0;
    Hu=$(i2c_read $Hue);
    printf "Read  Hue  is %d \n" "$Hu";
}

read_workmodecap()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $WorkMode_Cap);
    printf "Read WorkMode_Cap is 0x%x \n" $value;
}

read_lanecap()
{
    local lanecap=0;
    typeset -i lanecap;
	lanecap=$(i2c_read $LaneNum_Cap);
    printf "Read LaneNum_Cap is 0x%x \n" $lanecap;
}

read_cameramodel0()
{
    local cameramodel=0;
    typeset -i cameramodel;
    cameramodel=$(i2c_read $CameraModel0);
    printf "Read CameraModel0 is 0x%x\n" $cameramodel;
}

read_cameramodel1()
{
    local cameramodel=0;
    typeset -i cameramodel;
    cameramodel=$(i2c_read $CameraModel1);
    printf "Read CameraModel1 is 0x%x \n" $cameramodel;
}

read_cameramodel2()
{
    local cameramodel=0;
    typeset -i cameramodel;
    cameramodel=$(i2c_read $CameraModel2);
    printf "Read CameraModel2 is 0x%x \n" $cameramodel;
}

read_cameramodel3()
{
    local cameramodel=0;
    typeset -i cameramodel;
    cameramodel=$(i2c_read $CameraModel3);
    printf "Read CameraModel3 is 0x%x \n" $cameramodel;
}

read_cameramodel4()
{
    local cameramodel=0;
    typeset -i cameramodel;
    cameramodel=$(i2c_read $CameraModel4);
    printf "Read CameraModel4 is 0x%x \n" $cameramodel;
}

read_cameramodel5()
{
    local cameramodel=0;
    typeset -i cameramodel;
    cameramodel=$(i2c_read $CameraModel5);
    printf "Read CameraModel5 is 0x%x \n" $cameramodel;
}

read_cameramodel6()
{
    local cameramodel=0;
    typeset -i cameramodel;
    cameramodel=$(i2c_read $CameraModel6);
    printf "Read CameraModel6 is 0x%x \n" $cameramodel;
}

read_cameramodel7()
{
    local cameramodel=0;
    typeset -i cameramodel;
    cameramodel=$(i2c_read $CameraModel7);
    printf "Read CameraModel7 is 0x%x \n" $cameramodel;
}

#read_trgcycle()
#{
 #   local cycle_min=0;
  #  local cycle_max=0;
 #   typeset -i cycle_min;
 #   typeset -i cycle_max;
#	cycle_min=$(i2c_read $Trigger_Cycle_Min);
 #   cycle_max=$(i2c_read $Trigger_Cycle_Max);
 #   printf "Read Trigger_Cycle_Min is %d us,Trigger_Cycle_Max is %d us\n" $cycle_min $cycle_max;
#}

read_temp()
{
    local value=0
    local kelvin=0
    local celsius=0

    # Read temperature value, unit is 100 times Kelvin
    value=$(i2c_read $Temp_K);

    # Calculate the actual Kelvin temperature
    kelvin=$(echo "scale=2; $value / 100" | bc);

    # Calculate Celsius temperature
    celsius=$(echo "scale=2; $kelvin - 273.15" | bc);

    # Print temperature values
    printf "Read temperature is %.2f K (%.2f \u2103)\n" "$kelvin" "$celsius"
}

read_readmodecap()
{
    local value=0;
    typeset -i value;
    value=$(i2c_read $ReadMode_Cap);
    printf "Read ReadMode_Cap is 0x%x \n" $value;
}

read_trgedge()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $Trigger_Activation);
    printf "Read Trigger_Activation is %d \n" $value;
}
write_trgedge()
{
    local res=$1;
	i2c_write $Trigger_Activation "$res"
    printf "Write Trigger_Activation is %d \n" "$res";
}

read_trgdelay()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $Trigger_Delay);
    printf "Read Trigger_Delay is %d us\n" $value;
}
write_trgdelay()
{
    local res=$1;
	i2c_write $Trigger_Delay "$res"
    printf "Write Trigger_Delay is %d us\n" "$res";
}

read_trgexp_delay()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $Exposure_Delay);
    printf "Read Exposure_Delay is %d us\n" $value;
}
write_trgexp_delay()
{
    local res=$1;
	i2c_write $Exposure_Delay "$res"
    printf "Write Exposure_Delay is %d us\n" "$res";
}

read_outio1_mode()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $GPIO1_OutSelect);
    printf "Read GPIO1_OutSelect %d \n" $value;
}
write_outio1_mode()
{
    local res=$1;
	i2c_write $GPIO1_OutSelect "$res"
    printf "Write GPIO1_OutSelect is %d \n" "$res";
}

read_outio1_rvs()
{
    local value=0;
    typeset -i value;
	value=$(i2c_read $GPIO1_Reverse);
    printf "Read GPIO1_Reverse %d \n" $value;
}
write_outio1_rvs()
{
    local res=$1;
	i2c_write $GPIO1_Reverse "$res"
    printf "Write GPIO1_Reverse is %d \n" "$res";
}

write_ldc()
{
    local ldc=$1;
    i2c_write $LDC "$ldc"
    printf "Write  ldc is %d   \n" "$ldc";
}

read_ldc()
{
    local ldc=0;
    ldc=$(i2c_read $LDC);
    printf "Read  LDC is %d   \n" "$ldc";
}

write_lsc()
{
    local lsc=$1;
    i2c_write $LSC "$lsc"
    printf "Write  lsc is %d   \n" "$lsc";
}

read_lsc()
{
    local lsc=0;
    lsc=$(i2c_read $LSC);
    printf "Read  LSC is %d   \n" "$lsc";
}

#Dehaze_strength
write_dehaze()
{
    local strength=$1;
    i2c_write $Dehaze_strength "$strength"
    printf "Write  Dehaze_strength is %d \n" "$strength";
}

read_dehaze()
{
    local strength=0;
    strength=$(i2c_read $Dehaze_strength);    
    printf "Read  Dehaze_strength is %d  \n" "$strength" ;
}

write_drc()
{
    local drc=$1;
    i2c_write $DRC_strength "$drc"
    printf "Write  drc is %d   \n" "$drc";
}

read_drc()
{
    local drc=0;
    drc=$(i2c_read $DRC_strength);
    printf "Read  DRC_strength is %d   \n" "$drc";
}



