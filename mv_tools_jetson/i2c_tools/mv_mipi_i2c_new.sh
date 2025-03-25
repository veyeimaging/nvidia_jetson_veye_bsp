#!/bin/bash
#
#This script is a simplified version of mv_mipi_i2c.sh, in which -f -p1 -p2 -p3 -p4 is no longer needed.
<<'COMMENT_SAMPLE'
./mv_mipi_i2c_new.sh -r  manufacturer
./mv_mipi_i2c_new.sh -r  model
./mv_mipi_i2c_new.sh -r  version
./mv_mipi_i2c_new.sh -r  serialno
./mv_mipi_i2c_new.sh -r  timestamp
./mv_mipi_i2c_new.sh -r  errcode
./mv_mipi_i2c_new.sh -r  fmtcap

./mv_mipi_i2c_new.sh -w  factoryparam
./mv_mipi_i2c_new.sh -w  paramsave
./mv_mipi_i2c_new.sh -w  reboot

./mv_mipi_i2c_new.sh -w  i2caddr  0x3c
./mv_mipi_i2c_new.sh -r  i2caddr 

./mv_mipi_i2c_new.sh -w  slavemode [param1]
./mv_mipi_i2c_new.sh -r  slavemode 

## =======================================
## image properties

#start/stop image acq
./mv_mipi_i2c_new.sh -w  imgacq [param1]

./mv_mipi_i2c_new.sh -r  maxwh

./mv_mipi_i2c_new.sh -r  maxfps

0 normal，
1 mirror，
2 flip，
3 flip&mirror
./mv_mipi_i2c_new.sh -w  imgdir  [0,3]
./mv_mipi_i2c_new.sh -r  imgdir

0 raw8
1 raw10
2 raw12
3 raw14
4 UYVY
./mv_mipi_i2c_new.sh -w  pixelformat  [0,2]
./mv_mipi_i2c_new.sh -r  pixelformat

./mv_mipi_i2c_new.sh -w  blacklevel [param1]
./mv_mipi_i2c_new.sh -r  blacklevel

./mv_mipi_i2c_new.sh -w  roi  [x]  [y]  [width]  [height]
./mv_mipi_i2c_new.sh -r  roi

1 宽竖条纹
2 细密竖条纹
./mv_mipi_i2c_new.sh -w  testimg [param1]
./mv_mipi_i2c_new.sh -r  testimg

./mv_mipi_i2c_new.sh -w  fps [param1]
./mv_mipi_i2c_new.sh -r  fps

./mv_mipi_i2c_new.sh -r  clkmode

## ======================================================
## ISP properties
./mv_mipi_i2c_new.sh -r  gamma
./mv_mipi_i2c_new.sh -w  gamma [param1]

./mv_mipi_i2c_new.sh -r  gammaenable
./mv_mipi_i2c_new.sh -w  gammaenable [param1]

./mv_mipi_i2c_new.sh -r  dpcenable
./mv_mipi_i2c_new.sh -w  dpcenable [param1]

./mv_mipi_i2c_new.sh -r  lutenable
./mv_mipi_i2c_new.sh -w  lutenable [param1]

./mv_mipi_i2c_new.sh -r  lut  lut.txt
./mv_mipi_i2c_new.sh -w  lut  lut.txt

0 manual exposure
1 auto exp once
2 auto exp continuous
./mv_mipi_i2c_new.sh -r  expmode
./mv_mipi_i2c_new.sh -w  expmode [param1]

0 manual gain
1 auto gain once
2 auto gain continuous
./mv_mipi_i2c_new.sh -r  gainmode
./mv_mipi_i2c_new.sh -w  gainmode [param1]

# read back to confirm the real metime take effect
./mv_mipi_i2c_new.sh -w  metime [us]
./mv_mipi_i2c_new.sh -r  metime

# read back to confirm the real mgain take effect
./mv_mipi_i2c_new.sh -w  mgain  [gain]
./mv_mipi_i2c_new.sh -r  mgain

./mv_mipi_i2c_new.sh -r  aatarget
./mv_mipi_i2c_new.sh -w  aatarget [1,255]

./mv_mipi_i2c_new.sh -r  aemaxtime
./mv_mipi_i2c_new.sh -w  aemaxtime [param1]

./mv_mipi_i2c_new.sh -r  exptime
./mv_mipi_i2c_new.sh -r  curgain

./mv_mipi_i2c_new.sh -r  agmaxgain
./mv_mipi_i2c_new.sh -w  agmaxgain [param1]

./mv_mipi_i2c_new.sh -w  aaroi  [x] [y] [width] [height]
./mv_mipi_i2c_new.sh -r  aaroi

./mv_mipi_i2c_new.sh -w  aaroienable [0,1]
./mv_mipi_i2c_new.sh -r  aaroienable

#run ae ag once and save as manual value
./mv_mipi_i2c_new.sh -w  aeag_run_once_save

### ======================
### trigger properties 

0 continuous
1 trigger mode
2 high speed trigger mode
./mv_mipi_i2c_new.sh -w  trgmode [param1]
./mv_mipi_i2c_new.sh -r  trgmode

0 software trigger
1 hardware trigger
./mv_mipi_i2c_new.sh -w  trgsrc [param1]
./mv_mipi_i2c_new.sh -r  trgsrc

./mv_mipi_i2c_new.sh -w  trgnum [param1]
./mv_mipi_i2c_new.sh -r  trgnum

./mv_mipi_i2c_new.sh -w  trginterval [param1]
./mv_mipi_i2c_new.sh -r  trginterval

./mv_mipi_i2c_new.sh -w  trgone

./mv_mipi_i2c_new.sh -r  trgcount
./mv_mipi_i2c_new.sh -w  trgcountclr

./mv_mipi_i2c_new.sh -r  trgdelay
./mv_mipi_i2c_new.sh -w  trgdelay [param1]

0 Rising edge
1 Falling edge
./mv_mipi_i2c_new.sh -r  trgedge
./mv_mipi_i2c_new.sh -w  trgedge [param1]

./mv_mipi_i2c_new.sh -r  trgfilter_enable
./mv_mipi_i2c_new.sh -w  trgfilter_enable [param1]

./mv_mipi_i2c_new.sh -r  trgfilter_time
./mv_mipi_i2c_new.sh -w  trgfilter_time [param1]

./mv_mipi_i2c_new.sh -r  trgexp_delay
./mv_mipi_i2c_new.sh -w  trgexp_delay [param1]

./mv_mipi_i2c_new.sh -r  gpios_status

0 strobe 
1 userdef
./mv_mipi_i2c_new.sh -r  outio1_mode
./mv_mipi_i2c_new.sh -w  outio1_mode [param1]

0 low
1 high
./mv_mipi_i2c_new.sh -r  outio1_usr
./mv_mipi_i2c_new.sh -w  outio1_usr [param1]

./mv_mipi_i2c_new.sh -r  outio1_rvs
./mv_mipi_i2c_new.sh -w  outio1_rvs [param1]

0 strobe 
1 userdef
2 trigger wait
3 XVS
4 XHS
./mv_mipi_i2c_new.sh -r  outio2_mode
./mv_mipi_i2c_new.sh -w  outio2_mode [param1]

0 low
1 high
./mv_mipi_i2c_new.sh -r  outio2_usr
./mv_mipi_i2c_new.sh -w  outio2_usr [param1]

./mv_mipi_i2c_new.sh -r  outio2_rvs
./mv_mipi_i2c_new.sh -w  outio2_rvs [param1]

./mv_mipi_i2c_new.sh -r  lanenum
./mv_mipi_i2c_new.sh -w  lanenum  [2/4]
### Special function
./mv_mipi_i2c_new.sh -r  snsreg  [SensorAddr]

./mv_mipi_i2c_new.sh -r framecount

./mv_mipi_i2c_new.sh -r trgmodecap

./mv_mipi_i2c_new.sh -r  minwh
./mv_mipi_i2c_new.sh -r  model

./mv_mipi_i2c_new.sh -r temp

./mv_mipi_i2c_new.sh -r exptime_range

./mv_mipi_i2c_new.sh -r osdmode
./mv_mipi_i2c_new.sh -w osdmode [0/1]

./mv_mipi_i2c_new.sh -r readmodecap

./mv_mipi_i2c_new.sh -r readmode
./mv_mipi_i2c_new.sh -w readmode [0/1/2]

COMMENT_SAMPLE

#
#set camera i2c pin mux
#

I2C_DEV=7;
I2C_ADDR=0x3b;

print_usage()
{
	echo "Usage:  ./mv_mipi_i2c_new.sh [-r/w]  [function name] [param1] [param2 ] [param3] [param4] -b bus"
	echo "options:"
	echo "    -r                       read "
	echo "    -w                       write"
	echo "    [function name]       function name"
	echo "    [param1] 			    param1 of each function"
	echo "    [param1] 			    param2 of each function"
    echo "    [param3] 			    param3 of each function"
    echo "    [param4] 			    param4 of each function"
	echo "    -b [i2c bus num] 		   i2c bus number"
	echo "    -d [i2c addr] 		   i2c addr if not default 0x3b"
	echo "Please open this srcipt and read the COMMENT on top for support functions and samples"
}
######################reglist###################################
BaseCommon=0x0000;
ImageAcqu=0x0400;
ImageFeature=0x0800;
ImageSigProc=0x0c00;
IOCtrl=0x1000;
FPGACmd=0x1400;
FPGABlock=0x1800;

Manufacturer_Name=0x0000;
Model_Name=0x0004;
Sensor_Name=0x0008;
Product_Info=0x000C;
Device_Version=0x0010;
System_reset=0x0014;
Param_save_to_flash=0x0018;
System_reboot=0x001C;
Time_stamp=0x0020;
Error_code=0x0024;
Format_Cap=0x0028;
ReadMode_Cap=0x002C;
TriggerMode_Cap=0x0030;
LaneNum_Cap=0x0034;
Temp_K=0x0058;


Image_Acquisition=0x400;
Trigger_Mode=0x404;
Trigger_Source=0x408;
Trigger_Num=0x40C;
Trigger_Inerval=0x410;
Trigger_Software=0x414;
Trigger_Count=0x418;
I2C_Addr=0x41C;
I2C_Port_Sel=0x420;

User_overlay_enable=0x428;
User_overlay_zone0=0x42C;
User_overlay_zone1=0x430;
User_overlay_zone2=0x434;
User_overlay_zone3=0x438;
User_define_zone0=0x43C;
User_define_zone1=0x440;
User_define_zone2=0x444;
User_define_zone3=0x448;
Nondiscontinuous_mode=0x44C;
Sensor_Reg_Addr=0x0450;
Sensor_Reg_Val=0x454;
Slave_mode=0x460;
Sensor_Frame_Count=0x464;
Out_Frame_Count=0x468;
Trigger_Cycle_Min=0x46C;
Trigger_Cycle_Max=0x470;

Test_Image_Selector=0x800;
Pixel_Format=0x804;
Sensor_Width=0x808;
Sensor_Height=0x80C;
MaxFrame_Rate=0x810;
Framerate=0x814;
ROI_Width=0x818;
ROI_Height=0x81C;
ROI_Offset_X=0x820;
ROI_Offset_Y=0x824;
Image_Direction=0x828;
Data_shift=0x82C;
Black_Level=0x830;
BLC_Mode=0x834;
ReadOut_Mode=0x838;
Lane_Num=0x83C;
MIPI_DataRate=0x840;
MIN_ROI_Width=0x844;
MIN_ROI_Height=0x848;
Min_Frame_Rate=0x84C;
FrameRate_Ex=0x850;
OSD_Mode=0x854;

ISP_module_ctrl=0xC00;
Exposure_Mode=0xC04;
Target_Brightness=0xC08;
Exposure_Time_Source=0xC0C;
ME_Time=0xC10;
AE_MAX_Time=0xC14;
Exp_Time=0xC18;
Gain_Mode=0xC1C;
Manual_Gain=0xC20;
AG_Max_Gain=0xC24;
Cur_Gain=0xC28;
AAROIOffsetX=0xC2C;
AAROIOffsetY=0xC30;
AAROIWidth=0xC34;
AAROIHeight=0xC38;
WB_Mode=0xC3C;
MWB_Rgain=0xC40;
MWB_Bgain=0xC44;
AWBROIOffsetX=0xC48;
AWBROIOffsetY=0xC4C;
AWBROIWidth=0xC50;
AWBROIHeight=0xC54;
AWB_Rgain=0xC58;
AWB_Bgain=0xC5C;
Gamma=0xC60;
DPC_Start=0xC64;
DPC_Status=0xC68;
DPC_Count=0xC6C;
AAROI_enable=0xC80;
Max_Exp_time=0xC8C;
Min_Exp_time=0xC90;

Trigger_Delay=0x1000;
Trigger_Activation=0x1004;
Trigger_Filter_Enable=0x1008;
Trigger_Filter_Width=0x100C;
Trigger_Exp_Delay=0x1010;
GPIOS_Status=0x1014;

GPIO1_OutSelect=0x1020;
GPIO1_Useroutput=0x1024;
GPIO1_Reverse=0x1028;


GPIO2_OutSelect=0x1030;
GPIO2_Useroutput=0x1034;
GPIO2_Reverse=0x1038;



######################parse arg###################################

MODE="";
FUNCTION="";
PARAMS=();
b_arg_bus=0;
b_arg_addr=0;

for arg in $@
do
    if [ $b_arg_bus -eq 1 ] ; then
        b_arg_bus=0;
        I2C_DEV=$arg;
        continue;
    fi
    if [ $b_arg_addr -eq 1 ] ; then
        b_arg_addr=0;
        I2C_ADDR=$arg;
        continue;
    fi
    case $arg in
        "-r")
            MODE=read;
            ;;
        "-w")
            MODE=write;
            ;;
        "-b")
            b_arg_bus=1;
            ;;
        "-d")
            b_arg_addr=1;
            ;;
        "-h")
            print_usage;
            ;;
        *)
            PARAMS+=($arg);
            ;;
    esac
done

if [ ${#PARAMS[@]} -gt 0 ]; then
    FUNCTION=${PARAMS[0]};
fi

if [ -z $MODE ] || [ -z "$FUNCTION" ]; then
    print_usage;
    exit;
fi

if [ ${#PARAMS[@]} -gt 1 ]; then
    PARAM1=${PARAMS[1]};
fi

if [ ${#PARAMS[@]} -gt 2 ]; then
    PARAM2=${PARAMS[2]};
fi

if [ ${#PARAMS[@]} -gt 3 ]; then
    PARAM3=${PARAMS[3]};
fi

if [ ${#PARAMS[@]} -gt 4 ]; then
    PARAM4=${PARAMS[4]};
fi

#######################parse arg end########################

#######################Action###############################
if [ $# -lt 1 ]; then
    print_usage;
    exit 0;
fi


read_manufacturer()
{
    local Manufacturer=0;
    typeset -i Manufacturer;
	Manufacturer=$(./i2c_4read $I2C_DEV $I2C_ADDR $Manufacturer_Name 2>/dev/null);
	#printf "Manufacturer id is  %s \n" $Manufacturer;
    case $Manufacturer in
    "1447385413")
        #VEYE
        printf "Manufacturer is VEYE\n";
    ;;
    *)
     printf "Manufacturer %8x not recognized\n" $Manufacturer;
     ;;
    esac
}
read_model()
{
    local model=0;
    typeset -i model;
	model=$(./i2c_4read $I2C_DEV $I2C_ADDR $Model_Name 2>/dev/null);
    case $model in
    "662")
        #IMX296
        printf "model is MV-MIPI-IMX296M\n";
    ;;
    "376")
        #IMX178
        printf "model is MV-MIPI-IMX178M\n";
    ;;
    "304")
        printf "model is MV-MIPI-SC130M\n";
    ;;
    "613")
        printf "model is MV-MIPI-IMX265M\n";
    ;;
    "612")
        printf "model is MV-MIPI-IMX264M\n";
    ;;
    "647")
        printf "model is MV-MIPI-IMX287M\n";
    ;;
    "33074")
        printf "model is RAW-MIPI-SC132M\n";
    ;;
    "33332")
        printf "model is RAW-MIPI-AR0234M\n";
    ;;
    "33890")
        printf "model is RAW-MIPI-IMX462M\n";
    ;;
    "34101")
        printf "model is RAW-MIPI-SC535M\n";
    ;;
    "16386")
        printf "model is MV-MIPI-GMAX4002M\n";
    ;;
    *)
     printf " model %8x not recognized\n" $model;
     ;;
    esac
}

read_version()
{
    local version=0;
    typeset -i version;
	version=$(./i2c_4read $I2C_DEV $I2C_ADDR $Device_Version 2>/dev/null);
    printf "version is C %02x.%02x and L %02x.%02x\n" $(($version>>24&0xFF)) $(($version>>16 &0xFF)) $(($version>>8&0xFF)) $(($version&0xFF));
}

read_serialno()
{
    local serialno=0;
    typeset -i serialno;
	serialno=$(./i2c_4read $I2C_DEV $I2C_ADDR $Product_Info 2>/dev/null);
    printf "serialno is 0x%x \n" $serialno;
}

read_timestamp()
{
    local timestamp=0;
    typeset -i timestamp;
	timestamp=$(./i2c_4read $I2C_DEV $I2C_ADDR $Time_stamp 2>/dev/null);
    printf "timestamp is %d \n" $timestamp;
}

read_errcode()
{
    local errcode=0;
    typeset -i errcode;
	errcode=$(./i2c_4read $I2C_DEV $I2C_ADDR $Error_code 2>/dev/null);
    printf "errcode is 0x%x \n" $errcode;
}


read_fmtcap()
{
    local fmtcap=0;
    typeset -i fmtcap;
	fmtcap=$(./i2c_4read $I2C_DEV $I2C_ADDR $Format_Cap 2>/dev/null);
    printf "fmtcap is 0x%x \n" $fmtcap;
}

read_lanecap()
{
    local lanecap=0;
    typeset -i lanecap;
	lanecap=$(./i2c_4read $I2C_DEV $I2C_ADDR $LaneNum_Cap 2>/dev/null);
    printf "lane number cap is 0x%x \n" $lanecap;
}

write_factoryparam()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $System_reset 0x01);
    printf "w factoryparam,all param will reset\n";
}

write_paramsave()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Param_save_to_flash 0x01);
    printf "all param saved to flash\n";
}

write_reboot()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $System_reboot 0x01);
    printf "system reboot!\n";
}

read_i2caddr()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $I2C_Addr 2>/dev/null);
    printf "r i2c addr is 0x%02x \n" $value;
}
write_i2caddr()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $I2C_Addr $PARAM1);
    printf "w i2c addr is 0x%02x \n" $PARAM1;
}

read_i2cport()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $I2C_Port_Sel 2>/dev/null);
    printf "r i2c port is %02x \n" $value;
}

write_i2cport()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $I2C_Port_Sel $PARAM1);
    printf "w i2c port is %02x \n" $PARAM1;
}

write_imgacq()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Image_Acquisition $PARAM1);
    printf "w image acq is %02x \n" $PARAM1;
}

read_metime()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $ME_Time 2>/dev/null);
    printf "r manual exp time %d us \n" $value;
}

write_metime()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $ME_Time $PARAM1);
    printf "w manual exp time is %d us \n" $PARAM1;
}

read_mgain()
{
    local value=0;
    local gain=0;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Manual_Gain 2>/dev/null);
    gain=`awk -v x="$value" 'BEGIN {printf "%.1f\n",x/10}'`
    printf "r manual gain %.1f dB \n" $gain;
}

write_mgain()
{
    local res=0;
    local reg_val=0;
    reg_val=`awk -v x="$PARAM1" 'BEGIN {printf ("%d\n",x*10)}'`;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Manual_Gain $reg_val);
    printf "w mgain %.01f dB\n" $PARAM1;
}

read_maxwh()
{
    local width=0;
    local height=0;
    typeset -i width;
    typeset -i height;
    width=$(./i2c_4read $I2C_DEV $I2C_ADDR $Sensor_Width 2>/dev/null);
    height=$(./i2c_4read $I2C_DEV $I2C_ADDR $Sensor_Height 2>/dev/null);
    printf "r sensor max width %d max height %d \n" $width $height;
}

read_maxfps()
{
    local value=0;
    local maxfps=0;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $MaxFrame_Rate 2>/dev/null);
    maxfps=`awk -v x="$value" 'BEGIN {printf "%.2f\n",x/100}'`
    printf "r maxfps @ current setting is %.2f fps\n" $maxfps;
}

read_minfps()
{
    local value=0;
    local minfps=0;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Min_Frame_Rate 2>/dev/null);
    minfps=`awk -v x="$value" 'BEGIN {printf "%.4f\n",x/10000}'`
    printf "r minfps @ current setting is %.4f fps\n" $minfps;
}

read_imgdir()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Image_Direction 2>/dev/null);
    printf "r image direction is %d \n" $value;
}
write_imgdir()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Image_Direction $PARAM1);
    printf "w image direction is %d \n" $PARAM1;
}

read_blacklevel()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Black_Level 2>/dev/null);
    printf "r black level is %d \n" $value;
}

write_blacklevel()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Black_Level $PARAM1);
    printf "w black level is %d \n" $PARAM1;
}

read_blcmode()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $BLC_Mode 2>/dev/null);
    printf "r BLC_Mode is %d \n" $value;
}

write_blcmode()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $BLC_Mode $PARAM1);
    printf "w BLC_Mode is %d \n" $PARAM1;
}

read_roi()
{
    local x=0;
    local y=0;
    local width=0;
    local height=0;
    typeset -i x;
    typeset -i y;
    typeset -i width;
    typeset -i height;
    
    x=$(./i2c_4read $I2C_DEV $I2C_ADDR $ROI_Offset_X 2>/dev/null);
    y=$(./i2c_4read $I2C_DEV $I2C_ADDR $ROI_Offset_Y 2>/dev/null);
    width=$(./i2c_4read $I2C_DEV $I2C_ADDR $ROI_Width 2>/dev/null);
    height=$(./i2c_4read $I2C_DEV $I2C_ADDR $ROI_Height 2>/dev/null);
    
    printf "r roi is %d,%d,%d,%d \n" $x $y $width $height;
}
write_roi()
{
    local res=0;
    res=$(./i2c_4write $I2C_DEV $I2C_ADDR $ROI_Offset_X $PARAM1);
    res=$(./i2c_4write $I2C_DEV $I2C_ADDR $ROI_Offset_Y $PARAM2);
    res=$(./i2c_4write $I2C_DEV $I2C_ADDR $ROI_Width $PARAM3);
    res=$(./i2c_4write $I2C_DEV $I2C_ADDR $ROI_Height $PARAM4);
    printf "w roi is %d,%d,%d,%d \n" $PARAM1 $PARAM2 $PARAM3 $PARAM4;
}

read_testimg()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Test_Image_Selector 2>/dev/null);
    printf "r test image is %d \n" $value;
}

write_testimg()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Test_Image_Selector $PARAM1);
    printf "w test image is %d \n" $PARAM1;
}

read_fps()
{
    local value=0;
    local fps=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Framerate 2>/dev/null);
    fps=`awk -v x="$value" 'BEGIN {printf "%.2f\n",x/100}'`
    printf "r fps is %.02f fps\n" $fps;
}
write_fps()
{
    local res=0;
    local reg_val=0;
    reg_val=`awk -v x="$PARAM1" 'BEGIN {printf ("%d\n",x*100)}'`;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Framerate $reg_val);
    printf "w fps %.02f\n" $PARAM1 ;
}

read_fps_ex()
{
    local value=0;
    local fps=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $FrameRate_Ex 2>/dev/null);
    fps=`awk -v x="$value" 'BEGIN {printf "%.4f\n",x/10000}'`
    printf "r fps is %.04f fps\n" $fps;
}
write_fps_ex()
{
    local res=0;
    local reg_val=0;
    reg_val=`awk -v x="$PARAM1" 'BEGIN {printf ("%d\n",x*10000)}'`;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $FrameRate_Ex $reg_val);
    printf "w fps %.04f\n" $PARAM1 ;
}

read_trgmode()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Trigger_Mode 2>/dev/null);
    printf "r trigger mode is %d \n" $value;
}

write_trgmode()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Trigger_Mode $PARAM1);
    printf "w trigger mode is %d \n" $PARAM1;
}

read_trgsrc()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Trigger_Source 2>/dev/null);
    printf "r trigger source is %d \n" $value;
}

write_trgsrc()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Trigger_Source $PARAM1);
    printf "w trigger source is %d \n" $PARAM1;
}

read_trgnum()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Trigger_Num 2>/dev/null);
    printf "r trigger num is %d \n" $value;
}
write_trgnum()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Trigger_Num $PARAM1);
    printf "w trigger num is %d \n" $PARAM1;
}

read_trginterval()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Trigger_Inerval 2>/dev/null);
    printf "r trginterval is %d \n" $value;
}
write_trginterval()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Trigger_Inerval $PARAM1);
    printf "w trginterval is %d \n" $PARAM1;
}

write_trgone()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Trigger_Software 1);
    printf "w trigger one \n" ;
}

read_trgcount()
{
    local value=0;
    local trg_lost=0;
    local trg_count=0;
    typeset -i value;
    typeset -i trg_lost;
    typeset -i trg_count;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Trigger_Count 2>/dev/null);
    trg_lost=$((value>>16));
    trg_count=$((value&0xFFFF));
    printf "r trigger count %d, trigger lost %d\n" $trg_count $trg_lost;
}

write_trgcountclr()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Trigger_Count 1);
    printf "w clear trigger count\n";
}

read_trgdelay()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Trigger_Delay 2>/dev/null);
    printf "r trgdelay is %d \n" $value;
}
write_trgdelay()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Trigger_Delay $PARAM1);
    printf "w trgdelay is %d \n" $PARAM1;
}

read_trgedge()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Trigger_Activation 2>/dev/null);
    printf "r trg edge is %d \n" $value;
}
write_trgedge()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Trigger_Activation $PARAM1);
    printf "w trg edge is %d \n" $PARAM1;
}

read_trgfilter_enable()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Trigger_Filter_Enable 2>/dev/null);
    printf "r trgfilter enable is %d \n" $value;
}
write_trgfilter_enable()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Trigger_Filter_Enable $PARAM1);
    printf "w trgfilter enable is %d \n" $PARAM1;
}

read_trgfilter_time()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Trigger_Filter_Width 2>/dev/null);
    printf "r trgfilter time is %d us\n" $value;
}
write_trgfilter_time()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Trigger_Filter_Width $PARAM1);
    printf "w trgfilter time is %d us\n" $PARAM1;
}

read_trgexp_delay()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Trigger_Exp_Delay 2>/dev/null);
    printf "r trigger exposure delay is %d us\n" $value;
}
write_trgexp_delay()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Trigger_Exp_Delay $PARAM1);
    printf "w trigger exposure delay %d us\n" $PARAM1;
}

read_gpios_status()
{
    local io_status=0;
    local in=0;
    local out1=0;
    local out2=0;
    typeset -i io_status;
    typeset -i in;
    typeset -i out1;
    typeset -i out2;
	io_status=$(./i2c_4read $I2C_DEV $I2C_ADDR $GPIOS_Status 2>/dev/null);
    in=$((io_status&0x01));
    out1=$(((io_status>>1)&0x01));
    out2=$(((io_status>>2)&0x01));
    printf "r reg %x TriggerIN_IO status is %x OUT_IO1 status is %x OUT_IO2 status is %x\n" $io_status $in $out1 $out2;
}

read_outio1_mode()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $GPIO1_OutSelect 2>/dev/null);
    printf "r outIO1 mode is %d \n" $value;
}
write_outio1_mode()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $GPIO1_OutSelect $PARAM1);
    printf "w outIO1 mode is %d\n" $PARAM1;
}

read_outio1_usr()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $GPIO1_Useroutput 2>/dev/null);
    printf "r outIO1 user val is %d \n" $value;
}

write_outio1_usr()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $GPIO1_Useroutput $PARAM1);
    printf "w outIO1 val is %d\n" $PARAM1;
}

read_outio1_rvs()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $GPIO1_Reverse 2>/dev/null);
    printf "r outIO1 Reverse val is %d \n" $value;
}

write_outio1_rvs()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $GPIO1_Reverse $PARAM1);
    printf "w outIO1 Reverse is %d\n" $PARAM1;
}


read_outio2_mode()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $GPIO2_OutSelect 2>/dev/null);
    printf "r outIO2 mode is %d \n" $value;
}
write_outio2_mode()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $GPIO2_OutSelect $PARAM1);
    printf "w outIO2 mode is %d\n" $PARAM1;
}

read_outio2_usr()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $GPIO2_Useroutput 2>/dev/null);
    printf "r outIO2 user val is %d \n" $value;
}

write_outio2_usr()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $GPIO2_Useroutput $PARAM1);
    printf "w outIO2 val is %d\n" $PARAM1;
}

read_outio2_rvs()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $GPIO2_Reverse 2>/dev/null);
    printf "r outIO2 Reverse val is %d \n" $value;
}

write_outio2_rvs()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $GPIO2_Reverse $PARAM1);
    printf "w outIO2 Reverse is %d\n" $PARAM1;
}


read_pixelformat()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Pixel_Format 2>/dev/null);
    printf "r pixelformat is %d \n" $value;
}
write_pixelformat()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Pixel_Format $PARAM1);
    printf "w pixelformat is %d \n" $PARAM1;
}

read_gamma()
{
    local value=0;
    local gamma=0;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Gamma 2>/dev/null);
    gamma=`awk -v x="$value" 'BEGIN {printf "%.2f\n",x/100}'`
    printf "r gamma is %.2f \n" $gamma;
}

write_gamma()
{
    local res=0;
    local reg_val=0;
    reg_val=`awk -v x="$PARAM1" 'BEGIN {printf ("%d\n",x*100)}'`;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Gamma $reg_val);
    printf "w gamma %.02f\n" $PARAM1 ;
}

read_gammaenable()
{
    local value=0;
    local bit=5;
    local enable=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $ISP_module_ctrl 2>/dev/null);
    enable=$((value>>bit&0x1));
    printf "r gamma enable is %d \n" $enable;
}
write_gammaenable()
{
    local res=0;
    local bit=5;
    local enable=0;
    value=$(./i2c_4read $I2C_DEV $I2C_ADDR $ISP_module_ctrl 2>/dev/null);
    if [ $PARAM1 -eq 1 ]; then
        value=$((value|0x20));
	elif [ $PARAM1 -eq 0 ]; then
        value=$((value&0xFFFFFFDF));#set bit 5 zero
	fi    
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $ISP_module_ctrl $value);

    printf "w gamma enable is %d \n" $PARAM1 ;
}

read_dpcenable()
{
    local value=0;
    local bit=4;
    local enable=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $ISP_module_ctrl 2>/dev/null);
    enable=$((value>>bit&0x1));
    printf "r dpc enable is %d \n" $enable;
}

write_dpcenable()
{
    local res=0;
    local bit=4;
    local enable=0;
    value=$(./i2c_4read $I2C_DEV $I2C_ADDR $ISP_module_ctrl 2>/dev/null);
    if [ $PARAM1 -eq 1 ]; then
        value=$((value|0x10));
	elif [ $PARAM1 -eq 0 ]; then
        value=$((value&0xFFFFFFEF));#set bit 4 zero
	fi    
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $ISP_module_ctrl $value);
    printf "w dpc enable is %d \n" $PARAM1 ;
}

read_lutenable()
{
    local value=0;
    local bit=8;
    local enable=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $ISP_module_ctrl 2>/dev/null);
    enable=$((value>>bit&0x1));
    printf "r lut enable is %d \n" $enable;
}

write_lutenable()
{
    local res=0;
    local bit=8;
    local enable=0;
    value=$(./i2c_4read $I2C_DEV $I2C_ADDR $ISP_module_ctrl 2>/dev/null);
    if [ $PARAM1 -eq 1 ]; then
        value=$((value|0x100));
	elif [ $PARAM1 -eq 0 ]; then
        value=$((value&0xFFFFFEFF));#set bit 8 zero
	fi    
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $ISP_module_ctrl $value);
    printf "w lut enable is %d \n" $PARAM1 ;
}

read_lut()
{
    local value=0;
    typeset -i value;
	value=$(./lut_rw $I2C_DEV $I2C_ADDR r $PARAM1 2>/dev/null);
    printf "r lut and save to file : %s \n" $PARAM1;
}
write_lut()
{
    local value=0;
    typeset -i value;
    value=$(./lut_rw $I2C_DEV $I2C_ADDR w $PARAM1 2>/dev/null);
    if [ $value -eq 0 ]; then
        printf "w lut from file : %s successs\n" $PARAM1;
	else
        printf "Sorry, an error occurred, please try again\n";
	fi  
}

read_expmode()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Exposure_Mode 2>/dev/null);
    printf "r expmode is %d \n" $value;
}
write_expmode()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Exposure_Mode $PARAM1);
    printf "w expmode is %d \n" $PARAM1;
}

read_gainmode()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Gain_Mode 2>/dev/null);
    printf "r gain mode is %d \n" $value;
}
write_gainmode()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Gain_Mode $PARAM1);
    printf "w gain mode is %d \n" $PARAM1;
}


read_aatarget()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Target_Brightness 2>/dev/null);
    printf "r aatarget is %d \n" $value;
}
write_aatarget()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Target_Brightness $PARAM1);
    printf "w aatarget is %d \n" $PARAM1;
}

read_aemaxtime()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $AE_MAX_Time 2>/dev/null);
    printf "r aemaxtime is %d us\n" $value;
}
write_aemaxtime()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $AE_MAX_Time $PARAM1);
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $AE_MAX_Time 2>/dev/null);
    printf "w aemaxtime is %d us\n" $PARAM1;
    printf "read back take effective ae max time\n";
    read_aemaxtime;
}

read_exptime()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Exp_Time 2>/dev/null);
    printf "r current exptime is %d us\n" $value;
}

read_curgain()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Cur_Gain 2>/dev/null);
    gain=`awk -v x="$value" 'BEGIN {printf "%.1f\n",x/10}'`
    printf "r currnet gain %.1f dB \n" $gain;
    
}

read_agmaxgain()
{
    local value=0;
    local gain=0;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $AG_Max_Gain 2>/dev/null);
    gain=`awk -v x="$value" 'BEGIN {printf "%.1f\n",x/10}'`
    printf "r ag maxgain gain %.1f dB \n" $gain;
}

write_agmaxgain()
{
    local res=0;
    local reg_val=0;
    reg_val=`awk -v x="$PARAM1" 'BEGIN {printf ("%d\n",x*10)}'`;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $AG_Max_Gain $reg_val);
    # read back to check effective value
    printf "w ag maxgain %.01f dB\n" $PARAM1;
    printf "read back take effective gain\n";
    read_agmaxgain;
}

read_aaroi()
{
    local x=0;
    local y=0;
    local width=0;
    local height=0;
    typeset -i x;
    typeset -i y;
    typeset -i width;
    typeset -i height;
    
    x=$(./i2c_4read $I2C_DEV $I2C_ADDR $AAROIOffsetX 2>/dev/null);
    y=$(./i2c_4read $I2C_DEV $I2C_ADDR $AAROIOffsetY 2>/dev/null);
    width=$(./i2c_4read $I2C_DEV $I2C_ADDR $AAROIWidth 2>/dev/null);
    height=$(./i2c_4read $I2C_DEV $I2C_ADDR $AAROIHeight 2>/dev/null);
    
    printf "r ae again roi is %d,%d,%d,%d \n" $x $y $width $height;
}
write_aaroi()
{
    local res=0;
    res=$(./i2c_4write $I2C_DEV $I2C_ADDR $AAROIOffsetX $PARAM1);
    res=$(./i2c_4write $I2C_DEV $I2C_ADDR $AAROIOffsetY $PARAM2);
    res=$(./i2c_4write $I2C_DEV $I2C_ADDR $AAROIWidth $PARAM3);
    res=$(./i2c_4write $I2C_DEV $I2C_ADDR $AAROIHeight $PARAM4);
    printf "w ae again roi is %d,%d,%d,%d \n" $PARAM1 $PARAM2 $PARAM3 $PARAM4;
}

read_aaroienable()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $AAROI_enable 2>/dev/null);
    printf "r aaroienable is %d \n" $value;
}
write_aaroienable()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $AAROI_enable $PARAM1);
    printf "w aaroienable is %d \n" $PARAM1;
}

read_snsreg()
{
    local res=0;
    local sensor_val=0;
    typeset -i sensor_val;
    res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Sensor_Reg_Addr $PARAM1);
    sleep 0.01;
    sensor_val=$(./i2c_4read $I2C_DEV $I2C_ADDR $Sensor_Reg_Val 2>/dev/null);
    #check if msb set to 1
    if [ $((($sensor_val & 0x80000000) == 0x80000000)) -eq 1 ]; then
        sensor_val=$((sensor_val&0xFFFF));
        printf "read sensor register addr 0x%x value 0x%x \n" $PARAM1 $sensor_val;
    else
        printf "read sensor register failed\n";
	fi  
}

read_clkmode()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Nondiscontinuous_mode 2>/dev/null);
    printf "r non discontinuous_mode mode is %d \n" $value;
}

read_mipidatarate()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $MIPI_DataRate 2>/dev/null);
    printf "r MIPI datarate is %d Kbps/lane\n" $value;
}
read_lanenum()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Lane_Num 2>/dev/null);
    printf "r lane number is %d \n" $value;
}

write_lanenum()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Lane_Num $PARAM1);
    printf "w lane number is %d \n" $PARAM1;
}

read_slavemode()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Slave_mode 2>/dev/null);
    printf "r slave mode is %d \n" $value;
}

write_slavemode()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $Slave_mode $PARAM1);
    printf "w slave mode is %d \n" $PARAM1;
}

read_framecount()
{
    local sns_count=0;
    local out_count=0;
    typeset -i sns_count;
    typeset -i out_count;
	sns_count=$(./i2c_4read $I2C_DEV $I2C_ADDR $Sensor_Frame_Count 2>/dev/null);
    out_count=$(./i2c_4read $I2C_DEV $I2C_ADDR $Out_Frame_Count 2>/dev/null);
    printf "r Sensor framecount is %d , Out framecount is %d\n" $sns_count $out_count;
}

read_trgmodecap()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $TriggerMode_Cap 2>/dev/null);
    printf "r trigger mode capbility is 0x%x \n" $value;
}

read_trgcycle()
{
    local cycle_min=0;
    local cycle_max=0;
    typeset -i cycle_min;
    typeset -i cycle_max;
	cycle_min=$(./i2c_4read $I2C_DEV $I2C_ADDR $Trigger_Cycle_Min 2>/dev/null);
    cycle_max=$(./i2c_4read $I2C_DEV $I2C_ADDR $Trigger_Cycle_Max 2>/dev/null);
    printf "r trigger cycle min is %d us,trigger cycle max is %d us\n" $cycle_min $cycle_max;
}

read_minwh()
{
    local width=0;
    local height=0;
    typeset -i width;
    typeset -i height;
    width=$(./i2c_4read $I2C_DEV $I2C_ADDR $MIN_ROI_Width 2>/dev/null);
    height=$(./i2c_4read $I2C_DEV $I2C_ADDR $MIN_ROI_Height 2>/dev/null);
    printf "r ROI min width is %d height is %d\n" $width $height;
}
read_temp()
{
    local value=0
    local kelvin=0
    local celsius=0

    # Read temperature value, unit is 100 times Kelvin
    value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Temp_K 2>/dev/null)

    # Calculate the actual Kelvin temperature
    kelvin=$(echo "scale=2; $value / 100" | bc)

    # Calculate Celsius temperature
    celsius=$(echo "scale=2; $kelvin - 273.15" | bc)

    # Print temperature values
    printf "r temperature is %.2f K (%.2f °C)\n" $kelvin $celsius
}

read_exptime_range()
{
    local value=0;
    local min=0;
    local max=0;
    value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Min_Exp_time 2>/dev/null);
    #0.1us as a unit
    min=$(echo "$value * 0.1" | bc);
    value=$(./i2c_4read $I2C_DEV $I2C_ADDR $Max_Exp_time 2>/dev/null);
    max=$value;
    printf "r exposure time range is: %.1f us ~ %d us\n" $min $max
}

read_osdmode()
{
    local value=0;
    typeset -i value;
    value=$(./i2c_4read $I2C_DEV $I2C_ADDR $OSD_Mode 2>/dev/null);
    printf "r osd mode is %d \n" $value;
}

write_osdmode()
{
    local res=0;
    res=$(./i2c_4write $I2C_DEV $I2C_ADDR $OSD_Mode $PARAM1);
    printf "w osd mode is %d \n" $PARAM1;
}

read_readmodecap()
{
    local value=0;
    typeset -i value;
    value=$(./i2c_4read $I2C_DEV $I2C_ADDR $ReadMode_Cap 2>/dev/null);
    printf "r read mode capbility is 0x%x \n" $value;
}

read_readmode()
{
    local value=0;
    typeset -i value;
    value=$(./i2c_4read $I2C_DEV $I2C_ADDR $ReadOut_Mode 2>/dev/null);
    printf "r read mode is 0x%x \n" $value;
}

write_readmode()
{
    local res=0;
    res=$(./i2c_4write $I2C_DEV $I2C_ADDR $ReadOut_Mode $PARAM1);
    printf "w read mode is 0x%x \n" $PARAM1;
}

<<'COMMENT_SAMPLE'
read_fun()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_4read $I2C_DEV $I2C_ADDR $[reg] 2>/dev/null);
    printf "r [fun] is %d \n" $value;
}
write_fun()
{
    local res=0;
	res=$(./i2c_4write $I2C_DEV $I2C_ADDR $[reg] $PARAM1);
    printf "w [fun] is %d \n" $PARAM1;
}
COMMENT_SAMPLE
# 
write_aeag_run_once_save()
{
    # change this value if you need Limitations
    local MAX_AE_TIME=1000000;
    local MAX_AG_GAIN=100;
    
    local expresult=0;
    typeset -i expresult;
    local gainresult=0;
    typeset -i gainresult;
    local gain_dB=0;
    printf "Execute AE&AG once operation and save the results to the camera as manual values!\n";
    printf "[Step 1]: set AE AG Max value\n"
    PARAM1=$MAX_AE_TIME;
    write_aemaxtime >/dev/null;
    PARAM1=$MAX_AG_GAIN;
    write_agmaxgain >/dev/null;
    printf "[Step 2]: set AE AG to ONCE mode\n"
    PARAM1=1;
    write_expmode>/dev/null;
    write_gainmode>/dev/null;
    printf "[Step 3]: wait 5 seconds\n"
    sleep 5;
    printf "[Step 4]: check ONCE result\n"
    read_errcode;
    expresult=$(./i2c_4read $I2C_DEV $I2C_ADDR $Exp_Time 2>/dev/null);
    gainresult=$(./i2c_4read $I2C_DEV $I2C_ADDR $Cur_Gain 2>/dev/null);
    gain_dB=`awk -v x="$gainresult" 'BEGIN {printf "%.1f\n",x/10}'`
    printf "[ONCE result]: exp time %d us, gain %.1f dB\n" $expresult $gain_dB

    printf "[Step 5]: set to manual mode\n"
    PARAM1=0;
    write_expmode>/dev/null;
    write_gainmode>/dev/null;
    PARAM1=$expresult;
    write_metime>/dev/null;
    PARAM1=$gain_dB;
    write_mgain>/dev/null;
    sleep 0.5;
    printf "[Step 6]: check resutlt\n"
    read_expmode;
    read_exptime;
    read_gainmode;
    read_curgain;
    write_paramsave;

    printf "[Finsh!] save param to flash\n"
    
}

#######################Action# BEGIN##############################

if [ ${MODE} = "read" ] ; then
	case $FUNCTION in
		"manufacturer")
			read_manufacturer;
            ;;
        "model")
            read_model;
            ;;
        "version")
            read_version;
            ;;
        "serialno")
            read_serialno;
            ;;
        "timestamp")
            read_timestamp;
            ;;
        "errcode")
            read_errcode;
            ;;
        "fmtcap")
            read_fmtcap;
            ;;
        "i2caddr")
            read_i2caddr;
            ;;
        "i2cport")
            read_i2cport;
            ;;
        "metime")
            read_metime;
            ;;
        "mgain")
            read_mgain;
            ;;
        "maxwh")
            read_maxwh;
            ;;
        "maxfps")
            read_maxfps;
            ;;
        "minfps")
            read_minfps;
            ;;
        "imgdir")
            read_imgdir;
            ;;
        "blacklevel")
            read_blacklevel;
            ;;
        "roi")
            read_roi;
            ;;
        "testimg")
            read_testimg;
            ;;
        "fps")
            read_fps;
            ;;
        "fps_ex")
            read_fps_ex;
            ;;
        "trgmode")
            read_trgmode;
            ;;
        "trgsrc")
            read_trgsrc;
            ;;
        "trginterval")
            read_trginterval;
            ;;
        "trgcount")
            read_trgcount;
            ;;
        "trgnum")
            read_trgnum;
            ;;
        "trgdelay")
            read_trgdelay;
            ;;
        "trgedge")
            read_trgedge;
            ;;
        "trgfilter_enable")
            read_trgfilter_enable;
            ;;
        "trgfilter_time")
            read_trgfilter_time;
            ;;
        "trgexp_delay")
            read_trgexp_delay;
            ;;
        "gpios_status")
            read_gpios_status;
            ;;
        "outio1_mode")
            read_outio1_mode;
            ;;
        "outio1_usr")
            read_outio1_usr;
            ;;
        
        "outio1_rvs")
            read_outio1_rvs;
            ;;
        "outio2_mode")
            read_outio2_mode;
            ;;
        "outio2_usr")
            read_outio2_usr;
            ;;
        "outio2_rvs")
            read_outio2_rvs;
            ;;
        "pixelformat")
            read_pixelformat;
            ;;
        "gamma")
            read_gamma;
            ;;
        "gammaenable")
            read_gammaenable;
            ;;
        "dpcenable")
            read_dpcenable;
            ;;
        "lutenable")
            read_lutenable;
            ;;
        "lut")
            read_lut;
            ;;
        "expmode")
            read_expmode;
            ;;
        "gainmode")
            read_gainmode;
            ;;
        "aatarget")
            read_aatarget;
            ;;
        "aemaxtime")
            read_aemaxtime;
            ;;
        "exptime")
            read_exptime;
            ;;
        "curgain")
            read_curgain;
            ;;
        "agmaxgain")
            read_agmaxgain;
            ;;
        "aaroi")
            read_aaroi;
            ;;
        "aaroienable")
            read_aaroienable;
            ;;
        "snsreg")
            read_snsreg;
            ;;
        "clkmode")
            read_clkmode;
            ;;
        "mipidatarate")
            read_mipidatarate;
            ;;
        "lanenum")
            read_lanenum;
            ;;
        "blcmode")
            read_blcmode;
            ;;
        "slavemode")
            read_slavemode;
            ;;
        "framecount")
            read_framecount;
            ;;
        "trgmodecap")
            read_trgmodecap;
            ;;
        "trgcycle")
            read_trgcycle;
            ;;
        "minwh")
            read_minwh;
            ;;
        "lanecap")
            read_lanecap;
            ;;
        "temp")
            read_temp;
            ;;
        "exptime_range")
            read_exptime_range;
            ;;
        "osdmode")
            read_osdmode;
            ;;
        "readmodecap")
            read_readmodecap;
            ;;
        "readmode")
            read_readmode;
            ;;
        *)
        echo "NOT SUPPORTED!";
        ;;
	esac
fi

if [ ${MODE} = "write" ] ; then
	case $FUNCTION in
		"factoryparam")
			write_factoryparam;
			;;
        "paramsave")
			write_paramsave;
			;;
        "reboot")
			write_reboot;
			;;
        "i2caddr")
            write_i2caddr;
            ;;
        "i2cport")
            write_i2cport;
            ;;
        "imgacq")
            write_imgacq;
            ;;
        "metime")
            write_metime;
            ;;
        "mgain")
            write_mgain;
            ;;
        "imgdir")
            write_imgdir;
            ;;
        "blacklevel")
            write_blacklevel;
            ;;
        "roi")
            write_roi;
            ;;
        "testimg")
            write_testimg;
            ;;
        "fps")
            write_fps;
            ;;
        "fps_ex")
            write_fps_ex;
            ;;
        "trgmode")
            write_trgmode;
            ;;
        "trgsrc")
            write_trgsrc;
            ;;
        "trginterval")
            write_trginterval;
            ;;
        "trgnum")
            write_trgnum;
            ;;
        "trgone")
            write_trgone;
            ;;
        "trgcountclr")
            write_trgcountclr;
            ;;
        "trgdelay")
            write_trgdelay;
            ;;
        "trgedge")
            write_trgedge;
            ;;
        "trgfilter_enable")
            write_trgfilter_enable;
            ;;
        "trgfilter_time")
            write_trgfilter_time;
            ;;
        "trgexp_delay")
            write_trgexp_delay;
            ;;
        "outio1_mode")
            write_outio1_mode;
            ;;
        "outio1_usr")
            write_outio1_usr;
            ;;
        "outio1_rvs")
            write_outio1_rvs;
            ;;
        "outio2_mode")
            write_outio2_mode;
            ;;
        "outio2_usr")
            write_outio2_usr;
            ;;
        "outio2_rvs")
            write_outio2_rvs;
            ;;
        "pixelformat")
            write_pixelformat;
            ;;
        "gamma")
            write_gamma;
            ;;
        "gammaenable")
            write_gammaenable;
            ;;
        "dpcenable")
            write_dpcenable;
            ;;
        "lutenable")
            write_lutenable;
            ;;
        "lut")
            write_lut;
            ;;
        "expmode")
            write_expmode;
            ;;
        "gainmode")
            write_gainmode;
            ;;
        "aatarget")
            write_aatarget;
            ;;
        "aemaxtime")
            write_aemaxtime;
            ;;
        "exptime")
            write_exptime;
            ;;
        "curgain")
            write_curgain;
            ;;
        "agmaxgain")
            write_agmaxgain;
            ;;
        "aaroi")
            write_aaroi;
            ;;
        "aaroienable")
            write_aaroienable;
            ;;
        "aeag_run_once_save")
            write_aeag_run_once_save;
            ;;
        "lanenum")
            write_lanenum;
            ;;
        "blcmode")
            write_blcmode;
            ;;
        "slavemode")
            write_slavemode;
            ;;
        "osdmode")
            write_osdmode;
            ;;
        "readmode")
            write_readmode;
            ;;
        *)
        echo "NOT SUPPORTED!";
        ;;
	esac
fi

