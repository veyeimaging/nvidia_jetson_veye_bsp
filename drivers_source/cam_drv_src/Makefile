KERNEL_ROOT=/lib/modules/$(shell uname -r)/build 
PWD=$(shell pwd)

obj-m := veye_mvcam.o veyecam.o cs_imx307.o cs_sc132.o ds90ub954.o veye_imx335.o veye_vbyone.o

all:
	make ARCH=arm64 -C $(KERNEL_ROOT) M=$(PWD) modules

clean:
	make ARCH=arm64 -C $(KERNEL_ROOT) M=$(PWD) clean
