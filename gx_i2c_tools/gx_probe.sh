
#!/bin/bash
# Usage: source ./gx_probe.sh [i2c_bus_number]
# Example: source ./gx_probe.sh 10
#
# This script probes the specified I2C bus for a VEYE camera (address 0x3b)
# and sets environment variables CAMERAMODEL, FPS, WIDTH, HEIGHT
# based on files in /sys/bus/i2c/devices/i2c-<bus>/<bus>-003b/veye_gxcam/.
#
# Requires gxcam driver v1.1.06 or later.

if [ "$#" -ne 1 ]; then
    echo "Error: Please provide exactly one argument: the I2C bus number."
    echo "Usage: source $0 <i2c_bus_number>"
    echo "Example: source $0 10"
    return 1
fi

i2c_bus_num="$1"
i2c_device="/sys/bus/i2c/devices/i2c-${i2c_bus_num}"

# Check if the I2C bus exists
if [ ! -d "$i2c_device" ]; then
    echo "Error: I2C bus i2c-${i2c_bus_num} not found."
    return 1
fi

found=0
# Iterate through subdirectories under the specified I2C bus
for sub_device in "$i2c_device"/*; do
    if [ -d "$sub_device" ]; then
        # Match device address 0x3b (i.e., ends with -003b)
        if [[ "$sub_device" == *-003b ]]; then
            if [ -d "$sub_device/veye_gxcam" ]; then
                echo "Found veye_gxcam camera on i2c-${i2c_bus_num}."
                export I2C_BUS="$i2c_bus_num"
                echo "Setenv I2C_BUS = $I2C_BUS"
                found=1
                # Read and export environment variables
                for file in camera_model fps width height; do
                    file_path="$sub_device/veye_gxcam/$file"
                    if [ -f "$file_path" ]; then
                        value=$(cat "$file_path")
                        case $file in
                            camera_model)
                                export CAMERAMODEL="$value"
                                echo "Setenv CAMERAMODEL = $value"
                                ;;
                            fps)
                                export FPS="$value"
                                echo "Setenv FPS = $value"
                                ;;
                            width)
                                export WIDTH="$value"
                                echo "Setenv WIDTH = $value"
                                ;;
                            height)
                                export HEIGHT="$value"
                                echo "Setenv HEIGHT = $value"
                                ;;
                        esac
                    else
                        echo "$file: File not found"
                    fi
                done
                return 0
            else
                echo "The gxcam driver is loaded on i2c-${i2c_bus_num}, but the camera is not detected!"
            fi
        fi
    fi
done

if [ "$found" -eq 0 ]; then
    echo "No GX camera (0x3b) found on i2c-${i2c_bus_num}."
    echo "The driver may be too old, not loaded, or camera not connected."
    return 1
fi

