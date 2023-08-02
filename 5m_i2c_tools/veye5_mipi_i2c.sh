#!/bin/bash

<<'COMMENT_SAMPLE'
./veye5_mipi_i2c.sh -r -f manufacturer
./veye5_mipi_i2c.sh -r -f model
./veye5_mipi_i2c.sh -r -f version
./veye5_mipi_i2c.sh -r -f timestamp
./veye5_mipi_i2c.sh -w -f factoryparam
./veye5_mipi_i2c.sh -w -f paramsave
./veye5_mipi_i2c.sh -w -f reboot
./veye5_mipi_i2c.sh -r -f errcode

./veye5_mipi_i2c.sh -w -f i2caddr -p1 0x3c
./veye5_mipi_i2c.sh -r -f i2caddr 

./veye5_mipi_i2c.sh -w -f i2cport -p1 1
./veye5_mipi_i2c.sh -r -f i2cport 

#0 day 1 night 2 trigger
./veye5_mipi_i2c.sh -w -f daynightmode -p1 [0,2]
./veye5_mipi_i2c.sh -r -f daynightmode

./veye5_mipi_i2c.sh -w -f ircutdir -p1 [0,1]
./veye5_mipi_i2c.sh -r -f ircutdir 

./veye5_mipi_i2c.sh -w -f irtrigger -p1 [0,1]
./veye5_mipi_i2c.sh -r -f irtrigger 

./veye5_mipi_i2c.sh -r -f sensor_reg -p1 [sensor reg addr]

VIDEO_2592x1944p_20 = 1,//5M@20fps
VIDEO_2592x1944p_12_5 = 2,//5M@12.5fps
VIDEO_2560x1440p_25 = 3,//4M@25fps
VIDEO_2560x1440p_30 = 4,//4M@30fps
VIDEO_3840x2160p_12_5 = 5,//4K@12.5
VIDEO_3840x2160p_15 = 6,//4K@15
./veye5_mipi_i2c.sh -w -f videomode -p1 1
./veye5_mipi_i2c.sh -r -f videomode 

./veye5_mipi_i2c.sh -w -f test_pattern -p1 [test_enable] -p2 [type]
./veye5_mipi_i2c.sh -r -f test_pattern 

#0,auto;1,oneset
./veye5_mipi_i2c.sh -w -f awb_mode -p1 [0,1]
./veye5_mipi_i2c.sh -r -f awb_mode 

./veye5_mipi_i2c.sh -w -f antiflicker -p1 [0,1]
./veye5_mipi_i2c.sh -r -f antiflicker 

./veye5_mipi_i2c.sh -w -f agc_max -p1 [0,0xf]
./veye5_mipi_i2c.sh -r -f agc_max 

./veye5_mipi_i2c.sh -w -f ae_target -p1 [0,0x64]
./veye5_mipi_i2c.sh -r -f ae_target 

./veye5_mipi_i2c.sh -w -f ae_speed -p1 [0,0x64]
./veye5_mipi_i2c.sh -r -f ae_speed 

./veye5_mipi_i2c.sh -w -f agc_speed -p1 [0,0x64]
./veye5_mipi_i2c.sh -r -f agc_speed 

./veye5_mipi_i2c.sh -r -f exp_val 

./veye5_mipi_i2c.sh -r -f agc_val 

#0,auto; others, manual value
./veye5_mipi_i2c.sh -w -f mshutter -p1 [0,0x0B]
./veye5_mipi_i2c.sh -r -f mshutter 

#0x0 : Off(Normal Image)  0x1 : Mirror  0x2 : V-Flip  0x3 : 180 Degree Rotate  
./veye5_mipi_i2c.sh -w -f mirrormode -p1 [0,0x03]
./veye5_mipi_i2c.sh -r -f mirrormode 

./veye5_mipi_i2c.sh -w -f denoise_2d -p1 [0,0x3]
./veye5_mipi_i2c.sh -r -f denoise_2d

./veye5_mipi_i2c.sh -w -f denoise_3d -p1 [0,0x3]
./veye5_mipi_i2c.sh -r -f denoise_3d

#0x0 : OFF  0x09 :auto LOW  0x0B :auto MIDDLE  0x0C : auto HIGH 
./veye5_mipi_i2c.sh -w -f sharppen -p1 [0,0x0f]
./veye5_mipi_i2c.sh -r -f sharppen

./veye5_mipi_i2c.sh -w -f saturation -p1 [0,0x0A]
./veye5_mipi_i2c.sh -r -f saturation

./veye5_mipi_i2c.sh -w -f extra_color_gain -p1 [0,0xFF]
./veye5_mipi_i2c.sh -r -f extra_color_gain
COMMENT_SAMPLE


I2C_DEV=10;
I2C_ADDR=0x3b;

print_usage()
{
	echo "Usage:  ./veye_mipi_i2c.sh [-r/w] [-f] function name -p1 param1 -p2 param2 -b bus"
	echo "options:"
	echo "    -r                       read "
	echo "    -w                       write"
	echo "    -f [function name]       function name"
	echo "    -p1 [param1] 			   param1 of each function"
	echo "    -p2 [param1] 			   param2 of each function"
	echo "    -b [i2c bus num] 		   i2c bus number"
	echo "    -d [i2c addr] 		   i2c addr if not default 0x3b"
	echo "Please open this srcipt and read the COMMENT on top for support functions and samples"
}
# base functions
Manufacturer_Name=0x0000;
Model_Name=0x0004;
Sensor_Name=0x0008;
Serial_Num=0x000C;
Device_Version=0x0010;
System_reset=0x0014;
Param_save_to_flash=0x0018;
System_reboot=0x001C;
Time_stamp=0x0020;
Error_Code=0x0024;

I2C_Addr=0x400;
I2C_Port_Sel=0x404;
Day_Night_Mode=0x408;
IRCUT_Dir=0x40C;
Day_Night_Trgpin_Plr=0x410;
Video_Mode=0x414;

I2C_Special_ADDR=0x30000000;
I2C_Special_RETVAL=0x30000004;

ISP_ADDR_BASAE=0x10000000;

TEST_PATTERN=0x1000A574;
WB_MODE=0x1000EB0B;
ANTI_FLICKER=0x1000EB1F; #bit4
AGC_MAX=0x1000EB1F; #bit3-0
NTSC_AE_TARGET=0x1000EB18;
PAL_AE_TARGET=0x1000EB19;
AE_SPEED=0x1000EB1A;
AGC_SPEED=0x1000EB1B;
SHUTTER_VAL_M=0x1000EB16;
SHUTTER_VAL_L=0x1000EB17;
M_SHUTTER=0x1000EB0C;
AGC_LEVEL=0x1000AB3A;
MIRROR_FLIP=0x1000E80A;
DENOISE_2D=0x1000EAE1;
DENOISE_3D=0x1000EF67;
SHARP_MODE=0x1000E957; #bit0 is enable
SATURATION_B=0x1000EA08;
SATURATION_R=0x1000EA09;
EXTRA_GAIN_B=0x1000EB3D;
EXTRA_GAIN_R=0x1000EB3E;
ISP_PARAM_SAVE=0x1000EB78;


SENSOR_ADDR_BASAE=0x20000000;

######################parse arg###################################
MODE=read;
FUNCTION=nullfunction;
PARAM1=0;
PARAM2=0;
PARAM3=0;
b_arg_param1=0;
b_arg_param2=0;
b_arg_param3=0;
b_arg_functin=0;
b_arg_bus=0;
b_arg_addr=0;

for arg in $@
do
	if [ $b_arg_functin -eq 1 ]; then
		b_arg_functin=0;
		FUNCTION=$arg;
		if [ -z $FUNCTION ]; then
			echo "[error] FUNCTION is null"
			exit;
		fi
	fi
	if [ $b_arg_param1 -eq 1 ] ; then
		b_arg_param1=0;
		PARAM1=$arg;
	fi

	if [ $b_arg_param2 -eq 1 ] ; then
		b_arg_param2=0;
		PARAM2=$arg;
	fi
    if [ $b_arg_param3 -eq 1 ] ; then
		b_arg_param3=0;
		PARAM3=$arg;
	fi
	if [ $b_arg_bus -eq 1 ] ; then
		b_arg_bus=0;
		I2C_DEV=$arg;
	fi
    if [ $b_arg_addr -eq 1 ] ; then
		b_arg_addr=0;
		I2C_ADDR=$arg;
	fi
	case $arg in
		"-r")
			MODE=read;
			;;
		"-w")
			MODE=write;
			;;
		"-f")			
			b_arg_functin=1;
			;;
		"-p1")
			b_arg_param1=1;
			;;
		"-p2")
			b_arg_param2=1;
			;;
        "-p3")
			b_arg_param3=1;
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
	esac
done
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
	Manufacturer=$(./i2c_lread $I2C_DEV $I2C_ADDR $Manufacturer_Name 2>/dev/null);
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
	model=$(./i2c_lread $I2C_DEV $I2C_ADDR $Model_Name 2>/dev/null);
    case $model in
    "821")
        printf "model is VEYE_MIPI_IMX335\n";
    ;;
    "1045")
        printf "model is VEYE_MIPI_IMX415\n";
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
	version=$(./i2c_lread $I2C_DEV $I2C_ADDR $Device_Version 2>/dev/null);
    printf "control version is %02x.%02x , isp version %02x.%02x\n" $(($version>>24&0xFF)) $(($version>>16 &0xFF)) $(($version>>8&0xFF)) $(($version&0xFF));
}

read_serialnum()
{
    local serialnum=0;
    typeset -i serialnum;
	serialnum=$(./i2c_lread $I2C_DEV $I2C_ADDR $Serial_Num 2>/dev/null);
    printf "serial number is 0x%08X\n" $serialnum;
}

read_timestamp()
{
    local timestamp=0;
    typeset -i timestamp;
	timestamp=$(./i2c_lread $I2C_DEV $I2C_ADDR $Time_stamp 2>/dev/null);
    printf "timestamp is %d \n" $timestamp;
}

read_errcode()
{
    local errcode=0;
    typeset -i errcode;
	errcode=$(./i2c_lread $I2C_DEV $I2C_ADDR $Error_Code 2>/dev/null);
    printf "errcode is %x \n" $errcode;
}

write_factoryparam()
{
    local res=0;
	res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $System_reset 0x01);
    printf "w factoryparam,all param will reset\n";
}

write_paramsave()
{
    local res=0;
	res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $Param_save_to_flash 0x01);
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $ISP_PARAM_SAVE 0x01);
    printf "all param saved to flash\n";
}

write_reboot()
{
    local res=0;
	res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $System_reboot 0x01);
    printf "system reboot!\n";
}

read_i2caddr()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Addr 2>/dev/null);
    printf "r i2c addr is 0x%02x \n" $value;
}
write_i2caddr()
{
    local res=0;
	res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Addr $PARAM1);
    printf "w i2c addr is 0x%02x \n" $PARAM1;
}

read_i2cport()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Port_Sel 2>/dev/null);
    printf "r i2c port is %x \n" $value;
}

write_i2cport()
{
    local res=0;
	res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Port_Sel $PARAM1);
    printf "w i2c port is %x \n" $PARAM1;
}

read_videomode()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_lread $I2C_DEV $I2C_ADDR $Video_Mode 2>/dev/null);
    printf "r videomode is %x \n" $value;
}

write_videomode()
{
    local res=0;
	res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $Video_Mode $PARAM1);
    printf "w videomode is %x \n" $PARAM1;
}

read_daynightmode()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_lread $I2C_DEV $I2C_ADDR $Day_Night_Mode 2>/dev/null);
    printf "r daynightmode is %x \n" $value;
}

write_daynightmode()
{
    local res=0;
	res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $Day_Night_Mode $PARAM1);
    printf "w daynightmode is %x \n" $PARAM1;
}

read_ircutdir()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_lread $I2C_DEV $I2C_ADDR $IRCUT_Dir 2>/dev/null);
    printf "r IRCUT Dir is %x \n" $value;
}

write_ircutdir()
{ 
    local res=0;
	res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $IRCUT_Dir $PARAM1);
    printf "w IRCUT Dir is %x \n" $PARAM1;
}

read_irtrigger()
{
    local value=0;
    typeset -i value;
	value=$(./i2c_lread $I2C_DEV $I2C_ADDR $Day_Night_Trgpin_Plr 2>/dev/null);
    printf "r daynight trig pin polarity is %x \n" $value;
}

write_irtrigger()
{
    local res=0;
	res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $Day_Night_Trgpin_Plr $PARAM1);
    printf "w daynight trig pin polarity %x \n" $PARAM1;
}

read_test_pattern()
{
    local res=0;
    local value=0;
    local enable=0;
    local patterntype=0;
    typeset -i enable;
    typeset -i patterntype;
    typeset -i value;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $TEST_PATTERN);
    sleep 0.01;
    value=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    enable=$(((value>>7)&0x1));
    patterntype=$((value&0x7));
    printf "r test pattern enable %d type %d\n" $enable $patterntype;
}

write_test_pattern()
{
    local value=0;
    local res=0;
    typeset -i value;
    value=$(((PARAM1<<7)|PARAM2));
	res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $TEST_PATTERN $value);
    printf "w test pattern enable %d type %d\n" $PARAM1 $PARAM2;
}

read_awb_mode()
{
    local value=0;
    local res=0;
    typeset -i value;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $WB_MODE);
    sleep 0.01;
    value=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    printf "r awb mode %x \n" $value;
}

write_awb_mode()
{
    local res=0;
	res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $WB_MODE $PARAM1);
    printf "w awb mode %x \n" $PARAM1;
}

read_antiflicker()
{
    local res=0;
    local value=0;
    typeset -i value;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $ANTI_FLICKER);
    sleep 0.01;
    value=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    value=$(((value>>4)&1));
    printf "r anti flicker %x \n" $value;
}

write_antiflicker()
{
    local res=0;
    local value=0;
    local antival=0;
    typeset -i value;
    
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $ANTI_FLICKER);
    sleep 0.01;
    value=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    
    antival=$(((PARAM1&0x1)<<4));
    value=$(((value&0xF)|antival));
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $ANTI_FLICKER $value);
    printf "w anti flicker %x\n" $PARAM1;
}

read_agc_max()
{
    local res=0;
    local value=0;
    typeset -i value;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $AGC_MAX);
    sleep 0.01;
    value=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    value=$((value&0xF));
    printf "r agx max %x \n" $value;
}

write_agc_max()
{
    local res=0;
    local value=0;
    local agc=0;
    typeset -i value;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $AGC_MAX);
    sleep 0.01;
    value=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    agc=$((PARAM1&0xf));
    value=$(((value&0x10)|agc));
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $AGC_MAX $value);
    printf "w agc max %x \n" $PARAM1;
}

read_ae_target()
{
    local videoformat=0;
    typeset -i videoformat;
	videoformat=$(./i2c_lread $I2C_DEV $I2C_ADDR $Video_Mode 2>/dev/null);
    case $videoformat in
    1|4)
        printf "Video Format is NTSC(60Hz) \n";
        res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $NTSC_AE_TARGET);
    ;;
    2|3)
        printf "Video Format is PAL(50Hz) \n";
        res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $PAL_AE_TARGET);
    ;;
    esac
    sleep 0.01;
    value=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    printf "r ae target is 0x%x \n" $value;
}

write_ae_target()
{
    local videoformat=0;
    typeset -i videoformat;
	videoformat=$(./i2c_lread $I2C_DEV $I2C_ADDR $Video_Mode 2>/dev/null);
    case $videoformat in
    1|4)
        printf "Video Format is NTSC(60Hz) \n";
        res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $NTSC_AE_TARGET $PARAM1);
    ;;
    2|3)
        printf "Video Format is PAL(50Hz) \n";
        res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $PAL_AE_TARGET $PARAM1);
    ;;
    esac
    printf "w ae target is 0x%x \n" $PARAM1;
}

read_ae_speed()
{
    local res=0;
    local value=0;
    typeset -i value;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $AE_SPEED);
    sleep 0.01;
    value=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    printf "r ae speed 0x%x \n" $value;
}

write_ae_speed()
{
    local res=0;
    local value=0;
    typeset -i value;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $AE_SPEED $PARAM1);
    printf "w ae speed 0x%x \n" $PARAM1;
}

read_agc_speed()
{
    local res=0;
    local value=0;
    typeset -i value;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $AGC_SPEED);
    sleep 0.01;
    value=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    printf "r agc speed %x \n" $value;
}

write_agc_speed()
{
    local res=0;
    local value=0;
    typeset -i value;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $AGC_SPEED $PARAM1);
    printf "w agc speed 0x%x \n" $PARAM1;
}

read_exp_val()
{
    local res=0;
    local value_m=0;
    local value_l=0;
    local value=0;
    typeset -i value_m;
    typeset -i value_l;
    typeset -i value;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $SHUTTER_VAL_M);
    sleep 0.01;
    value_m=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $SHUTTER_VAL_L);
    sleep 0.01;
    value_l=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    value=$(((value_m<<8)+value_l));
    printf "r current exposure val %d \n" $value;
}

read_agc_val()
{
    local res=0;
    local value=0;
    typeset -i value;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $AGC_LEVEL);
    sleep 0.01;
    value=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    printf "r current agc level %x \n" $value;
}

read_mshutter()
{
    local res=0;
    local value=0;
    typeset -i value;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $M_SHUTTER);
    sleep 0.01;
    value=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    printf "r mshutter %x \n" $value;
}

write_mshutter()
{
    local res=0;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $M_SHUTTER $PARAM1);
    printf "w mshutter %x \n" $PARAM1;
}

read_mirrormode()
{
    local res=0;
    local value=0;
    typeset -i value;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $MIRROR_FLIP);
    sleep 0.01;
    value=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    printf "r mirrormode %x \n" $value;
}

write_mirrormode()
{
    local res=0;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $MIRROR_FLIP $PARAM1);
    printf "w mirrormode %x \n" $PARAM1;
}

read_denoise_2d()
{
    local res=0;
    local value=0;
    typeset -i value;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $DENOISE_2D);
    sleep 0.01;
    value=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    printf "r 2d denoise %x \n" $value;
}

write_denoise_2d()
{
    local res=0;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $DENOISE_2D $PARAM1);
    printf "w 2d denoise %x \n" $PARAM1;
}

read_denoise_3d()
{
    local res=0;
    local value=0;
    typeset -i value;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $DENOISE_3D);
    sleep 0.01;
    value=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    printf "r 3d denoise %x \n" $value;
}

write_denoise_3d()
{
    local res=0;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $DENOISE_3D $PARAM1);
    printf "w 3d denoise %x \n" $PARAM1;
}

read_sharppen()
{
    local res=0;
    local value=0;
    typeset -i value;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $SHARP_MODE);
    sleep 0.01;
    value=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    printf "r sharppen %x \n" $value;
}

write_sharppen()
{
    local res=0;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR  $SHARP_MODE $PARAM1);
    printf "w sharppen %x \n" $PARAM1;
}

read_saturation()
{
    local res=0;
    local value_b=0;
    local value_r=0;
    typeset -i value_b;
    typeset -i value_r;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $SATURATION_B);
    sleep 0.01;
    value_b=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $SATURATION_R);
    sleep 0.01;
    value_r=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    printf "r saturation b 0x%x r 0x%x \n" $value_b $value_r;
}

write_saturation()
{
    local res=0;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $SATURATION_B $PARAM1);
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $SATURATION_R $PARAM1);
    printf "w saturation %x \n" $PARAM1;
}

read_extra_color_gain()
{
    local res=0;
    local value_b=0;
    local value_r=0;
    typeset -i value_b;
    typeset -i value_r;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $EXTRA_GAIN_B);
    sleep 0.01;
    value_b=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $EXTRA_GAIN_R);
    sleep 0.01;
    value_r=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    printf "r extra color gain b 0x%x r 0x%x \n" $value_b $value_r;
}

write_extra_color_gain()
{
    local res=0;
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $EXTRA_GAIN_B $PARAM1);
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $EXTRA_GAIN_R $PARAM1);
    printf "w saturation %x \n" $PARAM1;
}

read_sensor_reg()
{
    local addr=0;
    local value=0;
    typeset -i value;
    typeset -i addr;
    addr=$((SENSOR_ADDR_BASAE|PARAM1));
    res=$(./i2c_lwrite $I2C_DEV $I2C_ADDR $I2C_Special_ADDR $addr);
    sleep 0.01;
    value=$(./i2c_lread $I2C_DEV $I2C_ADDR $I2C_Special_RETVAL 2>/dev/null);
    printf "r sensor addr 0x%04x val 0x%02x\n" $PARAM1 $value;
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
        "serialnum")
            read_serialnum;
            ;;
        "timestamp")
            read_timestamp;
            ;;
        "i2caddr")
            read_i2caddr;
            ;;
        "i2cport")
            read_i2cport;
            ;;
        "videomode")
            read_videomode;
            ;;
        "daynightmode")
            read_daynightmode;
            ;;
		"ircutdir")
			read_ircutdir;
			;;
		"irtrigger")
			read_irtrigger;
			;;
        "sensor_reg")
			read_sensor_reg;
			;;
        "test_pattern")
			read_test_pattern;
			;;
        "awb_mode")
			read_awb_mode;
			;;
        "antiflicker")
			read_antiflicker;
			;;
        "agc_max")
			read_agc_max;
			;;
        "ae_target")
			read_ae_target;
			;;
        "ae_speed")
			read_ae_speed;
			;;
        "agc_speed")
			read_agc_speed;
			;;
        "exp_val")
			read_exp_val;
			;;
        "agc_val")
			read_agc_val;
			;;
        "mshutter")
			read_mshutter;
			;;
        "mirrormode")
			read_mirrormode;
			;;
        "denoise_2d")
			read_denoise_2d;
			;;
        "denoise_3d")
			read_denoise_3d;
			;;
        "sharppen")
			read_sharppen;
			;;
        "sharppen")
			read_sharppen;
			;;
        "saturation")
            read_saturation;
            ;;
        "extra_color_gain")
            read_extra_color_gain;
            ;;
        "errcode")
            read_errcode;
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
        "videomode")
            write_videomode;
            ;;
        "daynightmode")
            write_daynightmode;
            ;;
        "ircutdir")
			write_ircutdir;
			;;
		"irtrigger")
			write_irtrigger;
			;;
        "test_pattern")
			write_test_pattern;
			;;
        "awb_mode")
			write_awb_mode;
			;;
        "antiflicker")
			write_antiflicker;
			;;
        "agc_max")
			write_agc_max;
			;;
        "ae_target")
			write_ae_target;
			;;
        "ae_speed")
			write_ae_speed;
			;;
        "agc_speed")
			write_agc_speed;
			;;
        "mshutter")
			write_mshutter;
			;;
        "mirrormode")
			write_mirrormode;
			;;
        "denoise_2d")
			write_denoise_2d;
			;;
        "denoise_3d")
			write_denoise_3d;
			;;
        "sharppen")
			write_sharppen;
			;;
        "saturation")
            write_saturation;
            ;;
        "extra_color_gain")
            write_extra_color_gain;
            ;;
        *)
        echo "NOT SUPPORTED!";
        ;;
    esac
fi
