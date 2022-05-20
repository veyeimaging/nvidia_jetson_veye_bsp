
CFLAGS ?= -g -Wall -Wextra -lv4l2

#-O2

all: v4l2grab_mvcam_jetson.c
	gcc -o v4l2grab_mvcam_jetson v4l2grab_mvcam_jetson.c $(CFLAGS)

clean : 
	rm v4l2grab_mvcam_jetson.o
    