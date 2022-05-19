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
#include "i2c_4comm.h"

#define I2C_DEVICE_NAME_LEN 13	// "/dev/i2c-XXX"+NULL
static char i2c_device_name[I2C_DEVICE_NAME_LEN];

#define MAX_BLK_BUFF_SIZE (4096)

#define LUT_COUNT_ADDR (0xC70)
#define LUT_W_START_ADDR (0xC74)
#define LUT_W_DONE_ADDR (0xC78)
#define LUT_DATA_ADDR (0x1800)

static int i2c_rd(int fd, uint8_t i2c_addr, uint16_t reg, uint32_t *values, uint32_t n)
{
	int err;
	int i = 0;
	uint8_t buf[2] = { reg >> 8, reg & 0xff };
    uint8_t bufout[8] = {0};
	struct i2c_rdwr_ioctl_data msgset;
	struct i2c_msg msgs[2] = {
		{
			 .addr = i2c_addr,
			 .flags = 0,
			 .len = 2,
			 .buf = buf,
		},
		{
			.addr = i2c_addr,
			.flags = I2C_M_RD ,
			.len = 4,
			.buf = bufout,
		},
	};

	msgset.msgs = msgs;
	msgset.nmsgs = 2;

	err = ioctl(fd, I2C_RDWR, &msgset);
	//printf("Read i2c addr %02X\n", i2c_addr);
	if (err != msgset.nmsgs)
    {
        printf("Read i2c err %d\n", err);
		return -1;
    }
	for(i = 0; i < n;++i)
	{
        *values = ntohl(*(uint32_t*)bufout);
        //*values = ntohl(*values );
		fprintf(stderr,"addr %04x : value %08x \n",reg+i,values[i]);
	}
	
	return 0;
}

static int  send_regs(int fd,  const struct sensor_regs *regs, int num_regs)
{
    int ret = 0;
	int i;
	for (i=0; i<num_regs; i++)
	{
		{
			unsigned char msg[8] = {regs[i].reg>>8, regs[i].reg, regs[i].data>>24,
            regs[i].data>>16,regs[i].data>>8,regs[i].data,};
			int len = 6;

			if (write(fd, msg, len) != len)
			{
				fprintf(stderr,"Failed to write register index %d,please try again\r\n", regs[i].reg);
                ret = -1;
			}
		}
	}
    return ret;
}

U32 main(int argc, char *argv[])
{
	U32 I2C_port;
	U32 device_addr;
	U32 reg_addr;
	U32 new_data;
	char file[128];
	U32 num = 1;
    char * buff = NULL;
    U32 read_flag = 1;
    U32 i = 0;
    U32 lut_size = 0;
    U32 lut_val = 0;
    char* record = NULL;
    FILE *filefp = NULL;
	int result;
    int ret = 0;
    struct sensor_regs regs;
	if (argc < 4)
	{
		printf("usage: %s <bus_num> <device address(8bit)> <r/w> <filename>. sample: %s 0x10 0x3b 0x1800 r lutfile.txt\n", argv[0], argv[0]);
		return -1;
	}

	if(StrToNumber(argv[1], &I2C_port) != HI_SUCCESS ) {
		printf("Please input i2c port like 0x100 or 256.\r\n");
		return -1;
	}

	if(StrToNumber(argv[2], &device_addr) != HI_SUCCESS ) {
		printf("Please input dev addr like 0x100 or 256.\r\n");
		return -1;
	}
    if(strchr(argv[3], 'w') != NULL ) {
		//printf("write mode\r\n");
		read_flag = 0;
	}
    else{
        //printf("read mode\r\n");
    }
    strncpy(file,argv[4],sizeof(file));
	
	snprintf(i2c_device_name, sizeof(i2c_device_name), "/dev/i2c-%d", I2C_port);
	//printf("Using i2C device %s\n", i2c_device_name);
	
	//printf("====I2C read:<%#x> <%#x> <%#x>====\n", device_addr, reg_addr, num);
	
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
    //read lut size
    i2c_rd(fd, device_addr,LUT_COUNT_ADDR, &lut_size, 1);
   // printf("lutsize %d\r\n",lut_size);
    
    if(read_flag == 1)
    {
        filefp=fopen(file,"w+");
    }
    else
    {
        filefp=fopen(file,"r");
    }
    if(filefp == NULL)
    {
        printf("open file %s error !\r\n",file);
        goto ERROR;
    }
    
    if(read_flag == 1)
    {
        for(i = 0; i < lut_size;++i)
        {
            i2c_rd(fd, device_addr,LUT_DATA_ADDR+i*2, &lut_val, 1);
            fprintf(filefp,"%d,",lut_val);
        }
    }
    else
    {
        buff = malloc(5*MAX_BLK_BUFF_SIZE);
        if(buff == NULL)
        {
            printf("Failed to malloc memory\n");
            goto ERROR;
        }
        
        fread(buff, 5*MAX_BLK_BUFF_SIZE,1, filefp);

        regs.reg = LUT_W_START_ADDR;
        regs.data = 1;
        ret |= send_regs(fd,&regs, 1);
        usleep(1000*500);
        record = strtok(buff, ",");
        i = 0;
        while (record != NULL && i < lut_size)
        {
           // fprintf(stderr,"%s \r\n", record);//将读取到的每一个数据打印出来
            //send to camera
            StrToNumber(record,&lut_val);
            regs.data = lut_val;
            record = strtok(NULL, ",");
            StrToNumber(record,&lut_val);
            regs.data += lut_val<<16;
            regs.reg = LUT_DATA_ADDR+i*2;
            ret |= send_regs(fd,&regs, 1);
            usleep(1000*1);
            record = strtok(NULL, ",");
            ++i;
            ++i;
        }
        usleep(1000);
        regs.reg = LUT_W_DONE_ADDR;
        regs.data = 1;
        ret |= send_regs(fd,&regs, 1);
        usleep(1000);
    }
    
ERROR:
	if(fd) close(fd);
    
	if(filefp) fclose(filefp);
	fprintf(stdout,"%d",ret);
	return ret;
	
}

