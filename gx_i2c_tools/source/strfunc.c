/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : strfunc.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2005/7/27
  Last Modified :
  Description   : String functions
  Function List :
  History       :
  1.Date        : 2005/7/27
    Author      : T41030
    Modification: Created file

******************************************************************************/

#include <stdio.h>
#include <ctype.h>
//#include "hi.h"
//#include "strfunc.h"
#include "i2c_vcomm.h"

int atoul( int8_t *str, uint32_t * pulValue);
int atoulx( int8_t *str, uint32_t * pulValue);

int StrToNumber( int8_t *str ,  uint32_t * pulValue)
{
    /* check hex string */
    if ( *str == '0' && (*(str+1) == 'x' || *(str+1) == 'X') )
    {
        if (*(str+2) == '\0')
        {
            return HI_FAILURE;
        }
        else
        {
            return atoulx(str+2,pulValue);
        }
    }
    else
    {
        return atoul(str,pulValue);
    }
}

int atoul( int8_t *str, uint32_t * pulValue)
{
    uint32_t ulResult=0;

    while (*str)
    {
        if (isdigit((int)*str))
        {
            /* max value: 0xFFFFFFFF(4294967295),
               X * 10 + (*str)-48 <= 4294967295
               so, X = 429496729 */
            if ((ulResult<429496729) || ((ulResult==429496729) && (*str<'6')))
            {
                ulResult = ulResult*10 + (*str)-48;
            }
            else
            {
                *pulValue = ulResult;
                return HI_FAILURE;
            }
        }
        else
        {
            *pulValue=ulResult;
            return HI_FAILURE;
        }
        str++;
    }
    *pulValue=ulResult;
    return HI_SUCCESS;
}


#define ASC2NUM(ch) (ch - '0')
#define HEXASC2NUM(ch) (ch - 'A' + 10)

int  atoulx( int8_t *str, uint32_t * pulValue)
{
    uint32_t   ulResult=0;
    uint8_t ch;

    while (*str)
    {
        ch=toupper(*str);
        if (isdigit(ch) || ((ch >= 'A') && (ch <= 'F' )))
        {
            if (ulResult < 0x10000000)
            {
                ulResult = (ulResult << 4) + ((ch<='9')?(ASC2NUM(ch)):(HEXASC2NUM(ch)));
            }
            else
            {
                *pulValue=ulResult;
                return HI_FAILURE;
            }
        }
        else
        {
            *pulValue=ulResult;
            return HI_FAILURE;
        }
        str++;
    }
    
    *pulValue=ulResult;
    return HI_SUCCESS;
}

uint8_t xor8(const uint8_t *data, uint32_t len) 
{
    uint8_t checksum = 0;
    const uint8_t *p = data;
    while (len--) 
    {
        checksum ^= *p++;
    }
    return checksum;
}
