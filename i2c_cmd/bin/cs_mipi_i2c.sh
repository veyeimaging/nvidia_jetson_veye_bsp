
#!/bin/sh

<<'COMMENT_SAMPLE'
sudo ./cs_mipi_i2c.sh -r -f devid
sudo ./cs_mipi_i2c.sh -r -f firmwarever
sudo ./cs_mipi_i2c.sh -r -f productmodel
sudo ./cs_mipi_i2c.sh -r -f videofmtcap
sudo ./cs_mipi_i2c.sh -r -f videofmt
sudo ./cs_mipi_i2c.sh -r -f ispcap
sudo ./cs_mipi_i2c.sh -r -f powerhz


sudo ./cs_mipi_i2c.sh -w -f videofmt -p1 1280 -p2 720 -p3 60
sudo ./cs_mipi_i2c.sh -w -f powerhz -p1 50
sudo ./cs_mipi_i2c.sh -w -f sysreset
sudo ./cs_mipi_i2c.sh -w -f paramsave

COMMENT_SAMPLE
I2C_DEV=6;
I2C_ADDR=0x3b;

print_usage()
{
    echo "this shell scripts should be used for CS-MIPI-IMX307 !"
	echo "Usage:  ./cs_mipi_i2c.sh [-r/w] [-f] function name -p1 param1 -p2 param2 -p3 param3 -b bus"
	echo "options:"
	echo "    -r                       read "
	echo "    -w                       write"
	echo "    -f [function name]       function name"
	echo "    -p1 [param1] 			   param1 of each function"
	echo "    -p2 [param1] 			   param2 of each function"
	echo "    -p3 [param1] 			   param3 of each function"
	echo "    -b [i2c bus num] 		   i2c bus number"
}

######################reglist###################################
########################this is fpga reglist#####################
deviceID=0x00;
HardWare=0x01;
Csi2_Enable=0x03;
TriggerMode=0x10;
SlaveMode=0x11;


########################this is isp mcu reglist#####################
ARM_VER_L=0x0100;
ARM_VER_H=0x0101;
PRODUCTID_L=0x0102;
PRODUCTID_H=0x0103;
SYSTEM_RESET=0x0104;
PARAM_SAVE=0x0105;
VIDEOFMT_CAP=0x0106; 
VIDEOFMT_NUM=0x0107;
FMTCAP_WIDTH_L=0x0108;
FMTCAP_WIDTH_H=0x0109;
FMTCAP_HEIGHT_L=0x010A;
FMTCAP_HEIGHT_H=0x010B;
FMTCAP_FRAMRAT_L=0x010C;
FMTCAP_FRAMRAT_H=0x010D;

FMT_WIDTH_L=0x0180;
FMT_WIDTH_H=0x0181;
FMT_HEIGHT_L=0x0182;
FMT_HEIGHT_H=0x0183;
FMT_FRAMRAT_L=0x0184;
FMT_FRAMRAT_H=0x0185;

ISP_CAP_L=0x0200;
ISP_CAP_M=0x0201;
ISP_CAP_H=0x0202;
ISP_CAP_E=0x0203;
POWER_HZ=0x0204;
    
######################parse arg###################################
MODE=read;
FUNCTION=version;
PARAM1=0;
PARAM2=0;
PARAM3=0;
b_arg_param1=0;
b_arg_param2=0;
b_arg_param3=0;
b_arg_functin=0;
b_arg_bus=0;


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

pinmux()
{
	sh ./camera_i2c_config >> /dev/null 2>&1
}

read_hardwareid()
{
	local verid=0;
    local hardver=0;
	local res=0;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  $deviceID );
	verid=$?;
    res=$(./i2c_read $I2C_DEV $I2C_ADDR  $HardWare );
	hardver=$?;
	printf "hardwareid is 0x%2x%2x \n" $verid $hardver;
}

read_firmware_ver()
{
	local firmwarever_l=0;
    local firmwarever_h=0;
	local res=0;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  $ARM_VER_L );
	firmwarever_l=$?;
    res=$(./i2c_read $I2C_DEV $I2C_ADDR  $ARM_VER_H );
	firmwarever_h=$?;
    printf "r firmware version is %2d.%02d\n" $firmwarever_h $firmwarever_l;
}

read_productmodel()
{
	local productmodel_l=0;
    local productmodel_h=0;
    local productmodel;
	local res=0;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  $PRODUCTID_L );
	productmodel_l=$?;
    res=$(./i2c_read $I2C_DEV $I2C_ADDR  $PRODUCTID_H );
	productmodel_h=$?;
    
    productmodel=$((productmodel_h*256+productmodel_l));
    case $productmodel in
    "55")
        #0x37
        printf "PRODUCT MODEL is CS-MIPI-IMX307\n";
    ;;
    "56")
        #0x38
        printf "PRODUCT MODEL is CS-LVDS-IMX307\n";
    ;;
    "306")
        #0x132
        printf "PRODUCT MODEL is CS-MIPI-SC132\n";
    ;;
    "307")
        #0x133
        printf "PRODUCT MODEL is CS-LVDS-SC132\n";
    ;;
    *)
     printf "r product model is 0x%04x not recognized\n" $productmodel;
     ;;
    esac
}

read_videofmtcap()
{
    local fmtnum=0;
    local width=0;
    local height=0;
    local framerate=0;
    local data_l=0;
    local data_h=0;
	local res=0;
    res=$(./i2c_read $I2C_DEV $I2C_ADDR  $VIDEOFMT_NUM);
	fmtnum=$?;
	printf "camera support videofmt num %d\n" $fmtnum;
    local i=0
    while(( $i<$fmtnum ))
    do
        res=$(./i2c_read $I2C_DEV $I2C_ADDR $(($FMTCAP_WIDTH_L+$i*6)));
        data_l=$?;
        res=$(./i2c_read $I2C_DEV $I2C_ADDR $(($FMTCAP_WIDTH_H+$i*6)));
        data_h=$?;
        width=$((data_h*256+data_l));
        
        res=$(./i2c_read $I2C_DEV $I2C_ADDR $(($FMTCAP_HEIGHT_L+$i*6)));
        data_l=$?;
        res=$(./i2c_read $I2C_DEV $I2C_ADDR $(($FMTCAP_HEIGHT_H+$i*6)));
        data_h=$?;
        height=$((data_h*256+data_l));
        
        res=$(./i2c_read $I2C_DEV $I2C_ADDR $(($FMTCAP_FRAMRAT_L+$i*6)));
        data_l=$?;
        res=$(./i2c_read $I2C_DEV $I2C_ADDR $(($FMTCAP_FRAMRAT_H+$i*6)));
        data_h=$?;
        framerate=$((data_h*256+data_l));
        printf "r videofmtcap num %d width %d height %d framerate %d\n" $(($i+1)) $width $height $framerate;
        let "i++"
    done
    
}

read_videofmt()
{
    local width=0;
    local height=0;
    local framerate=0;
    local data_l=0;
    local data_h=0;
	local res=0;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  $FMT_WIDTH_L);
	data_l=$?;
    res=$(./i2c_read $I2C_DEV $I2C_ADDR  $FMT_WIDTH_H);
	data_h=$?;
    width=$((data_h*256+data_l));
    
    res=$(./i2c_read $I2C_DEV $I2C_ADDR  $FMT_HEIGHT_L);
	data_l=$?;
    res=$(./i2c_read $I2C_DEV $I2C_ADDR  $FMT_HEIGHT_H);
	data_h=$?;
    height=$((data_h*256+data_l));
    
    res=$(./i2c_read $I2C_DEV $I2C_ADDR  $FMT_FRAMRAT_L);
	data_l=$?;
    res=$(./i2c_read $I2C_DEV $I2C_ADDR  $FMT_FRAMRAT_H);
	data_h=$?;
    framerate=$((data_h*256+data_l));
    printf "r videofmt width %d height %d framerate %d\n" $width $height $framerate;
}

write_videofmt()
{
    local width=0;
    local height=0;
    local framerate=0;
    local data_l=0;
    local data_h=0;
    local res=0;
    width=$PARAM1;
    height=$PARAM2;
    framerate=$PARAM3;
    data_h=$((width/256));
    data_l=$((width%256));
	res=$(./i2c_write $I2C_DEV $I2C_ADDR $FMT_WIDTH_L $data_l);
    res=$(./i2c_write $I2C_DEV $I2C_ADDR $FMT_WIDTH_H $data_h);
    data_h=$((height/256));
    data_l=$((height%256));
	res=$(./i2c_write $I2C_DEV $I2C_ADDR $FMT_HEIGHT_L $data_l);
    res=$(./i2c_write $I2C_DEV $I2C_ADDR $FMT_HEIGHT_H $data_h);
    data_h=$((framerate/256));
    data_l=$((framerate%256));
	res=$(./i2c_write $I2C_DEV $I2C_ADDR $FMT_FRAMRAT_L $data_l);
    res=$(./i2c_write $I2C_DEV $I2C_ADDR $FMT_FRAMRAT_H $data_h);
    printf "w videofmt width %d height %d framerate %d\n" $width $height $framerate;
}

read_ispcap()
{
    local ispcap=0;
    local data_l=0;
    local data_m=0;
    local data_h=0;
    local data_e=0;
    local res=0;
    res=$(./i2c_read $I2C_DEV $I2C_ADDR  $ISP_CAP_L);
	data_l=$?;
    res=$(./i2c_read $I2C_DEV $I2C_ADDR  $ISP_CAP_M);
	data_m=$?;
    res=$(./i2c_read $I2C_DEV $I2C_ADDR  $ISP_CAP_H);
	data_h=$?;
    res=$(./i2c_read $I2C_DEV $I2C_ADDR  $ISP_CAP_E);
	data_e=$?;
    ispcap=$((data_e*256*256*256+data_h*256*256+data_m*256+data_l));
    printf "r ispcap 0x%x\n" $ispcap;
}

write_paramsave()
{
    local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  $PARAM_SAVE 0x1 );
    printf "w paramsave,all param will write to flash\n";
}

read_powerhz()
{
    local powerhz=0;
    local res=0;
    res=$(./i2c_read $I2C_DEV $I2C_ADDR  $POWER_HZ);
	powerhz=$?;
    printf "r powerhz is %d\n" $powerhz;
}
write_powerhz()
{
    local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  $POWER_HZ $PARAM1 );
    printf "w powerhz %d\n" $PARAM1;
}

write_sysreset()
{
    local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR $SYSTEM_RESET 0x01 );
    printf "w sysreset,all param will reset\n";
}


#######################Action# BEGIN##############################

pinmux;

if [ ${MODE} = "read" ] ; then
	case $FUNCTION in
		"devid"|"deviceid")
			read_hardwareid;
			;;
		"firmwarever")
			read_firmware_ver;
			;;
		"productmodel")
			read_productmodel;
			;;
		"videofmtcap")
			read_videofmtcap;
			;;
		"videofmt")
			read_videofmt;
			;;
		"ispcap")
			read_ispcap;
			;;
		"powerhz")
			read_powerhz;
			;;
        *)
			echo "NOT SUPPORTED!";
			;;
	esac
fi

if [ ${MODE} = "write" ] ; then
	case $FUNCTION in
		"sysreset")
			write_sysreset;
			;;
		"paramsave")
			write_paramsave;
			;;
		"videofmt")
			write_videofmt;
			;;
		"powerhz")
			write_powerhz;
			;;
        *)
			echo "NOT SUPPORTED!";
			;;
	esac
fi

