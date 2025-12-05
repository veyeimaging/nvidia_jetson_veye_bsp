
#ifndef _I2C_COMM_H
#define _I2C_COMM_H

#define uint32_t unsigned int
#define uint16_t unsigned short
#define uint8_t unsigned char
#define int8_t char

#define HI_SUCCESS 0
#define HI_FAILURE (-1)

#define WRITE_HEAD 0xAB
#define PRE_READ_HEAD 0xBC
#define POST_READ_HEAD 0xAC
#define DIR_READ_HEAD 0xDE

#pragma pack(push, 1)  
struct preread_regs {
	uint8_t	pre_head;
	uint16_t reg;
	uint8_t xor;
};
struct write_regs {
	uint8_t	head;
	uint16_t reg;
	uint32_t data;
	uint8_t xor;
};
#pragma pack(pop) 

int StrToNumber( int8_t *str ,  uint32_t * pulValue);
uint8_t xor8(const uint8_t *data, uint32_t len);

uint32_t get_val(uint8_t *buff, uint32_t len);
#endif

