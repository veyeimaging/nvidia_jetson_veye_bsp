#!/bin/bash
#
I2C_DEV=10;
I2C_ADDR=0x3b;

<<'COMMENT_SAMPLE'


./gx_mipi_i2c.sh -r manufacturer
./gx_mipi_i2c.sh -r model
./gx_mipi_i2c.sh -r sensorname
./gx_mipi_i2c.sh -r serialno
./gx_mipi_i2c.sh -r version
./gx_mipi_i2c.sh -w factoryparam
./gx_mipi_i2c.sh -w paramsave
./gx_mipi_i2c.sh -w reboot
./gx_mipi_i2c.sh -r timestamp


./gx_mipi_i2c.sh -r errcode
./gx_mipi_i2c.sh -r fmtcap
./gx_mipi_i2c.sh -r readmodecap
./gx_mipi_i2c.sh -r workmodecap
./gx_mipi_i2c.sh -r lanecap
./gx_mipi_i2c.sh -r cameramodel0
./gx_mipi_i2c.sh -r cameramodel1
./gx_mipi_i2c.sh -r cameramodel2
./gx_mipi_i2c.sh -r cameramodel3
./gx_mipi_i2c.sh -r cameramodel4
./gx_mipi_i2c.sh -r cameramodel5
./gx_mipi_i2c.sh -r cameramodel6
./gx_mipi_i2c.sh -r cameramodel7
./gx_mipi_i2c.sh -r temp
./gx_mipi_i2c.sh -r videomodecap
./gx_mipi_i2c.sh -r videomodenum
./gx_mipi_i2c.sh -r videomodewh1
./gx_mipi_i2c.sh -r videomodeparam1
./gx_mipi_i2c.sh -r videomodewh2
./gx_mipi_i2c.sh -r videomodeparam2
./gx_mipi_i2c.sh -r videomodewh3
./gx_mipi_i2c.sh -r videomodeparam3
./gx_mipi_i2c.sh -r videomodewh4
./gx_mipi_i2c.sh -r videomodeparam4
./gx_mipi_i2c.sh -r videomodewh5
./gx_mipi_i2c.sh -r videomodeparam5
./gx_mipi_i2c.sh -r videomodewh6
./gx_mipi_i2c.sh -r videomodeparam6
./gx_mipi_i2c.sh -r videomodewh7
./gx_mipi_i2c.sh -r videomodeparam7
./gx_mipi_i2c.sh -r videomodewh8
./gx_mipi_i2c.sh -r videomodeparam8

./gx_mipi_i2c.sh -w imgacq

./gx_mipi_i2c.sh -r workmode
./gx_mipi_i2c.sh -w workmode

./gx_mipi_i2c.sh -w trgsrc
./gx_mipi_i2c.sh -r trgsrc

./gx_mipi_i2c.sh -r trgnum
./gx_mipi_i2c.sh -w trgnum

./gx_mipi_i2c.sh -r trginterval
./gx_mipi_i2c.sh -w trginterval

./gx_mipi_i2c.sh -w trgone

./gx_mipi_i2c.sh -r trgcount
./gx_mipi_i2c.sh -w trgcount

./gx_mipi_i2c.sh -w i2caddr
./gx_mipi_i2c.sh -r i2caddr

./gx_mipi_i2c.sh -r nondiscontinuousmode
./gx_mipi_i2c.sh -w nondiscontinuousmode

./gx_mipi_i2c.sh -w slavemode
./gx_mipi_i2c.sh -r slavemode

./gx_mipi_i2c.sh -r framecount
./gx_mipi_i2c.sh -r triggercyclemin

./gx_mipi_i2c.sh -r daynightmode
./gx_mipi_i2c.sh -w daynightmode

./gx_mipi_i2c.sh -w ircutdir
./gx_mipi_i2c.sh -r ircutdir

./gx_mipi_i2c.sh -w pinpolarity
./gx_mipi_i2c.sh -r pinpolarity

./gx_mipi_i2c.sh -w ircuttimer
./gx_mipi_i2c.sh -r ircuttimer

./gx_mipi_i2c.sh -w testimg
./gx_mipi_i2c.sh -r testimg

./gx_mipi_i2c.sh -w pixelformat
./gx_mipi_i2c.sh -r pixelformat

./gx_mipi_i2c.sh -r maxwh
./gx_mipi_i2c.sh -r maxfps
./gx_mipi_i2c.sh -r curwh

./gx_mipi_i2c.sh -w imgdir
./gx_mipi_i2c.sh -r imgdir

./gx_mipi_i2c.sh -w videomode
./gx_mipi_i2c.sh -r videomode


./gx_mipi_i2c.sh -r readmode

./gx_mipi_i2c.sh -r lanenum

./gx_mipi_i2c.sh -r minwh
./gx_mipi_i2c.sh -r mipidatarate
./gx_mipi_i2c.sh -r minfps

./gx_mipi_i2c.sh -r fps
./gx_mipi_i2c.sh -w fps

./gx_mipi_i2c.sh -r expmode
./gx_mipi_i2c.sh -w expmode

./gx_mipi_i2c.sh -r aetarget
./gx_mipi_i2c.sh -w aetarget

./gx_mipi_i2c.sh -r aestrategy
./gx_mipi_i2c.sh -w aestrategy

./gx_mipi_i2c.sh -r metime
./gx_mipi_i2c.sh -w metime

./gx_mipi_i2c.sh -r aemaxtime
./gx_mipi_i2c.sh -w aemaxtime

./gx_mipi_i2c.sh -r exptime

./gx_mipi_i2c.sh -r mgain
./gx_mipi_i2c.sh -w mgain

./gx_mipi_i2c.sh -r aemaxgain
./gx_mipi_i2c.sh -w aemaxgain

./gx_mipi_i2c.sh -r curgain

./gx_mipi_i2c.sh -r wbmode
./gx_mipi_i2c.sh -w wbmode

./gx_mipi_i2c.sh -r awbcolortempmin
./gx_mipi_i2c.sh -w awbcolortempmin

./gx_mipi_i2c.sh -r awbcolortempmax
./gx_mipi_i2c.sh -w awbcolortempmax

./gx_mipi_i2c.sh -r mwbrgain
./gx_mipi_i2c.sh -w mwbrgain

./gx_mipi_i2c.sh -r mwbbgain
./gx_mipi_i2c.sh -w mwbbgain

./gx_mipi_i2c.sh -r colortemp
./gx_mipi_i2c.sh -r currgain
./gx_mipi_i2c.sh -r curbgain

./gx_mipi_i2c.sh -r aemintime
./gx_mipi_i2c.sh -w aemintime

./gx_mipi_i2c.sh -r gamma_index
./gx_mipi_i2c.sh -w gamma_index

./gx_mipi_i2c.sh -w antiflicker 1 60
./gx_mipi_i2c.sh -r antiflicker

./gx_mipi_i2c.sh -w wdrparam
./gx_mipi_i2c.sh -r wdrparam

./gx_mipi_i2c.sh -w sharppen
./gx_mipi_i2c.sh -r sharppen

./gx_mipi_i2c.sh -w denoise2d
./gx_mipi_i2c.sh -r denoise2d

./gx_mipi_i2c.sh -w denoise3d
./gx_mipi_i2c.sh -r denoise3d

./gx_mipi_i2c.sh -w saturation
./gx_mipi_i2c.sh -r saturation

./gx_mipi_i2c.sh -w contrast
./gx_mipi_i2c.sh -r contrast

./gx_mipi_i2c.sh -w hue
./gx_mipi_i2c.sh -r hue

./gx_mipi_i2c.sh -w slowshutter 1 300
./gx_mipi_i2c.sh -r slowshutter

./gx_mipi_i2c.sh -w ldc
./gx_mipi_i2c.sh -r ldc

./gx_mipi_i2c.sh -w lsc
./gx_mipi_i2c.sh -r lsc

./gx_mipi_i2c.sh -w dehaze
./gx_mipi_i2c.sh -r dehaze

./gx_mipi_i2c.sh -w drc
./gx_mipi_i2c.sh -r drc

./gx_mipi_i2c.sh -w trgedge
./gx_mipi_i2c.sh -r trgedge

./gx_mipi_i2c.sh -w trgdelay
./gx_mipi_i2c.sh -r trgdelay

./gx_mipi_i2c.sh -w trgexp_delay 
./gx_mipi_i2c.sh -r trgexp_delay 


./gx_mipi_i2c.sh -w outio1_mode
./gx_mipi_i2c.sh -r outio1_mode

./gx_mipi_i2c.sh -w outio1_rvs
./gx_mipi_i2c.sh -r outio1_rvs

COMMENT_SAMPLE

print_usage()
{
	echo "Usage:  ./gx_mipi_i2c.sh [-r/w]  [function name] [param1] [param2 ] [param3] [param4] -b bus"
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
    echo "$0 -r -b 10 -d 0x52 read_trgmode"
    echo "$0 -w -b 10 -d 0x52 write_imgacq 1"
    echo "$0 -w -b 10 -d 0x52 write_trgmode 4"
}
######################reglist###################################


if [[ ! -f "./i2c_functions.sh" ]]; then
    echo "❌ Error: "i2c_functions.sh" not found."
    echo "Please ensure that this file is in the same directory as i2c_functions.sh."
    exit 1
fi

source ./i2c_functions.sh || {
    echo "❌ Failed to load i2c_functions.sh"
    exit 1
}


MODE=""
FUNCTION=""
PARAMS=()
b_arg_bus=0
b_arg_addr=0

for arg in "$@"; do
    if [ $b_arg_bus -eq 1 ]; then
        b_arg_bus=0
        I2C_DEV=$arg
        continue
    fi

    if [ $b_arg_addr -eq 1 ]; then
        b_arg_addr=0
        I2C_ADDR=$arg
        continue
    fi

    case $arg in
        "-r")
            MODE="read"
            ;;
        "-w")
            MODE="write"
            ;;
        "-b")
            b_arg_bus=1
            ;;
        "-d")
            b_arg_addr=1
            ;;
        "-h")
            print_usage
            exit 0
            ;;
        *)
            PARAMS+=("$arg")
            ;;
    esac
done


if [ ${#PARAMS[@]} -gt 0 ]; then
    FUNCTION=${PARAMS[0]}
fi

if [ -z "$MODE" ]; then
    echo "Corrected: You must specify either -r (for reading) or -w (for writing)."
    print_usage
    exit 1
fi

if [ -z "$FUNCTION" ]; then
    echo "Error: No function specified to be calle"
    print_usage
    exit 1
fi

if [ -z "$I2C_DEV" ]; then
    echo "Error: The I2C bus number must be specified using the -b option."
    print_usage
    exit 1
fi

if [ -z "$I2C_ADDR" ]; then
    echo "Corrected: You must specify the I2C device address using the -d option."
    print_usage
    exit 1
fi

if [ "$MODE" = "read" ] && [[ "$FUNCTION" != read_* ]]; then
    FUNCTION="read_${FUNCTION}"
elif [ "$MODE" = "write" ] && [[ "$FUNCTION" != write_* ]]; then
    FUNCTION="write_${FUNCTION}"
fi

if ! declare -f "$FUNCTION" > /dev/null 2>&1; then
    echo "Error: The function '$FUNCTION' is undefined. Please check the function name."
    exit 1
fi

#echo "Execution: $MODE mode | Bus: $I2C_DEV | Address: $I2C_ADDR | Function: $FUNCTION ${PARAM1:+$PARAM1} ${PARAM2:+$PARAM2} ${PARAM3:+$PARAM3}"

"$FUNCTION" "${PARAMS[@]:1}"

exit 0


