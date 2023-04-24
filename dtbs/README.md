
## Introduction to the directorys
First, depending on the jetson model used and the current jetpack version of your board, you can go to the corresponding `dts dtb`directory. In this directory:
- common
  This directory contains dts source code files common to all cameras.
- Camera models
  Differentiated dts source code corresponding to different camera models as well as compiled dtb files.
  A special note is needed:
    1. MV-MIPI-CAM corresponds to all MV series and RAW series cameras.
    1. VEYE-MIPI-CAM2M corresponds to all VEYE series cameras with 2M resolution.
    1. The suffix is fpdlink, corresponding to the corresponding model of the fpdlink variant of the camera model.
- jetson-io
  If the jetson-io directory does not exist, it means that we have not compiled a version of jetson-io for this model. You can modify the dts source code to implement the jetson-io version.

  The source code and dtb in the jetson-io directory support the official jetson-io tool for switching between different cameras.(fpdlink models are not supported at this time.)

## For the non-jetson-io mechanism, the use of the dtb that names the camera model directly in the dtb
For example, if you are using VEYE-MIPI-IMX327S + Jetson Orin NX 8GB(with P3768 carrier board):
1. Create a directory.
   ```
    sudo mkdir /boot/veyecam/
    ```
2. Copy the required dtb file into it. 
   Copy tegra234-p3767-0001-p3768-0000-a0.dtb in the VEYE-MIPI-CAM2M directory to the /boot/veyecam directory.
3. Modify the /boot/extlinux/extlinux.conf file and add the following to the last line
    `FDT /boot/veyecam/tegra234-p3767-0001-p3768-0000-a0.dtb`
    This will override the original dtb configuration and enable the dtb file we specified.

## For the jetson-io mechanism
For example, if you are using VEYE-MIPI-IMX327S + Jetson Orin NX 8GB(with P3768 carrier board):
1. Backup origion dtb setting.
   `sudo cp /boot/extlinux/extlinux.conf /boot/extlinux/extlinux.conf.backup`
   `cat /boot/extlinux/extlinux.conf `
   You can see:
   `FDT /boot/dtb/kernel_tegra234-p3767-0001-p3768-0000-a0.dtb`
    This indicates that the dtb file currently in effect on the system is /boot/dtb/kernel_tegra234-p3767-0001-p3768-0000-a0.dtb.
    `sudo cp /boot/dtb/kernel_tegra234-p3767-0001-p3768-0000-a0.dtb /boot/dtb/kernel_tegra234-p3767-0001-p3768-0000-a0.dtb.backup`
2. Overwrite the original dtb file with the dtb file containing our camera configuration.
   `sudo cp <path_to_jetson-io>/tegra234-p3767-0001-p3768-0000-a0.dtb /boot/dtb/kernel_tegra234-p3767-0001-p3768-0000-a0.dtb`
3. Copy the required camera dtbo file to the system.
   `sudo cp <path_to_jetson-io>/tegra234-p3767-camera-p3768-veyecam2m-dual.dtbo /boot`
4. Refer to jetson's official website and use jetson-io to select the camera model.
    `sudo /opt/nvidia/jetson-io/jetson-io.py`