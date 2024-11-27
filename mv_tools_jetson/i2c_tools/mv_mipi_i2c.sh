#!/bin/bash

# Default values
function_name=""
param1=""
param2=""
param3=""
param4=""
bus=""
read_write=""

# Function to parse the arguments manually
while [[ $# -gt 0 ]]; do
    case "$1" in
        -r)
            read_write="-r"
            shift
            ;;
        -w)
            read_write="-w"
            shift
            ;;
        -f)
            function_name="$2"
            shift 2
            ;;
        -p1)
            param1="$2"
            shift 2
            ;;
        -p2)
            param2="$2"
            shift 2
            ;;
        -p3)
            param3="$2"
            shift 2
            ;;
        -p4)
            param4="$2"
            shift 2
            ;;
        -b)
            bus="-b $2"
            shift 2
            ;;
        *)
            echo "Invalid option: $1"
            exit 1
            ;;
    esac
done

# If function_name is empty, print an error
if [ -z "$function_name" ]; then
    echo "Error: function name must be provided."
    exit 1
fi

# Construct the parameters for the second script
params="$read_write $function_name"

# Add the parameters p1, p2, p3, and p4 if they are provided
if [ -n "$param1" ]; then
    params="$params $param1"
fi
if [ -n "$param2" ]; then
    params="$params $param2"
fi
if [ -n "$param3" ]; then
    params="$params $param3"
fi
if [ -n "$param4" ]; then
    params="$params $param4"
fi

# If -b bus is provided, include it in the parameters
if [ -n "$bus" ]; then
    params="$params $bus"
fi

# Print the final command for debugging
echo "Executing: ./mv_mipi_i2c_new.sh $params"

# Call the second script with the constructed parameters
./mv_mipi_i2c_new.sh $params
