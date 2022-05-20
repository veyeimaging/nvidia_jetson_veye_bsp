# v4l2 examples for VEYE MV Series MIPI CSI-2 cameras

This is a small tool kit based on v4l2grab. 
Support video streaming mode, soft trigger mode, hard trigger mode. 
Image save as raw format.

It use IO_MMAP mode only.

# Build

```
sudo apt install libv4l-dev
```

```
make
```
# Usage

```
Usage: ./v4l2grab_mvcam_jetson [options]

Options:
-d | --device name   Video device name [/dev/video0]
-h | --help          Print this message
-o | --output        Set output filename
-p | --pixelformat   Set pixel format,0:raw8,1:raw10,2:raw12
-m | --trgmode       Set camera mode,0:Video streaming mode,1:Normal trigger mode,2:High-speed continuous trigger mode.
-s | --trgsrc        Set camera trigger source,0:Software trigger,1:Hardware trigger.
-j | --jetson        Set jetson board, tx2 or xavier.
-X | --start_x        Set image start x
-Y | --start_y        Set image start y
-W | --width         Set image width
-H | --height        Set image height
-I | --interval      Set frame interval (fps) (-1 to skip)
-c | --continuous    Do continous capture, stop with SIGINT.
-v | --version       Print version
```

## open file

After setting the ROI, it will prompt the actual ROI in effect and the width and height needed to open the image.

For raw10 and raw12 formats, you need to use [pixel_layer_convert](https://github.com/veyeimaging/pixel_layer_convert) to convert the images first.

Recommend using [vooya](https://www.offminor.de/) to open images.

## Video streaming mode

### save one raw8 frame @(0,0,3088,2064)
```
./v4l2grab_mvcam_jetson -j xavier -d /dev/video0 -o y8_3136x2064.raw -X 0 -Y 0 -W 3088 -H 2064 
```
open with [vooya](https://www.offminor.de/) @ Color space: single channel, 3136*2064, bitdepth8

### save one raw8 frame @(500,500,500,500)
```
./v4l2grab_mvcam_jetson -j xavier -d /dev/video0 -o y8_500x500.raw -X 500 -Y 500 -W 500 -H 500 
```
open with [vooya](https://www.offminor.de/) @ Color space: single channel, 512*504,bitdepth8

### save one raw10 frame @(0,0,3088,2064)
```
./v4l2grab_mvcam_jetson -j xavier -d /dev/video0 -p 1 -o y10_3104x2064.raw -X 0 -Y 0 -W 3088 -H 2064 
```
convert
```
./pixel_layer_convert -I XY10 -i ./y10_3104x2064.raw -o ./y10_3136x2064_new.raw 
```
open with [vooya](https://www.offminor.de/) @ Color space: single channel, 3104*2064, bitdepth10

### save one raw12 frame @(0,0,3088,2064)

```
./v4l2grab_mvcam_jetson -j xavier -d /dev/video0 -p 2 -o y12_3104x2064.raw -X 0 -Y 0 -W 3088 -H 2064 
```
convert
```
./pixel_layer_convert -I XY12 -i ./y12_3104x2064.raw -o ./y12_3104x2064_new.raw 
```
open with [vooya](https://www.offminor.de/) @ Color space: single channel, 3104*2064, bitdepth12

## Soft trigger mode 
- In one shell
```
./v4l2grab_mvcam_jetson -j xavier -d /dev/video0 -p 0 -m 1 -s 1 -o y8_3136x2064 -X 0 -Y 0 -W 3088 -H 2064 -c 
```
- In another shell

Every time the following command is executed, it will trigger once.
```
v4l2-ctl -d /dev/video0 --set-ctrl soft_trgone=1
```
or

```
./mv_mipi_i2c.sh -w -f trgone -b [i2c_bus]
```
- Stop capturing

In v4l2grab_mvcam_jetson shell,Ctrl-C will send a SIGINT to v4l2grab_mvcam_jetson process and make it quit.

## Hardware trigger mode 
- In one shell

```
./v4l2grab_mvcam_jetson -j xavier -d /dev/video0 -p 0 -m 1 -s 0 -o y8_3136x2064 -X 0 -Y 0 -W 3088 -H 2064 -c 
```

- In another shell

This script use BOARD pin 40 as trigger source, will cause 5 rising edge.
```
python gpio_trigger_jetson.py
```
- Stop capturing

In v4l2grab_mvcam_jetson shell,Ctrl-C will send a SIGINT to v4l2grab_mvcam_jetson process and make it quit.
