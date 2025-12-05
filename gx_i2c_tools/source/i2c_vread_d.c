#include <ctype.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "i2c_vcomm.h"

#define I2C_DEVICE_NAME_LEN 13	// "/dev/i2c-XXX"+NULL
static char i2c_device_name[I2C_DEVICE_NAME_LEN];

//direct register access
static int gxcam_readl_d_reg(int fd, uint8_t i2c_addr, uint16_t reg, uint8_t head_addr,uint32_t *val)
{
	int err;
	uint8_t checksum;
	uint8_t buf[3] = {head_addr,  reg >> 8, reg & 0xff };
    uint8_t bufout[8] = {0};

	struct i2c_rdwr_ioctl_data msgset;
	struct i2c_msg msgs[2] = {
		{
			 .addr = i2c_addr,
			 .flags = 0,
			 .len = 3,
			 .buf = buf,
		},
		{
			.addr = i2c_addr,
			.flags = I2C_M_RD ,
			.len = 6,
			.buf = bufout,
		},
	};

	msgset.msgs = msgs;
	msgset.nmsgs = 2;

	err = ioctl(fd, I2C_RDWR, &msgset);
	if (err != msgset.nmsgs)
    {
        printf("Read i2c err %d\n", err);
		return -1;
    }

	checksum = xor8(bufout, 5);
	if (checksum != bufout[5]) {
		printf("check sum err\n");
		return -1;
	}
	*val = ntohl(*(uint32_t*)bufout);
	
	return 0;
}

int main(int argc, char *argv[])
{
	uint32_t I2C_port;
	uint32_t device_addr;
	uint32_t reg_addr;
	uint32_t head_addr;
	uint32_t val = 0;

	if (argc < 4)
	{
		printf("usage: %s <bus_num> <device address(8bit)> <register address(16bits)> <len default:1 max100>. sample: %s 0x0 0xA0 0x1000 0x2\n", argv[0], argv[0]);
		return -1;
	}

	if(StrToNumber(argv[1], &I2C_port) != HI_SUCCESS ) 
	{
		printf("Please input i2c port like 0x100 or 256.\r\n");
		return -1;
	}

	if(StrToNumber(argv[2], &device_addr) != HI_SUCCESS ) 
	{
		printf("Please input dev addr like 0x100 or 256.\r\n");
		return -1;
	}

	if(StrToNumber(argv[3], &head_addr) != HI_SUCCESS ) 
	{
		printf("Please input head addr like 0x100 0r 256.\r\n");
		return -1;
	}
	
	if(StrToNumber(argv[4], &reg_addr) != HI_SUCCESS ) 
	{
		printf("Please input reg_addr like 0x100 0r 256.\r\n");
		return -1;
	}

	snprintf(i2c_device_name, sizeof(i2c_device_name), "/dev/i2c-%d", I2C_port);

	int fd;
	fd = open(i2c_device_name, O_RDWR);
	if (!fd)
	{
		printf("Couldn't open I2C device\n");
		return -1;
	}
	if (ioctl(fd, I2C_SLAVE_FORCE, device_addr) < 0)
	{
		printf("Failed to set I2C address\n");
		return -1;
	}
    gxcam_readl_d_reg(fd, device_addr, reg_addr,head_addr, &val);
	
	fprintf(stderr,"value %08x \n", val);

	close(fd);
	
	fprintf(stdout,"%d",val);
	return 0;
}
