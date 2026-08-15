
Metadata Decode Usage Instructions
1. Ensure that the camera is initialized and running.
2. Enable the metadata_mode register using either veye_viewer or gx_mipi_i2c.sh.
3. Use the yata utility to capture the raw YUV image data.
4. Execute the following command to parse the metadata:
"./decode_metadata -i <yuv_file> <width> <height>"


