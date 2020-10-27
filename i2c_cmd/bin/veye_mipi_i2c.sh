
#!/bin/sh

#
#set camera i2c pin mux
#

I2C_DEV=6;
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
	echo "support functions: devid,hdver,wdrmode,videoformat,mirrormode,denoise,agc,lowlight,daynightmode,ircutdir,irtrigger¡ê?mshutter"
    echo "cameramode, notf, capture, csienable,saturation,wdrbtargetbr,wdrtargetbr, brightness ,contrast , sharppen, aespeed"
}
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

read_devid()
{
	local verid=0;
	local res=0;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x01 );
	verid=$?;
	printf "device id is 0x%2x\n" $verid;
}

read_hardver()
{
	local hardver=0;
	local res=0;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x00 );
	hardver=$?;
	printf "hardware version is 0x%2x\n" $hardver;
}

read_wdrmode()
{
	local wdrmode=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDB );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x32 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x01 );
    sleep 0.01;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x14 );
	wdrmode=$?;
	printf "r wdrmode is 0x%2x\n" $wdrmode;
}
write_wdrmode()
{
	local wdrmode=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDB );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x32 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x12 $PARAM1);
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x00 );
	printf "w wdrmode is 0x%2x\n" $PARAM1;
}

read_videoformat()
{
	local videoformat=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDE );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0xC2 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x01 );
    sleep 0.01;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x14 );
	videoformat=$?;
	echo "frame rate reg "$videoformat;
	if [ $videoformat -eq 1 ] ; then
		printf "r Video Format is NTSC(60Hz)\n";
	fi
	if [ $videoformat -eq 0 ] ; then
		printf "r Video Format is PAL(50Hz)\n";
	fi
}

write_videoformat()
{
	local videoformat=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDE );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0xC2 );
	if [ $PARAM1 = "PAL" ] ; then
		res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x12 0x0);
	fi
	if [ $PARAM1 = "NTSC" ] ; then
		res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x12 0x1);
	fi
	
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x00 );
	printf "w videoformat is %s\n" $PARAM1;
}

read_mirrormode()
{
	local mirrormode=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDE );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x57 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x01 );
    sleep 0.01;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x14 );
	mirrormode=$?;
	printf "r mirrormode is 0x%2x\n" $mirrormode;
}
write_mirrormode()
{
	local mirrormode=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDE );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x57 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x12 $PARAM1);
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x00 );
	printf "w mirrormode is 0x%2x\n" $PARAM1;
}

read_denoise()
{
	local denoise=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xD8 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x9B );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x01 );
    sleep 0.01;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x14 );
	denoise=$?;
	printf "r denoise is 0x%2x\n" $denoise;
}
write_denoise()
{
	local denoise=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xD8 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x9B );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x12 $PARAM1);
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x00 );
	printf "w denoise is 0x%2x\n" $PARAM1;
}

read_agc()
{
	local agc=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDA );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x67 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x01 );
    sleep 0.01;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x14 );
	agc=$?;
	printf "r agc is 0x%2x\n" $agc;
}
write_agc()
{
	local agc=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDA );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x67 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x12 $PARAM1);
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x00 );
	printf "w agc is 0x%2x\n" $PARAM1;
}

read_lowlight()
{
	local lowlight=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDA );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x64 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x01 );
    sleep 0.01;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x14 );
	lowlight=$?;
	printf "r lowlight is 0x%2x\n" $lowlight;
}
write_lowlight()
{
	local lowlight=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDA );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x64 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x12 $PARAM1);
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x00 );
	printf "w lowlight is 0x%2x\n" $PARAM1;
}

read_mshutter()
{
	local mshutter=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDA );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x66 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x01 );
    sleep 0.01;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x14 );
	mshutter=$?;
	printf "r mshutter is 0x%2x\n" $mshutter;
}
write_mshutter()
{
	local mshutter=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDA );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x66 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x12 $PARAM1);
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x00 );
	printf "w mshutter is 0x%2x\n" $PARAM1;
}

read_daynightmode()
{
	local daynightmode=0;
	local res=0;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x02 );
	daynightmode=$?;
	printf "r daynightmode is 0x%2x\n" $daynightmode;
}
write_daynightmode()
{
	local daynightmode=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x02 $PARAM1);
	printf "w daynightmode is 0x%2x\n" $PARAM1;
}
read_ircutdir()
{
	local ircutdir=0;
	local res=0;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x16 );
	ircutdir=$?;
	printf "r ircutdir is 0x%2x\n" $ircutdir;
}
write_ircutdir()
{
	local ircutdir=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x16 $PARAM1);
	printf "w ircutdir is 0x%2x\n" $PARAM1;
}

read_irtrigger()
{
	local irtrigger=0;
	local res=0;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x15 );
	irtrigger=$?;
	printf "r irtrigger is 0x%2x\n" $irtrigger;
}
write_irtrigger()
{
	local irtrigger=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR 0x15 $PARAM1);
	printf "w irtrigger is 0x%2x\n" $PARAM1;
}

read_cameramode()
{
	local cameramode=0;
	local res=0;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x1A );
	cameramode=$?;
	printf "r cameramode is 0x%2x\n" $cameramode;
}
write_cameramode()
{
	local cameramode=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR 0x1A $PARAM1);
	printf "w cameramode is 0x%2x\n" $PARAM1;
}

read_nodf()
{
	local nodf=0;
	local res=0;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x1B );
	nodf=$?;
	printf "r nodf is 0x%2x\n" $notf;
}

write_nodf()
{
	local nodf=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR 0x1B $PARAM1);
	printf "w nodf is 0x%2x\n" $PARAM1;
}

write_capture()
{
	local capture=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR 0x1C 0x1);
	printf "w capture\n" ;
}

read_csienable()
{
	local csienable=0;
	local res=0;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x1D );
	csienable=$?;
	printf "r csienable is 0x%2x\n" $csienable;
}

write_csienable()
{
	local csienable=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR 0x1D $PARAM1);
	printf "w csienable is 0x%2x\n" $PARAM1;
}


read_saturation()
{
	local saturation=0;
	local res=0;
    res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xD8 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x7A );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x01 );
    sleep 0.01;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x14 );
	saturation=$?;
	printf "r saturation is 0x%2x\n" $saturation;
}
write_saturation()
{
	local saturation=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xD8 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x7A );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x12 $PARAM1);
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x00 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xD8 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x7B );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x12 $PARAM1);
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x00 );
	printf "w saturation is 0x%2x\n" $PARAM1;
}

read_wdrbtargetbr()
{
	local wdrbtargetbr=0;
	local res=0;
    res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDA );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0xCA );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x01 );
    sleep 0.01;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x14 );
	wdrbtargetbr=$?;
	printf "r wdrbtargetbr is 0x%2x\n" $wdrbtargetbr;
}
write_wdrbtargetbr()
{
	local wdrbtargetbr=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDA );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0xCA );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x12 $PARAM1);
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x00 );
	printf "w wdrbtargetbr is 0x%2x\n" $PARAM1;
}
    
read_brightness()
{
    local videoformat=0;
    local brightness=0;
	local res=0;
    res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDE );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0xC2 );
    res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x01 );
    sleep 0.01;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x14 );
	videoformat=$?;
    sleep 0.01;
    if [ $videoformat -eq 1 ] ; then
        res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDA );
        res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x65 );
		printf "Video Format is NTSC(60Hz) \n";
	fi
	if [ $videoformat -eq 0 ] ; then
        res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDA );
        res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x1A );
		printf "Video Format is PAL(50Hz)\n";
	fi
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x01 );
    sleep 0.01;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x14 );
	brightness=$?;
	printf "r brightness is 0x%2x\n" $brightness;
}
write_brightness()
{
    local videoformat=0;
	local res=0;
    res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDE );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0xC2 );
    res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x01 );
    sleep 0.01;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x14 );
	videoformat=$?;
    sleep 0.01;
    if [ $videoformat -eq 1 ] ; then
        res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDA );
        res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x65 );
		printf "Video Format is NTSC(60Hz) \n";
	fi
	if [ $videoformat -eq 0 ] ; then
        res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDA );
        res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x1A );
		printf "Video Format is PAL(50Hz)\n";
	fi
    res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x12 $PARAM1 );
    res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x00 );
	printf "w brightness is 0x%2x\n" $PARAM1;
}


read_sharppen()
{
    local sharppen_enable=0;
    local sharppen_val=0;
	local res=0;
    res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xD9 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x5D );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x01 );
    sleep 0.01;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x14 );
	sharppen_enable=$?;
    sleep 0.01;
    res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xD9 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x52 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x01 );
    sleep 0.01;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x14 );
	sharppen_val=$(($?>>4));
	printf "r sharppen enable is 0x%2x val is 0x%2x\n" $sharppen_enable $sharppen_val;
}

write_sharppen()
{
    local sharppen_enable=0;
    local sharppen_val=0;
	local res=0;

	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xD9 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x5D );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x12 $PARAM1);
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x00 );
    
    sharppen_val=$(($PARAM2<<4|0x3));
    res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xD9 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x52 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x12 $sharppen_val);
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x00 );
	printf "w sharppen enable is 0x%2x val is 0x%2x \n" $PARAM1 $PARAM2;
}

read_aespeed()
{
    local agcspeed=0;
    local shutterspeed=0;
	local res=0;
    res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDA );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x18 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x01 );
    sleep 0.01;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x14 );
	agcspeed=$?;
    sleep 0.01;
    res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDA );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x1B );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x01 );
    sleep 0.01;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x14 );
	shutterspeed=$?;
	printf "r agcspeed is 0x%2x shutter speed is 0x%2x\n" $agcspeed $shutterspeed;
}
write_aespeed()
{
    local agcspeed=0;
    local shutterspeed=0;
	local res=0;

	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDA );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x18 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x12 $PARAM1);
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x00 );
    
    res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDA );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x1B );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x12 $PARAM2);
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x00 );
	printf "w agcspeed is 0x%2x shutterspeed is 0x%2x\n" $PARAM1 $PARAM2;
}

read_wdrtargetbr()
{
	local wdrtargetbr=0;
	local res=0;
    res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDA );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0xC1 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x01 );
    sleep 0.01;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x14 );
	wdrtargetbr=$?;
	printf "r wdrtargetbr is 0x%2x\n" $wdrtargetbr;
}
write_wdrtargetbr()
{
	local wdrtargetbr=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0xDA );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0xC1 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x12 $PARAM1);
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x00 );
	printf "w wdrtargetbr is 0x%2x\n" $PARAM1;
}

read_contrast()
{
	local contrast=0;
	local res=0;
    res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0x49 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x5B );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x01 );
    	sleep 0.01;
	res=$(./i2c_read $I2C_DEV $I2C_ADDR  0x14 );
	contrast=$?;
	printf "r contrast is 0x%2x\n" $contrast;
}
write_contrast()
{
	local contrast=0;
	local res=0;
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x10 0x49 );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x11 0x5B );
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x12 $PARAM1);
	res=$(./i2c_write $I2C_DEV $I2C_ADDR  0x13 0x00 );
	printf "w contrast is 0x%2x\n" $PARAM1;
}

#######################Action# BEGIN##############################

if [ `whoami` != "root" ];then
	echo "should be root!";
    exit 0;
fi

echo 100000 > /sys/bus/i2c/devices/i2c-$I2C_DEV/bus_clk_rate
./i2c_write $I2C_DEV $I2C_ADDR  0x07 0xFE&> /dev/null;

if [ ${MODE} = "read" ] ; then
	case $FUNCTION in
		"devid"|"deviceid")
			read_devid;
			;;
		"hdver"|"hardwareversion")
			read_hardver;
			;;
		"wdrmode")
			read_wdrmode;
			;;
		"videoformat")
			read_videoformat;
			;;
		"mirrormode")
			read_mirrormode;
			;;
		"denoise")
			read_denoise;
			;;
		"agc")
			read_agc;
			;;
		"lowlight")
			read_lowlight;
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
		"mshutter")
			read_mshutter;
			;;
        "cameramode")
			read_cameramode;
			;;
        "nodf")
			read_nodf;
			;;
        "csienable")
			read_csienable;
			;;
        "saturation")
			read_saturation;
			;;
        "wdrbtargetbr")
			read_wdrbtargetbr;
			;;
        "brightness")
			read_brightness;
			;;
        "aespeed")
			read_aespeed;
			;;
        "contrast")
            read_contrast;
            ;;
        "satu")
            read_satu;
            ;;
        "sharppen")
            read_sharppen;
            ;;
	"wdrtargetbr")
			read_wdrtargetbr;
			;;
	esac
fi



if [ ${MODE} = "write" ] ; then
	case $FUNCTION in
		"devid"|"deviceid")
			echo "NOT SUPPORTED!";
			;;
		"hdver"|"hardwareversion")
			echo "NOT SUPPORTED!";
			;;
		"wdrmode")
			write_wdrmode;
			;;
		"videoformat")
			write_videoformat;
			;;
		"mirrormode")
			write_mirrormode;
			;;
		"denoise")
			write_denoise;
			;;
		"agc")
			write_agc;
			;;
		"lowlight")
			write_lowlight;
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
		"mshutter")
			write_mshutter;
			;;
        "cameramode")
			write_cameramode;
			;;
        "nodf")
			write_nodf;
			;;
        "capture")
			write_capture;
			;;
        "csienable")
			write_csienable;
			;;
        "saturation")
			write_saturation;
			;;
        "wdrbtargetbr")
			write_wdrbtargetbr;
			;;
        "brightness")
			write_brightness;
			;;
        "aespeed")
			write_aespeed;
			;;
        "contrast")
            write_contrast;
            ;;
        "satu")
            write_satu;
            ;;
        "sharppen")
            write_sharppen;
            ;;
        "wdrtargetbr")
			write_wdrtargetbr;
			;;
	esac
fi


