/*
 * ds90ub954.c - TI DS90UB954 deserializer and DS90UB953 serializer driver
 *
 * Copyright (c) 2020, Institut of Embedded Systems ZHAW
 *
 * This program is for the DS90UB954 FDP Link III deserializer in connection
 * with the DS90UB953 serializer from Texas Instruments
 *
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef I2C_DS90UB954_H
#define I2C_DS90UB954_H

#include <linux/i2c.h>

/*------------------------------------------------------------------------------
 * Deserializer registers
 *----------------------------------------------------------------------------*/
#define TI954_REG_I2C_DEV_ID 0x00
#define TI954_DES_ID         0
#define TI954_DEVICE_ID      1

#define TI954_REG_RESET        0x01
#define TI954_DIGITAL_RESET0   0
#define TI954_DIGITAL_RESET1   1
#define TI954_RESTART_AUTOLOAD 2

#define TI954_REG_GENERAL_CFG           0x2
#define TI954_FORCE_REFCLK_DET          0
#define TI954_RX_PARITY_CHECKER_ENABLE  1
#define TI954_OUTPUT_SLEEP_STATE_SELECT 2
#define TI954_OUTPUT_ENABLE             3
#define TI954_OUTPUT_EN_MODE            4
#define TI954_I2C_MASTER_EN             5

#define TI954_REG_REVISION 0x03
#define TI954_MASK_ID      0

#define TI954_REG_DEVICE_STS 0x04
#define TI954_LOCK           2
#define TI954_PASS           3
#define TI954_REFCLK_VALID   4
#define TI954_CFG_INIT_DONE  6
#define TI954_CFG_CKSUM_STS  7

#define TI954_REG_PAR_ERR_THOLD_HI 0x5
#define TI954_PAR_ERR_THOLD_HI     0

#define TI954_REG_PAR_ERR_THOLD_LO 0x6
#define TI954_PAR_ERR_THOLD_LO     0

#define TI954_REG_BCC_WD_CTL             0x07
#define TI954_BCC_WATCHDOG_TIMER_DISABLE 0
#define TI954_BCC_WATCHDOG_TIMER         1

#define TI954_REG_I2C_CTL1        0x08
#define TI954_I2C_FILTER_DEPTH    0
#define TI954_I2C_SDA_HOLD        4
#define TI954_LOCAL_WRITE_DISABLE 7

#define TI954_REG_I2C_CTL2          0x09
#define TI954_I2C_BUS_TIMER_DISABLE 0
#define TI954_I2C_BUS_TIMER_SPEEDUP 1
#define TI954_SDA_OUTPUT_DELAY      2
#define TI954_SDA_OUTPUT_SETUP      4

#define TI954_REG_SCL_HIGH_TIME 0x0a
#define TI954_SCL_HIGH_TIME     0

#define TI954_REG_SCL_LOW_TIME 0x0b
#define TI954_SCL_LOW_TIME     0

#define TI954_REG_RX_PORT_CTL 0x0c
#define TI954_PORT0_EN        0
#define TI954_PORT1_ER        1
#define TI954_LOCK_SEL        2
#define TI954_PASS_SEL        4

#define TI954_REG_IO_CTL        0x0d
#define TI954_IO_SUPPLY_MODE    4
#define TI954_IO_SUPPLY_MODE_OV 6
#define TI954_SEL3P3V           7

#define TI954_REG_GPIO_PIN_STS 0x0e
#define TI954_GPIO_STS         0
#define TI954_GPIO0_STS        0
#define TI954_GPIO1_STS        1
#define TI954_GPIO2_STS        2
#define TI954_GPIO3_STS        3
#define TI954_GPIO4_STS        4
#define TI954_GPIO5_STS        5
#define TI954_GPIO6_STS        6

#define TI954_REG_GPIO_INPUT_CTL 0x0f
#define TI954_GPIO_INPUT_EN      0
#define TI954_GPIO0_INPUT_EN     0
#define TI954_GPIO1_INPUT_EN     1
#define TI954_GPIO2_INPUT_EN     2
#define TI954_GPIO3_INPUT_EN     3
#define TI954_GPIO4_INPUT_EN     4
#define TI954_GPIO5_INPUT_EN     5
#define TI954_GPIO6_INPUT_EN     6

#define TI954_REG_GPIO0_PIN_CTL 0x10
#define TI954_GPIO0_OUT_EN      0
#define TI954_GPIO0_OUT_VAL     1
#define TI954_GPIO0_OUT_SRC     2
#define TI954_GPIO0_OUT_SEL     5

#define TI954_REG_GPIO1_PIN_CTL 0x11
#define TI954_GPIO1_OUT_EN      0
#define TI954_GPIO1_OUT_VAL     1
#define TI954_GPIO1_OUT_SRC     2
#define TI954_GPIO1_OUT_SEL     5

#define TI954_REG_GPIO2_PIN_CTL 0x12
#define TI954_GPIO2_OUT_EN      0
#define TI954_GPIO2_OUT_VAL     1
#define TI954_GPIO2_OUT_SRC     2
#define TI954_GPIO2_OUT_SEL     5

#define TI954_REG_GPIO3_PIN_CTL 0x13
#define TI954_GPIO3_OUT_EN      0
#define TI954_GPIO3_OUT_VAL     1
#define TI954_GPIO3_OUT_SRC     2
#define TI954_GPIO3_OUT_SEL     5

#define TI954_REG_GPIO4_PIN_CTL 0x14
#define TI954_GPIO4_OUT_EN      0
#define TI954_GPIO4_OUT_VAL     1
#define TI954_GPIO4_OUT_SRC     2
#define TI954_GPIO4_OUT_SEL     5

#define TI954_REG_GPIO5_PIN_CTL 0x15
#define TI954_GPIO5_OUT_EN      0
#define TI954_GPIO5_OUT_VAL     1
#define TI954_GPIO5_OUT_SRC     2
#define TI954_GPIO5_OUT_SEL     5

#define TI954_REG_GPIO6_PIN_CTL 0x16
#define TI954_GPIO6_OUT_EN      0
#define TI954_GPIO6_OUT_VAL     1
#define TI954_GPIO6_OUT_SRC     2
#define TI954_GPIO6_OUT_SEL     5

#define TI954_REG_RESERVED    0x17

#define TI954_REG_FS_CTL    0x18
#define TI954_FS_GEN_ENABLE 0
#define TI954_FS_GEN_MODE   1
#define TI954_FS_INIT_STATE 2
#define TI954_FS_SINGLE     3
#define TI954_FS_MODE       4

#define TI954_REG_FS_HIGH_TIME_1    0x19
#define TI954_FRAMESYNC_HIGH_TIME_1 0

#define TI954_REG_FS_HIGH_TIME_0    0x1A
#define TI954_FRAMESYNC_HIGH_TIME_0 0

#define TI954_REG_FS_LOW_TIME_1    0x1B
#define TI954_FRAMESYNC_LOW_TIME_1 0

#define TI954_REG_FS_LOW_TIME_0    0x1C
#define TI954_FRAMESYNC_LOW_TIME_0 0

#define TI954_REG_MAX_FRM_HI 0x1d
#define TI954_MAX_FRAME_HI   0

#define TI954_REG_MAX_FRM_LO 0x1e
#define TI954_MAX_FRAME_LO   0

#define TI954_REG_CSI_PLL_CTL 0x1f
#define TI954_CSI_TX_SPEED    0

#define TI954_REG_FWD_CTL1  0x20
#define TI954_FWD_PORT0_DIS 4
#define TI954_FWD_PORT1_DIS 6

#define TI_954_FWD_CTL2         0x21
#define TI954_CSI0_RR_RWD       0
#define TI954_CSI0_SYNC_FWD     2
#define TI954_FWD_SYNC_AS_AVAIL 6
#define TI954_CSI_REPLICATE     7

#define TI954_REG_FWD_STS    0x22
#define TI954_FWD_SYNC0      0
#define TI954_FWD_SYNC_FAIL0 2

#define TI954_REG_INTERRUPT_CTL 0x23
#define TI954_IE_RX0            0
#define TI954_IE_RX1            1
#define TI954_IE_CSI_TX0        4
#define TI954_INT_EN            7

#define TI954_REG_INTERRUPT_STS 0x24
#define TI954_IS_RX0            0
#define TI954_IS_RX1            1
#define TI954_IS_CSI_TX0        4
#define TI954_INTERRUPT_STS     7

#define TI954_REG_TS_CONFIG 0x25
#define TI954_TS_MODE       0
#define TI954_TS_FREERUN    1
#define TI954_TS_AS_AVAIL   3
#define TI954_TS_RES_CTL    4
#define TI954_FS_POLARITY   6

#define TI954_REG_TS_CONTROL 0x26
#define TI954_TS_ENABLE0     0
#define TI954_TS_ENABLE1     1
#define TI954_TS_FREEZE      4

#define TS954_TS_LINE_HI 0x27
#define TI954_TS_LINE_HI 0

#define TI954_REG_TS_LINE_LO 0x28
#define TI954_TS_LINE_LO     0

#define TI954_REG_TS_STATUS 0x29
#define TI954_TS_VALID0     0
#define TI954_TS_VALID1     1
#define TI954_TS_READY      42

#define TI954_TI954_REG_TIMESTAMP_P0_HI 0x2a
#define TI954_TIMESTAMP_P0_HI           03

#define TI954_TI954_REG_TIMESTAMP_P0_LO 0x2b
#define TI954_TIMESTAMP_P0_LO           04

#define TI954_TI954_REG_TIMESTAMP_P1_HI 0x2c
#define TI954_TIMESTAMP_P1_HI           0

#define TI954_REG_TIMESTAMP_P1_LO 0x2d
#define TI954_TIMESTAMP_P1_LO     0

#define TI954_REG_CSI_CTL     0x33
#define TI954_CSI_ENABLE      0
#define TI954_CSI_CONTS_CLOCK 1
#define TI954_CSI_ULP         2
#define TI954_CSI_LANE_COUNT  4
#define TI954_CSI_CAL_EN      6
#define TI954_CSI_4_LANE      0
#define TI954_CSI_3_LANE      1
#define TI954_CSI_2_LANE      2
#define TI954_CSI_1_LANE      3

#define TI954_REG_CSI_CTL2     0x34
#define TI954_CSI_CAL_PERIODIC 0
#define TI954_CSI_CAL_SINGLE   1
#define TI954_CSI_CAL_INV      2
#define TI954_CSI_PASS_MODE    3

#define TI954_REG_CSI_STS  0x35
#define TI954_TX_PORT_PASS 0
#define TI954_TX_PORT_SYNC 1

#define TI954_REG_CSI_TX_ICR    0x36
#define TI954_IE_CSI_PASS       0
#define TI954_IE_SCI_PASS_ERROR 1
#define TI954_IE_CSI_SYNC       2
#define TI954_IE_CSI_SYNC_ERROR 3

#define TI954_REG_CSI_TX_ISR     0x37
#define TI954_IS_CSI_PASS        0
#define TI954_IS_CSI_PASS_ERR_OR 1
#define TI954_IS_CSI_SYNC        2
#define TI954_IS_CSI_SYNC_ERR_OR 3
#define TI954_IS_RX_PORT_INT     4

#define TI954_REG_CSI_TEST_CTL 0x38

#define TI954_REG_CSI_TEST_PATT_HI 0x39
#define TI954_CSI_TEST_PATT_HI     0

#define TI954_REG_CSI_TEST_PATT_LO 0x3a
#define TI954_CSI_TEST_PATT_LO     0

#define TI954_REG_SFILTER_CFG 0x41
#define TI954_SFILTER_MIN     0
#define TI954_SFILTER_MAX     4

#define TI954_REG_AEQ_CTL1   0x42
#define TI954_AEQ_SFILTER_EN 0
#define TI954_AEQ_OUTER_LOOP 1
#define TI954_AEQ_2STEP_EN   2
#define TI954_AEQ_ERR_CTL    4

#define TI954_REG_AEQ_ERR_THOLD 0x43
#define TI954_AEQ_ERR_THRESHOLD 0

#define TI954_REG_FPD3_CAP     0x4a
#define TI954_FPD3_ENC_CRC_CAP 4

#define TI954_REG_RAQ_EMBED_DTYPE 0x4b
#define TI954_EMBED_DTYPE_ID      0
#define TI954_EMBED_DTYPE_EN      6

#define TI954_REG_FPD3_PORT_SEL 0x4c
#define TI954_RX_WRITE_PORT_0   0
#define TI954_RX_WRITE_PORT_1   1
#define TI954_RX_READ_PORT      4
#define TI954_PHYS_PORT_NUM     6

#define TI954_REG_RX_PORT_STS1 0x4d
#define TI954_LOCK_STS         0
#define TI954_PORT_PASS        1
#define TI954_PARITY_ERROR     2
#define TI954_BCC_SEQ_ERROR    3
#define TI954_LOCK_STS_CHG     4
#define TI954_BCC_CRC_ERROR    5
#define TI954_RX_PORT_NUM      6

#define TI954_REG_RX_PORT_STS2  0x4e
#define TI954_LINE_CNT_CHG      0
#define TI954_CABLE_FAULT       1
#define TI954_FREQ_STABLE       2
#define TI954_CSI_ERROR         3
#define TI954_BUFFER_ERROR      4
#define TI954_FPD3_ENCODE_ERROR 5
#define TI954_LINE_LEN_CHG      6
#define TI954_LINE_LEN_UNSTABLE 7

#define TI954_REG_RX_FREQ_HIGH 0x4f
#define TI954_FREQ_CNT_HIGH    0

#define TI954_REG_RX_FERQ_LOQ 0x50
#define TI954_FREQ_CNT_LOW    0

#define TI954_REG_SENSOR_STS_0  0x51
#define TI954_VOLT0_SENSE_ALARM 0
#define TI954_VOLT1_SENSE_ALARM 1
#define TI954_TEMP_SENSE_ALARM  2
#define TI954_LINK_DETECT_ALARM 3
#define TI954_BCC_ALARM         4
#define TI954_CSI_ALARM         5

#define TI954_REG_SENSOR_STS_1  0x52
#define TI954_VOLT0_SENSE_LEVEL 0
#define TI954_VOLT1_SENSE_LEVEL 4

#define TI954_REG_SENSOR_STS_2 0x53
#define TI954_TEMP_SENSE_LEVEL 0

#define TI954_REG_SENSOR_ST_3  0x54
#define TI954_CSI_CNTRL_ERR    0
#define TI954_CSI_SYNC_ERR     1
#define TI954_CSI_SOT_ERR      2
#define TI954_CSI_CHKSUM_ERR   3
#define TI954_CSI_ECC_2BIT_ERR 4

#define TI954_REG_RX_PAR_ERR_HI 0x55
#define TI954_PAR_ERROR_BYTE_1  0

#define TI954_REG_RX_PAR_ERR_LO 0x56
#define TI954_PAR_ERROR_BYTE_0  0

#define TI954_REG_BIST_ERR_COUNT 0x57
#define TI954_BIST_ERROR_COUNT   0

#define TI954_REG_BCC_CONFIG          0x58
#define TI954_BC_FREQ_SELECT          0
#define TI954_BC_CRC_GENERAOTR_ENABLE 3
#define TI954_BC_ALWAYS_ON            4
#define TI954_AUTO_ACK_ALL            5
#define TI954_I2C_PASS_THROUGH        6
#define TI954_I2C_PASS_THROUGH_ALL    7
#define TI954_BC_FREQ_2M5             0
#define TI954_BC_FREQ_1M              2
#define TI954_BC_FREQ_25M             5
#define TI954_BC_FREQ_50M             6
#define TI954_BC_FREQ_250             7


#define TI954_REG_DATAPATH_CTL1  0x59
#define TI954_FC_GPIO_EN         0
#define TI954_OVERRIDE_FC_CONFIG 7

#define TI965_REG_DATAPATH_CTL2 0x5a

#define TI954_REG_SER_ID       0x5b
#define TI954_FREEZE_DEVICE_ID 0
#define TI954_SER_ID           1

#define TI954_REG_SER_ALIAS_ID 0x5c
#define TI954_SER_AUTO_ACK     0
#define TI954_SER_ALIAS_ID     1

#define TI954_REG_SLAVE_ID0 0x5d
#define TI954_SLAVE_ID0     1
#define TI954_REG_SLAVE_ID1 0x5e
#define TI954_SLAVE_ID1     1
#define TI954_REG_SLAVE_ID2 0x5f
#define TI954_SLAVE_ID2     1
#define TI954_REG_SLAVE_ID3 0x60
#define TI954_SLAVE_ID3     1
#define TI954_REG_SLAVE_ID4 0x61
#define TI954_SLAVE_ID4     1
#define TI954_REG_SLAVE_ID5 0x62
#define TI954_SLAVE_ID5     1
#define TI954_REG_SLAVE_ID6 0x63
#define TI954_SLAVE_ID6     1
#define TI954_REG_SLAVE_ID7 0x64
#define TI954_SLAVE_ID7     1

#define TI954_REG_ALIAS_ID0 0x65
#define TI954_ALIAS_ID0     1
#define TI954_REG_ALIAS_ID1 0x66
#define TI954_ALIAS_ID1     1
#define TI954_REG_ALIAS_ID2 0x67
#define TI954_ALIAS_ID2     1
#define TI954_REG_ALIAS_ID3 0x68
#define TI954_ALIAS_ID3     1
#define TI954_REG_ALIAS_ID4 0x644
#define TI954_ALIAS_ID4     1
#define TI954_REG_ALIAS_ID5 0x6a
#define TI954_ALIAS_ID5     1
#define TI954_REG_ALIAS_ID6 0x6b
#define TI954_ALIAS_ID6     1
#define TI954_REG_ALIAS_ID7 0x6c
#define TI954_ALIAS_ID7     1

#define TI954_REG_PORT_CONFIG 0x6d
#define TI954_FPD3_MODE       0
#define TI954_COAX_MODE       2
#define TI954_CSI_FWD_LEN     3
#define TI954_CSI_FWD_ECC     4
#define TI954_CSI_FWD_CKSUM   5
#define TI954_CSI_WAIT_FS     6
#define TI954_CSI_WAIT_FS1    7

#define TI954_REG_BC_GPIO_CTL0 0x6e
#define TI954_BC_GPIO0_SEL     0
#define TI954_BC_GPIO1_SEL     4

#define TI954_REG_BC_GPIO_CTL1 0x6f
#define TI954_BC_GPIO2_SEL     0
#define TI954_BC_GPIO3_SEL     4

#define TI954_REG_RAW10_ID 0x70
#define TI954_RAW10_DT     0
#define TI954_RAW10_VC     6

#define TI954_REG_RAW12_ID 0x71
#define TI954_RAW12_DT     0
#define TI954_RAW12_VC     6

#define TI954_REG_CSI_VC_MAP 0x72
#define TI954_CSI_VC_MAP     0

#define TI954_REG_LINE_COUNT_HI 0x73
#define TI954_LINE_COUNT_HI     0

#define TI954_REG_LINE_COUNT_LO 0x74
#define TI954_LINE_COUNT_LO     0

#define TI954_REG_LINE_LEN_1 0x750
#define TI954_LINE_LEN_HI    0

#define TI954_REG_LINE_LEN_0 0x76
#define TI954_LINE_LEN_LO    0

#define TI954_REG_FREQ_DET_CTL 0x77
#define TI954_FREW_LO_THR      0
#define TI954_FREQ_STABLE_THR  4
#define TI954_FREQ_HYST        6

#define TI954_REG_MAILBOX_1 0x78
#define TI954_MAILBOX_0     0

#define TI954_REG_MAILBOX_2 0x79
#define TI954_MAILBOX_1     0

#define TI954_REG_CSI_RX_STS 0x7a
#define TI954_ECC1_ERR       0
#define TI954_ECC2_ERR       1
#define TI954_CKSUM_ERR      2
#define TI954_LENGTH_ERR     3

#define TI954_REG_CSI_ERR_COUNTER 0x7b
#define TI954_CSI_ERR_CNT         0

#define TI954_REG_PORT_CONFIG2      0x7c
#define TI954_FV_POLARITY           0
#define TI954_LV_POLARITY           1
#define TI954_DISCARD_ON_FRAME_SIZE 3
#define TI954_DISCARD_ON_LINE_SIZE  4
#define TI954_DISCARD_ON_PAR_ERR    5
#define TI954_RAW10_8BIT_CTL        6

#define TI954_REG_PORT_PASS_CTL 0x7d
#define TI954_PASS_THRESHOLD    0
#define TI954_PASS_WDOG_DIS     2
#define TI954_PASS_PARITY_ERR   3
#define TI954_PASS_LINE_SIZE    4
#define TI954_PASS_LINE_CNT     5
#define TI954_PASS_DISCARD_EN   7

#define TI954_REG_SEN_INT_RISE_CTL 0x7e
#define TI954_SEN_INT_RISE_MASK    0

#define TI954_REG_SEN_INT_FALL_CTL 0x7f
#define TI954_SEN_INT_FALL_MASK    0

#define TI954_REG_REFCLK_FREQ 0xa5
#define TI954_REFCLK_FREQ     0

#define TI954_REG_IND_ACC_CTL 0xb0
#define TI954_IA_READ         0
#define TI954_IA_AUTO_INC     1
#define TI954_IA_SEL          2

#define TI954_REG_IND_ACC_ADDR 0xb1
#define TI954_IA_ADDR          0

#define TI954_REG_IND_ACC_DATA 0xb2
#define TI954_IA_DATA          0


#define TI954_REG_BIST_CONTROL  0xb3
#define TI954_BIST_EN           0
#define TI954_BIST_CLOCK_SOURCE 1
#define TI954_BIST_PIN_CONFIG   3
#define TI954_BIST_OUT_MODE     6

#define TI954_REG_MODE_IDX_STS 0xb8
#define TI954_MODE             0
#define TI954_MODE_DONE        1
#define TI954_IDX              4
#define TI954_IDX_DONE         7

#define TI954_REG_LINK_ERROR_COUNT 0xb9
#define TI954_LINK_ERR_THRESH      0
#define TI954_LINK_ERR_COUNT_EN    4
#define TI954_LINK_SFIL_WAIT       5

#define TI954_REG_FPD3_ENC_CTL 0xba
#define TI954_FPD3_ENC_CRC_DIS 7

#define TI954_REG_FV_MIN_TIME 0xbc
#define TI954_FRAME_VALID_MIN 0

#define TI954_REG_GPIO_PD_CTL 0xbe
#define TI954_GPIO0_PD_DIS    0
#define TI954_GPIO1_PD_DIS    1
#define TI954_GPIO2_PD_DIS    2
#define TI954_GPIO3_PD_DIS    3
#define TI954_GPIO4_PD_DIS    4
#define TI954_GPIO5_PD_DIS    5
#define TI954_GPIO6_PD_DIS    6

#define TI954_REG_PORT_DEBUG   0xd0
#define TI954_FORCE_1_BC_ERROR 0
#define TI954_FORCE_BC_ERRORS  1
#define TI954_SER_BIST_ACT     5

#define TI954_REG_AEQ_CTL2            0xd2
#define TI954_SET_AEQ_FLOOR           2
#define TI954_AEQ_RESTART             3
#define TI954_AEQ_1ST_LOCK_MODE       4
#define TI954_ADAPTIVE_EQ_RELOCK_TIME 5

#define TI954_REG_AEQ_STATUS 0xd3
#define TI954_EQ_STATUS      0

#define TI954_REG_ADAPTIVE_EQ_BYPASS  0xd4
#define TI954_ADAPTIVE_EQ_BYPASS      0
#define TI954_EQ_STAGE_2_SELECT_VALUE 1
#define TI954_AE_LOCK_MODE            4
#define TI954_EQ_STAGE_1_SELECT_VALUE 5

#define TI954_REG_AEQ_MIN_MAX         0xd5
#define TI954_ADAPTIVE_EQ_FLOOR_VALUE 0
#define TI954_AEQ_MAX                 4

#define TI954_REG_PRT_ICR_HI  0xd8
#define TI954_IE_BC_CRC_ERR   0
#define TI954_IE_BCC_SEQ_ERR  1
#define TI954_IE_FPD3_ENC_ERR 2

#define TI954_REG_PORT_ICR_LO 0xd9
#define TI954_IE_LOCK_STS     0
#define TI954_IE_PORT_PASS    1
#define TI954_IE_FPD3_PAR_ERR 2
#define TI954_IE_CSI_RX_ERR   3
#define TI954_IE_BUFFER_ERR   4
#define TI954_IE_LINE_CNT_CHG 5
#define TI954_IE_LINE_LNE_CHG 6

#define TI954_REG_PORT_ISR_HI 0xda
#define TI954_IS_BCC_CRC_ERR  0
#define TI954_IS_BCC_CEQ_ERR  1
#define TI954_IS_FPD3_ENC_ERR 2
#define TI954_IS_FC_SENS_STS  3
#define TI954_IE_FC_GPIO      4

#define TI954_REG_PORT_ISR_LO 0xdb
#define TI954_IS_LOCK_STS     0
#define TI954_IS_PORT_PASS    1
#define TI954_IS_PFD3_PAR_ERR 2
#define TI954_IS_SCI_RX_ERR   3
#define TI954_IS_BUFFER_ERR   4
#define TI954_IS_LINE_CNT_CHG 5
#define TI954_IS_LINE_LEN_CHG 6

#define TI954_REG_FC_GPIO_STS 0xdc
#define TI954_FC_GPIO0_STS    0
#define TI954_FC_GPIO1_STS    1
#define TI954_FC_GPIO2_STS    2
#define TI954_FC_GPIO3_STS    3
#define TI954_GPIO0_INT_STS   4
#define TI954_GPIO1_INT_STS   5
#define TI954_GPIO2_INT_STS   6
#define TI954_GPIO3_INT_STS   7

#define TI954_REG_FC_GPIO_ICR 0xdd
#define TI954_GPIO0_RISE_IE   0
#define TI954_GPIO0_FALL_IE   1
#define TI954_GPIO1_RISE_IE   2
#define TI954_GPIO1_FALL_IE   3
#define TI954_GPIO2_RISE_IE   4
#define TI954_GPIO2_FALL_IE   5
#define TI954_GPIO3_RISE_IE   6
#define TI954_GPIO3_FALL_IE   7

#define TI954_REG_SEN_INT_RISE_STS 0xde
#define TI954_SEN_INT_RISE         0

#define TI954_REG_SEN_INT_FALL_STS 0xdf
#define TI954_SEN_INT_FALL         0

#define TI954_REG_FPD3_RX_ID0 0xf0
#define TI954_FPD3_RX_ID0     0
#define TI954_REG_FPD3_RX_ID1 0xf1
#define TI954_FPD3_RX_ID1     0
#define TI954_REG_FPD3_RX_ID2 0xf2
#define TI954_FPD3_RX_ID2     0
#define TI954_REG_FPD3_RX_ID3 0xf3
#define TI954_FPD3_RX_ID3     0
#define TI954_REG_FPD3_RX_ID4 0xf4
#define TI954_FPD3_RX_ID4     0
#define TI954_REG_FPD3_RX_ID5 0xf5
#define TI954_FPD3_RX_ID5     0
#define TI954_RX_ID_LENGTH    6

#define TI954_REG_I2C_RX0_ID 0xf8
#define TI954_RX_PORT0_ID    1

#define TI954_REG_I2C_RX1_ID 0xf9
#define TI954_RX_PORT1_ID    1

/* Indirect Register Map Description */
#define TI954_REG_IA_PATTERN_GEN_PAGE_BLOCK_SELECT 0x0

#define TI954_REG_IA_PGEN_CTL 0x01
#define TI954_PGEB_ENABLE     0

#define TI954_REG_IA_PGEB_CFG 0x02
#define TI954_BLOCK_SIZE      0
#define TI954_NUM_CBARS       4
#define TI954_PGEN_FIXED_EN   7

#define TI954_REG_IA_PGEN_CSI_DI 0x03
#define TI954_PGEN_CSI_DT        0
#define TI954_PGEN_CSI_VC        6

#define TI954_REG_IA_PGEN_LINE_SIZE1 0x04
#define TI954_PGEN_LINE_SIZE1        0

#define TI954_REG_IA_PGEN_LINE_SIZE0 0x05
#define TI954_PGEN_LINE_SIZE0        0

#define TI954_REG_IA_PGEN_BAR_SIZE1 0x06
#define TI954_PGEN_BAR_SIZE1        0

#define TI954_REG_IA_PGEN_BAR_SIZE0 0x07
#define TI954_PGEN_BAR_SIZE0        0

#define TI954_REG_IA_PGEN_ACT_LPF1 0x08
#define TI954_PGEN_ACT_LPF1        0

#define TI954_REG_IA_PGEN_ACT_LPF0 0x09
#define TI954_PGEN_ACT_LPF0        0

#define TI954_REG_IA_PGEN_TOT_LPF1 0x0a
#define TI954_PGEN_TOT_LPF1        0

#define TI954_REG_IA_PGEN_TOT_LPF0 0x0b
#define TI954_PGEN_TOT_LPF0        0

#define TI954_REG_IA_PGEN_LINE_PD1 0x0c
#define TI954_PGEN_LINE_PD1        0

#define TI954_REG_IA_PGEN_LINE_PD0 0x0d
#define TI954_PGEN_LINE_PD0        0

#define TI954_REG_IA_PGEN_VBP 0x0e
#define TI954_PGEN_VBP        0

#define TI954_REG_IA_PGEN_VFP 0x0f
#define TI954_PGEN_VFP        0

#define TI954_REG_IA_PGEN_COLOR0  0x10
#define TI954_PGEN_COLOR0         0
#define TI954_REG_IA_PGEN_COLOR1  0x11
#define TI954_PGEN_COLOR1         0
#define TI954_REG_IA_PGEN_COLOR2  0x12
#define TI954_PGEN_COLOR2         0
#define TI954_REG_IA_PGEN_COLOR3  0x13
#define TI954_PGEN_COLOR3         0
#define TI954_REG_IA_PGEN_COLOR4  0x14
#define TI954_PGEN_COLOR4         0
#define TI954_REG_IA_PGEN_COLOR5  0x15
#define TI954_PGEN_COLOR5         0
#define TI954_REG_IA_PGEN_COLOR6  0x16
#define TI954_PGEN_COLOR6         0
#define TI954_REG_IA_PGEN_COLOR7  0x17
#define TI954_PGEN_COLOR7         0
#define TI954_REG_IA_PGEN_COLOR8  0x18
#define TI954_PGEN_COLOR8         0
#define TI954_REG_IA_PGEN_COLOR9  0x19
#define TI954_PGEN_COLOR9         0
#define TI954_REG_IA_PGEN_COLOR10 0x1a
#define TI954_PGEN_COLOR10        0
#define TI954_REG_IA_PGEN_COLOR11 0x1b
#define TI954_PGEN_COLOR11        0
#define TI954_REG_IA_PGEN_COLOR12 0x1c
#define TI954_PGEN_COLOR12        0
#define TI954_REG_IA_PGEN_COLOR13 0x1d
#define TI954_PGEN_COLOR13        0
#define TI954_REG_IA_PGEN_COLOR14 0x1e
#define TI954_PGEN_COLOR14        0

#define TI954_REG_IA_CSI0_TCK_PREP 0x40
#define TI954_MC_TCK_PREP          0
#define TI954_MC_TCK_PREP_OV       7

#define TI954_REG_IA_CSI0_TCK_ZERO 0x41
#define TI954_MC_TCK_ZERO          0
#define TI954_MC_TCK_ZERO_OV       7

#define TI954_REG_IA_CSI0_TCK_TRAIL 0x42
#define TI954_MR_TCK_TRAIL          0
#define TI954_MR_TCK_TRAIL_OV       7

#define TI954_REG_IA_CSI0_TCK_POST 0x43
#define TI954_MR_TCK_POST          0
#define TI954_MR_TCK_POST_OV       7

#define TI954_REG_IA_CSI0_THS_PREP 0x44
#define TI954_MR_THS_PREP          0
#define TI954_MR_THS_PREP_OV       7

#define TI954_REG_IA_CSI0_THS_ZERO 0x45
#define TI954_MR_THS_ZERO          0
#define TI954_MR_THS_ZERO_OV       7

#define TI954_REG_IA_CSI0_THS_TRAIL 0x46
#define TI954_MR_THS_TRAIL          0
#define TI954_MR_THS_TRIAL_OV       7

#define TI954_REG_IA_CSI0_THS_EXIT 0x47
#define TI954_MR_THS_EXIT          0
#define TI954_MR_THS_EXIT_OV       7

#define TI954_REG_IA_CSI0_TPLX 0x48
#define TI954_MR_TPLX          0
#define TI954_MR_TPLX_OV       7

/* IA test and debug registers not now defined */

/*------------------------------------------------------------------------------
 * Serializer registers
 *----------------------------------------------------------------------------*/
#define TI953_REG_I2C_DEV_ID  0x00
#define TI953_SER_ID_OVERRIDE 0
#define TI953_DEVICE_ID       1

#define TI953_REG_RESET        0x01
#define TI953_DIGITAL_RESET_0  0
#define TI953_DIGITAL_RESET_1  1
#define TI953_RESTART_AUTOLOAD 2

#define TI953_REG_GENERAL_CFG   0x02
#define TI953_I2C_STRAP_MODE    0
#define TI953_CRC_TX_GEN_ENABLE 1
#define TI953_CSI_LANE_SEL      4
#define TI953_CONTS_CLK         6
#define TI953_CSI_LANE_SEL1     0
#define TI953_CSI_LANE_SEL2     1
#define TI953_CSI_LANE_SEL4     3

#define TI953_REG_MODE_SEL 0x03
#define TI953_MODE         0
#define TI953_MODE_DONE    3
#define TI953_MODE_OV      4

#define TI953_REG_BC_MODE_SELECT  0x04
#define TI953_DVP_MODE_OVER_EN    0
#define TI953_MODE_OVERWRITE_75M  1
#define TI953_MODE_OVERWRITE_100M 2

#define TI953_REG_PLLCLK_CTL 0x05
#define TI953_OSCCLO_SEL     3
#define TI953_CLKIN_DIV      4

#define TI953_REG_CLKOUT_CTRL0 0x06
#define TI953_DIV_M_VAL        0
#define TI953_HS_CLK_DIV       5
#define TI953_HS_CLK_DIV_1     0
#define TI953_HS_CLK_DIV_2     1
#define TI953_HS_CLK_DIV_4     2
#define TI953_HS_CLK_DIV_8     3
#define TI953_HS_CLK_DIV_16    4


#define TI953_REG_CLKOUT_CTRL1 0x07
#define TI953_DIV_N_VAL        0

#define TI953_REG_BBC_WATCHDOG     0x08
#define TI953_BCC_WD_TIMER_DISABLE 0
#define TI953_BCC_WD_TIMER         1

#define TI953_REG_I2C_CONTROL1  0x09
#define TI953_I2C_FILTER_DEPTH  0
#define TI953_I2C_SDA_HOLD      4
#define TI953_LCL_WRITE_DISABLE 7

#define TI953_REG_I2C_CONTROL2      0x0a
#define TI953_I2C_BUS_TIMER_DISABLE 0
#define TI953_I2C_BUS_TIMER_SPEEDUP 1
#define TI953_SDA_OUTPUT_DELAY      2
#define TI953_SDA_OUTPUT_SETUP      4

#define TI953_REG_SCL_HIGH_TIME 0x0b
#define TI953_SCL_HIGH_TIME     0

#define TI953_REG_SCL_LOW_TIME 0x0c
#define TI953_SCL_LOW_TIME     0

#define TI953_REG_LOCAL_GPIO_DATA 0x0d
#define TI953_GPIO_OUT_SRC        0
#define TI953_GPIO_RMTEN          4

#define TI953_REG_GPIO_CTRL  0x0e
#define TI953_GPIO0_INPUT_EN 0
#define TI953_GPIO1_INPUT_EN 1
#define TI953_GPIO2_INPUT_EN 2
#define TI953_GPIO3_INPUT_EN 3
#define TI953_GPIO0_OUT_EN   4
#define TI953_GPIO1_OUT_EN   5
#define TI953_GPIO2_OUT_EN   6
#define TI953_GPIO3_OUT_EN   7

#define TI953_REG_DVP_CFG    0x10
#define TI953_DVP_LV_INV     0
#define TI953_DVP_FV_IN      1
#define TI953_DVP_DT_YUV_EN  2
#define TI953_DVP_DT_MATH_EN 3
#define TI953_DVP_DT_ANY_EN  4

#define TI953_REG_DVP_DT       0x11
#define TI953_DVP_DT_MATCH_VAL 0

#define TI953_REG_FORCE_BIST_EN 0x13
#define TI953_FORCE_FC_CNT      0
#define TI953_FORCE_FC_ERR      7

#define TI953_REG_REMOTE_BIST_CTRL 0x14
#define TI953_REMOTE_BIST_EN       0
#define TI953_BIST_CLOCK           1
#define TI953_LOCAL_BIST_EN        3
#define TI953_FORCE_ERR_CNT        4

#define TI953_REG_SENSOR_VGAIN 0x15
#define TI953_VOLT_GAIN        0

#define TI953_REG_SENSOR_CTRL0 0x17
#define TI953_SENSE_V_GPIO     0
#define TI953_SENSOR_ENABLE    2

#define TI953_REG_SENSOR_CTRL1 0x18
#define TI953_SENSE_GAIN_EN    7

#define TI953_REG_SENSOR_V0_THRESH 0x19
#define TI953_SENSE_V0_LO          0
#define TI953_SENSE_V0_HI          4

#define TI953_REG_SENSOR_V1_THRESH 0x1a
#define TI953_SENSE_V1_LO          0
#define TI953_SENSE_V1_HI          4

#define TI953_REG_SENSOR_T_THRESH 0x1b
#define TI953_SENSE_T_LO          0
#define TI953_SENSE_T_HI          4

#define TI953_REG_ALARM_CSI_EN  0x1c
#define TI953_CSI_LENGTH_ERR_EN 0
#define TI953_CSI_CHKSUM_ERR_EN 1
#define TI953_CSI_ECC_2_EN      2
#define TI953_DPHY_CTRL_ERR_EN  3
#define TI953_CSI_NO_FV_EN      5

#define TI953_REG_SENSE_EN 0x1d
#define TI953_V0_UNDER     0
#define TI953_V0_OVER      1
#define TI953_V1_UNSER     2
#define TI953_V1_OVER      3
#define TI953_T_UNDER      4
#define TI953_T_OVER       5

#define TI953_REG_ALARM_BC_EN 0x1e
#define TI953_LINK_DETECT_EN  0
#define TI953_CRC_ER_EN       1

#define TI953_REG_CSI_POL_SEL 0x20
#define TI953_POLARITY_D0     0
#define TI953_POLARITY_D1     1
#define TI953_POLARITY_D2     2
#define TI953_POLARITY_D3     3
#define TI953_POLARITY_CK0    4

#define TI953_REG_CSI_LP_POLARITY 0x21
#define TI953_POL_LP_DATA         0
#define TI953_POL_LP_CLK0         4

#define TI953_REG_CSI_EN_RXTERM 0x24
#define TI953_EN_RXTERM_D0      0
#define TI953_EN_RXTERM_D1      1
#define TI953_EN_RXTERM_D2      2
#define TI953_EN_RXTERM_D3      3

#define TI953_REG_CSI_PKT_HDR_TINT_CTRL 0x31
#define TI953_TINIT_TIME                0
#define TI953_PKT_HDR_VCI_ENABLE        4
#define TI953_PKT_HDR_CORRECTED         5
#define TI953_PKT_HDR_SEL_VC            6

#define TI953_REG_BCC_CONFIG           0x32
#define TI953_RX_PARITY_CHECKER_ENABLE 3
#define TI953_AUTO_ACK_ALL             5
#define TI953_I2C_PASS_THROUGH         6
#define TI953_I2C_PASS_THROUGH_ALL     7

#define TI953_REG_DATAPATH_CTL1 0x33
#define TI953_FC_GPIO_EN        0
#define TI953_DCA_CRC_EN        2

#define TI953_REG_DES_PAR_CAP1 0x35
#define TI953_PORT_NUM         0
#define TI953_MPORT            4
#define TI953_BIST_EN          5
#define TI953_FREEZE_DES_CAP   7

#define TI953_REG_DES_ID       0x37
#define TI953_FREEZE_DEVICE_ID 0
#define TI953_DES_ID           1

#define TI953_REG_SLAVE_ID_0 0x39
#define TI953_SLAVE_ID_0     1
#define TI953_REG_SLAVE_ID_1 0x3a
#define TI953_SLAVE_ID_1     1
#define TI953_REG_SLAVE_ID_2 0x3b
#define TI953_SLAVE_ID_2     1
#define TI953_REG_SLAVE_ID_3 0x3c
#define TI953_SLAVE_ID_3     1
#define TI953_REG_SLAVE_ID_4 0x3d
#define TI953_SLAVE_ID_4     1
#define TI953_REG_SLAVE_ID_5 0x3e
#define TI953_SLAVE_ID_5     1
#define TI953_REG_SLAVE_ID_6 0x3f
#define TI953_SLAVE_ID_6     1
#define TI953_REG_SLAVE_ID_7 0x40
#define TI953_SLAVE_ID_7     1

#define TI953_REG_SLAVE_ID_ALIAS_0 0x41
#define TI953_SLAVE_ID_ALIAS_0     1
#define TI953_REG_SLAVE_ID_ALIAS_1 0x42
#define TI953_SLAVE_ID_ALIAS_1     1
#define TI953_REG_SLAVE_ID_ALIAS_2 0x43
#define TI953_SLAVE_ID_ALIAS_2     1
#define TI953_REG_SLAVE_ID_ALIAS_3 0x44
#define TI953_SLAVE_ID_ALIAS_3     1
#define TI953_REG_SLAVE_ID_ALIAS_4 0x45
#define TI953_SLAVE_ID_ALIAS_4     1
#define TI953_REG_SLAVE_ID_ALIAS_5 0x46
#define TI953_SLAVE_ID_ALIAS_5     1
#define TI953_REG_SLAVE_ID_ALIAS_6 0x47
#define TI953_SLAVE_ID_ALIAS_6     1
#define TI953_REG_SLAVE_ID_ALIAS_7 0x48
#define TI953_SLAVE_ID_ALIAS_7     1

#define TI953_REG_CB_CTRL      0x49
#define TI953_LINK_DET_TIMER   0
#define TI953_CRC_ERR_CLR      3
#define TI953_BIST_CRC_ERR_CLR 5

#define TI953_REG_REV_MASK_ID 0x50
#define TI953_MASK_ID         0
#define TI953_REVISION_ID     4

#define TI953_REG_DEVICE_STS 0x51
#define TI953_CFG_INIT_DONE  6
#define TI953_CFG_CKSUM_STS  7

#define TI953_REG_GENERAL_STATUS 0x52
#define TI953_LINK_DET           0
#define TI953_CRC_ERR            1

#define TI953_REG_GPIO_PIN_STS 0x53
#define TI953_GPIO_STS         0

#define TI953_REG_BIST_ERR_CNT 0x54
#define TI953_BIST_BC_ERRCNT   0

#define TI953_REG_CRC_ERR_CNT1 0x55
#define TI953_CRC_ERR_CNT1     0

#define TI953_REG_CRC_ERR_CNT2 0x56
#define TI953_CRC_ERR_CNT2     0

#define TI953_REG_SENSOR_STATUS 0x57
#define TI953_V0_SENSOR_LOW     0
#define TI953_V0_SENOSR_HI      1
#define TI953_V1_SENSOR_LOW     2
#define TI953_V1_SENSOR_HI      3
#define TI953_T_SENSOR_LOW      4
#define TI953_T_SENSOR_HI       5

#define TI953_REG_SENSOR_V0         0x58
#define TI953_VOLTAGE_SENSOR_V0_MIN 0
#define TI953_VOLTAGE_SENSOR_V0_MAX 4

#define TI953_REG_SENSOR_V1         0x59
#define TI953_VOLTAGE_SENOSR_V1_MIN 0
#define TI953_VOLTAGE_SENSOR_V1_MAX 4

#define TI953_REG_SENSOR_T 0x5a
#define TI953_TEMP_MIN     0
#define TI953_TMEP_MAX     4

#define TI953_REG_CSI_ERR_CNT 0x5c
#define TI953_CSI_ERR_CNT     0

#define TI953_REG_CSI_ERR_STATUS 0x5d
#define TI953_ECC_1BIT_ERR       0
#define TI953_ECC_2BIT_ERR       1
#define TI953_CHKSUM_ERR         2
#define TI953_LINE_LEN_MISMATCH  3

#define TI953_REG_CSI_ERR_DLANE01 0x5e
#define TI953_CNTRL_ERR_HSRQST_0  1
#define TI953_SOT_SYNC_ERROR_0    2
#define TI953_SOT_ERROR_0         3
#define TI953_CNTRL_ERR_HSRQST_1  5
#define TI953_SOT_SYNC_ERROR_1    6
#define TI953_SOT_ERROR_1         7

#define TI953_REG_CSI_ERR_DLANE23 0x5f
#define TI953_CNTRL_ERR_HSRQST_2  1
#define TI953_SOT_SYNC_ERROR_2    2
#define TI953_SOT_ERROR_2         3
#define TI953_CNTRL_ERR_HSRQST_3  5
#define TI953_SOT_SYNC_ERROR_3    6
#define TI953_SOT_ERROR_3         7

#define TI953_REG_CSI_ERR_CLK_LANE 0x60
#define TI953_CNTRL_ERR_HSRQST_CK0 1

#define TI953_REG_CSI_PKT_HDR_VC_ID 0x61
#define TI953_LONG_PKT_DATA_ID      0
#define TI953_LONG_PKT_VCHNL_ID     6

#define TI953_REG_PKT_HDR_WC_LSB   0x62
#define TI953_LONG_PKT_WRD_CNT_LSB 0

#define TI953_REG_PKT_HDR_WC_MSB   0x63
#define TI953_LONG_PKT_WRD_CNT_MSB 0

#define TI953_REG_CSI_ECC        0x64
#define TI953_CSI2_ECC           0
#define TI953_LINE_LENGTH_CHANGE 7

#define TI953_REG_IND_ACC_CTL 0xb0
#define TI953_IA_READ         0
#define TI953_IA_AUTO_INC     1
#define TI953_IA_SEL          2

#define TI953_REG_IND_ACC_ADDR 0xb1
#define TI953_IND_ACC_ADDR     0

#define TI953_REG_IND_ACC_DATA 0xb2
#define TI953_IND_ACC_DATA     0

#define TI953_REG_FPD3_RX_ID0 0xf0
#define TI953_FPD3_RX_ID0     0
#define TI953_REG_FPD3_RX_ID1 0xf1
#define TI953_FPD3_RX_ID1     0
#define TI953_REG_FPD3_RX_ID2 0xf2
#define TI953_FPD3_RX_ID2     0
#define TI953_REG_FPD3_RX_ID3 0xf3
#define TI953_FPD3_RX_ID3     0
#define TI953_REG_FPD3_RX_ID4 0xf4
#define TI953_FPD3_RX_ID4     0
#define TI953_REG_FPD3_RX_ID5 0xf5
#define TI953_FPD3_RX_ID5     0
#define TI953_RX_ID_LENGTH    6

/*------------------------------------------------------------------------------
 * DEFINES
 *----------------------------------------------------------------------------*/
// GPIO
#define CAM_ENABLE 0x01   /* Assumes bit 0 of GPIO is connected to CAM_ENABLE */
#define GPIO_DEVICE_ID  XPAR_GPIO_0_DEVICE_ID
#define CAM_ENABLE_CHANNEL 1
// IIC
#define IIC_BASEADDR            XPAR_AXI_IIC_0_BASEADDR
#define IIC_DEVICE_ID           XPAR_IIC_0_DEVICE_ID
// DESERIALIZER
#define TI954_I2C               0x30    // ID of deserializer
// SERIALIZER
#define TI953_I2C_0             0x41    // alias of serializer on port 0
#define TI953_I2C_1             0x42    // alias of serializer on port 0
// sensor
#define SENSOR_I2C_0            0x43
#define SENSOR_I2C_1            0x44
// #define SENSOR_I2C_ALIAS      	0x20
#define SENSOR_ID               0x1A //0x50
// EEPROM
#define EEPROM_ID               0x55
#define EEPROM_I2C_0            0x45
#define EEPROM_I2C_1            0x46

#define NUM_SERIALIZER 2
#define NUM_ALIAS 8

struct ds90ub953_priv {
	struct i2c_client *client;
	struct regmap *regmap;
	struct ds90ub954_priv *parent;
	int rx_channel;
	int test_pattern;
	int i2c_address;
	int csi_lane_count;
	int i2c_alias_num; // number of slave alias pairs
	int i2c_slave[NUM_ALIAS]; // array with the i2c slave addresses
	int i2c_alias[NUM_ALIAS]; // array with the i2c alias addresses
	int conts_clk; // continuous clock (0: discontinuous, 1: continuous)
	int i2c_pt; // i2c-pass-through-all

	int initialized;

	int gpio0_oe; // gpio0_output_enable
	int gpio1_oe; // gpio1_output_enable
	int gpio2_oe; // gpio2_output_enable
	int gpio3_oe; // gpio3_output_enable

	int gpio0_oc; // gpio0_output_control
	int gpio1_oc; // gpio1_output_control
	int gpio2_oc; // gpio2_output_control
	int gpio3_oc; // gpio3_output_control

	/* reference output clock control parameters */
	int hs_clk_div;
	int div_m_val;
	int div_n_val;

	int vc_map; // virtual channel mapping
};


struct ds90ub954_priv {
	struct i2c_client *client;
	struct regmap *regmap;
	struct ds90ub953_priv *ser[NUM_SERIALIZER]; //serializers
	int pass_gpio;
	int lock_gpio;
	int pdb_gpio;
	int sel_rx_port; // selected rx port
	int sel_ia_config; // selected ia configuration
	int csi_lane_count;
	int csi_lane_speed;
	int test_pattern;
	int num_ser; // number of serializers connected
	int conts_clk; // continuous clock (0: discontinuous, 1: continuous)
};

#endif /* I2C_DS90UB954_H */
