#!/bin/bash
#usage source ./mv_probe.sh
# This script is used to probe the connected camera and set the environment variables
# CAMERAMODEL, FPS, HEIGHT, and WIDTH based on the contents of the files in the veye_mvcam directory.
# The script will iterate through all I2C buses in /sys/bus/i2c/devices and look for the veye_mvcam directory.
# please make sure that the camera is connected to the I2C bus and the driver is loaded.
# Please make sure the mvcam driver version is 1.1.06 or later.

for i2c_device in /sys/bus/i2c/devices/i2c-*; do
    # Extract the I2C bus name (e.g., i2c-10)
    i2c_bus=$(basename "$i2c_device")
    
    # Check each subdirectory under the I2C bus
    for sub_device in "$i2c_device"/*; do
        # Check if the subdirectory exists
        if [ -d "$sub_device" ]; then
            # Check if the subdirectory matches the expected format (e.g., 11-003b)
            if [[ "$sub_device" == *-003b ]]; then
                #echo "Found device directory: $sub_device on $i2c_bus"
                # Check if the veye_mvcam directory exists
                if [ -d "$sub_device/veye_mvcam" ]; then
                    echo "Found veye_mvcam camera on $i2c_bus."
                    # Read and set the contents of the files as environment variables
                    #echo "Reading files in $sub_device/veye_mvcam:"
                    for file in camera_model fps width height; do
                        file_path="$sub_device/veye_mvcam/$file"
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
                    return 0  # Use return instead of exit
                else
                    # If the subdirectory exists but veye_mvcam is missing
                    echo "The mvcam driver is loaded on $i2c_bus, but the camera is not detected!"
                fi
            fi
        fi
    done
done

echo "No device found. The driver may be too old or not loaded."
return 1