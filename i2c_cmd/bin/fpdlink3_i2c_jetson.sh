#!/bin/sh

<<'COMMENT_SAMPLE'


COMMENT_SAMPLE
#
#/-------------------\        /----------------------\        /--------------------\        /------------------\
#| MIPI CSI RECEIVER |        | FPDLINK DESERIALIZER |        | FPDLINK SERIALIZER |        | MIPI CSI CAMERA  |
#+-------------------+  CSI   +----------------------+        +--------------------+  CSI   +------------------+
#|                   | <----- |                      |  Coax  |                    | <----- |                  |
#| Raspberry Pi (4)  |  GPIO  | fpdlink2raspi        | <----> | piCam2fpdlink      |  GPIO  | Raspberry Pi     |
#| Jetson Nano       | <----> | PCB                  |        | PCB                | <----> | Camera Module v2 |
#| ...               |   I2C  | ds90ub954            |        | ds90ub953          |   I2C  |                  |
#|                   | <----> |                      |        |                    | <----> |                  |
#\-------------------/        \----------------------/        \--------------------/        \------------------/
#   ID ADDR (7BIT)               0x30                               0x18                     0x3B(VEYE CAMERA)
#   ID ADDR (8BIT)               0x60                               0x30                     0x76(VEYE CAMERA)
#   ALIAS ADDR (7BIT)             NA                                0x19                     0x3B(VEYE CAMERA)
#   ALIAS ADDR (8BIT)             NA                                0x32                     0x76(VEYE CAMERA)
# 
#
I2C_DEV=0;

SER_ID_PORT0=0x19;
SER_ALAIS_ID_PORT0=0x32;
VEYE_CAM_ID_PORT0=0x3B;

SER_ID_PORT1=0x20;
SER_ALAIS_ID_PORT1=0x40;
VEYE_CAM_ID_PORT1=0x3C;

DES_ID=0x30;

FPDLINK_PORT=0;

print_usage()
{
	echo "Usage:  ./fpdlink3_i2c.sh [-f] function name -b bus -p port -p1 param1 -p2 param2 -p3 param3 "
	echo "options:"
	echo "    -f [function name]       function name"
    echo "    -b [i2c bus num] 		   i2c bus number"
    echo "    -p [fpdlink port] 	   fpd link port number [0,1]"
	echo "    -p1 [param1] 			   param1 of each function"
	echo "    -p2 [param1] 			   param2 of each function"
	echo "    -p3 [param1] 			   param3 of each function"
	

    echo "support functions: sync_init,trigger_init"
}

######################reglist###################################
########################this is ses reglist#####################

########################this is des reglist#####################

######################parse arg###################################
MODE=write;
FUNCTION=version;
PARAM1=0;
PARAM2=0;
PARAM3=0;
b_arg_param1=0;
b_arg_param2=0;
b_arg_param3=0;
b_arg_functin=0;
b_arg_bus=0;
b_arg_port=0;
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
    if [ $b_arg_port -eq 1 ] ; then
		b_arg_port=0;
		FPDLINK_PORT=$arg;
	fi
	case $arg in
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
        "-p")
			b_arg_port=1;
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

#######################Action# BEGIN##############################

## init for ward channel, port 0 ses gpio 1 to des gpio 1
forward_port0_pin1_init()
{
    # select port 0
    i2cset -f -y $I2C_DEV $DES_ID 0x4c 0x01 
    i2cset -f -y $I2C_DEV $DES_ID 0x58 0x5e 
    ## set ser chip io 1 input
    i2cset -f -y $I2C_DEV $SER_ID_PORT0 0x0D 0x00 
    i2cset -f -y $I2C_DEV $SER_ID_PORT0 0x0E 0x02
    ## forward 2 gpio chn 0 1
    i2cset -f -y $I2C_DEV $SER_ID_PORT0 0x33 0x02

    ## set des chip io 1 
    #io 1 set to output
    i2cset -f -y $I2C_DEV $DES_ID 0x0F 0x02
    # pull up
    i2cset -f -y $I2C_DEV $DES_ID 0xBE 0x3F
    #gpio  1 output from sers 1
    #i2cset -f -y $I2C_DEV $DES_ID 0x10 0x01
    i2cset -f -y $I2C_DEV $DES_ID 0x11 0x21
}

## init backward channel, des gpio 1 to port 0 ses gpio 1
backward_port0_pin1_init()
{
    # select port 0
    i2cset -f -y $I2C_DEV $DES_ID 0x4c 0x01 
    i2cset -f -y $I2C_DEV $DES_ID 0x58 0x5e 

    ## set ser chip io  1 output
    i2cset -f -y $I2C_DEV $SER_ID_PORT0 0x0E 0x20
    i2cset -f -y $I2C_DEV $SER_ID_PORT0 0x0D 0x20
    
    ## 
    i2cset -f -y $I2C_DEV $SER_ID_PORT0 0x33 0x00
    
    #gpio0 1 input connect to port0 gpio 0 1
    i2cset -f -y $I2C_DEV $DES_ID 0x6E 0x10
    ## control the io using i2c cmd
    #i2cset -f -y $I2C_DEV $DES_ID 0x6E 0x88
    #i2cset -f -y $I2C_DEV $DES_ID 0x6E 0x99
    ## set des chip io 0 1 
    #io 0 1 set to input
    i2cset -f -y $I2C_DEV $DES_ID 0x0F 0x0F
    # pull up
    #i2cset -f -y $I2C_DEV $DES_ID 0xBE 0x3F
}

## init backward channel, des gpio 4 to port 1 ses gpio 1
backward_port1_pin1_init()
{
    # select port 1
    i2cset -f -y $I2C_DEV $DES_ID 0x4c 0x12 
    i2cset -f -y $I2C_DEV $DES_ID 0x58 0x5e 

    ## set ser chip io 1 output
    i2cset -f -y $I2C_DEV $SER_ID_PORT1 0x0E 0x20
    i2cset -f -y $I2C_DEV $SER_ID_PORT1 0x0D 0x20 

    #gpio 3 input connect to port1,gpio  1
    i2cset -f -y $I2C_DEV $DES_ID 0x6E 0x30
    ## set des chip io  1 
    #io  3 set to input
    i2cset -f -y $I2C_DEV $DES_ID 0x0F 0x0F
    # pull up
    #i2cset -f -y $I2C_DEV $DES_ID 0xBE 0x3F
}
## init backward channel, des gpio 0 to port 0 ses gpio 0
backward_port0_pin0_init()
{
    # select port 0
    i2cset -f -y $I2C_DEV $DES_ID 0x4c 0x01 
    i2cset -f -y $I2C_DEV $DES_ID 0x58 0x5e 

    ## set ser chip io 0 1 output
    i2cset -f -y $I2C_DEV $SER_ID_PORT0 0x0E 0x10
    i2cset -f -y $I2C_DEV $SER_ID_PORT0 0x0D 0x10
    
    ## 
    i2cset -f -y $I2C_DEV $SER_ID_PORT0 0x33 0x00
    
    #gpio0 input connect to port0 gpio 0
    i2cset -f -y $I2C_DEV $DES_ID 0x6E 0x00
    ## control the io using i2c cmd
    #i2cset -f -y $I2C_DEV $DES_ID 0x6E 0x88
    #i2cset -f -y $I2C_DEV $DES_ID 0x6E 0x99
    ## set des chip io 0 1 
    #io 0 1 set to input
    i2cset -f -y $I2C_DEV $DES_ID 0x0F 0x0F
    # pull up
    #i2cset -f -y $I2C_DEV $DES_ID 0xBE 0x3F
}

## init backward channel, des gpio 2  to port 1 ses gpio 0
backward_port1_pin0_init()
{
    # select port 1
    i2cset -f -y $I2C_DEV $DES_ID 0x4c 0x12 
    i2cset -f -y $I2C_DEV $DES_ID 0x58 0x5e 

    ## set ser chip io 0 1 output
    i2cset -f -y $I2C_DEV $SER_ID_PORT1 0x0E 0x10
    i2cset -f -y $I2C_DEV $SER_ID_PORT1 0x0D 0x10 

    #gpio2  input connect to port1,gpio 0
    i2cset -f -y $I2C_DEV $DES_ID 0x6E 0x02
    ## set des chip io 0 1 
    #io 2 3 set to input
    i2cset -f -y $I2C_DEV $DES_ID 0x0F 0x0F
    # pull up
    #i2cset -f -y $I2C_DEV $DES_ID 0xBE 0x3F
}
write_sync_init()
{
    printf "usage: p1, role,0 is master,1 is slave\n"
    
    if [ $FPDLINK_PORT -eq 1 ] ; then
        if [ $PARAM1 -eq 1 ] ; then
            backward_port1_pin1_init;
            printf "init fpdlink sync mode port %d role as SLAVE! \n" $FPDLINK_PORT;
        else
            printf "do not support use port %d as MASTER! \n" $FPDLINK_PORT ;
        fi
    else
        if [ $PARAM1 -eq 1 ] ; then
            backward_port0_pin1_init;
            printf "init fpdlink sync mode port %d role as SLAVE! \n" $FPDLINK_PORT;
        else
            forward_port0_pin1_init;
            printf "init fpdlink sync mode port %d role as MASTER!\n" $FPDLINK_PORT;
        fi
    fi
    #gpio2 3 input connect to port0; 2 3 to port 2 3 
    #i2cset -f -y $I2C_DEV $DES_ID 0x6F 0x32
}

write_trigger_init()
{
    if [ $FPDLINK_PORT -eq 1 ] ; then
        backward_port1_pin0_init;
        printf "init fpdlink trigger mode port %d  \n" $FPDLINK_PORT;
    else
        backward_port0_pin0_init;
        printf "init fpdlink trigger mode port %d  \n" $FPDLINK_PORT;
    fi
}

if [ ${MODE} = "write" ] ; then
	case $FUNCTION in
		"sync_init")
			write_sync_init;
			;;
		"trigger_init")
			write_trigger_init;
			;;
        *)
			echo "NOT SUPPORTED!";
			;;
	esac
fi

