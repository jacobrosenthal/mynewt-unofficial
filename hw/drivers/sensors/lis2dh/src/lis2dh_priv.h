/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef __LIS2DH_PRIV_H__
#define __LIS2DH_PRIV_H__

#include "lis2dh/lis2dh.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LIS2DH_READ 0x80
#define LIS2DH_MULTIPLE 0x40
#define LIS2DH_ID 0x33
#define SPI_BAUDRATE 8000

#define LIS2DH_REGISTER_CLICK_CFG_XS            1 << 0
#define LIS2DH_REGISTER_CLICK_CFG_XD            1 << 1
#define LIS2DH_REGISTER_CLICK_CFG_YS            1 << 2
#define LIS2DH_REGISTER_CLICK_CFG_YD            1 << 3
#define LIS2DH_REGISTER_CLICK_CFG_ZD            1 << 4
#define LIS2DH_REGISTER_CLICK_CFG_ZS            1 << 5

#define LIS2DH_REGISTER_CLICK_SRC_X             1 << 0
#define LIS2DH_REGISTER_CLICK_SRC_Y             1 << 1
#define LIS2DH_REGISTER_CLICK_SRC_Z             1 << 2
#define LIS2DH_REGISTER_CLICK_SRC_Sign          1 << 3
#define LIS2DH_REGISTER_CLICK_SRC_SClick        1 << 4
#define LIS2DH_REGISTER_CLICK_SRC_DClick        1 << 5
#define LIS2DH_REGISTER_CLICK_SRC_IA            1 << 6

#define LIS2DH_REGISTER_CTRL_REG1_XEN           1 << 0
#define LIS2DH_REGISTER_CTRL_REG1_YEN           1 << 1
#define LIS2DH_REGISTER_CTRL_REG1_ZEN           1 << 2
#define LIS2DH_REGISTER_CTRL_REG1_LPEN          1 << 3
#define LIS2DH_REGISTER_CTRL_REG1_ODR           1 << 4

#define LIS2DH_REGISTER_CTRL_REG2_HPIS          1 << 0
#define LIS2DH_REGISTER_CTRL_REG2_HPCLICK       1 << 2
#define LIS2DH_REGISTER_CTRL_REG2_FDS           1 << 3
#define LIS2DH_REGISTER_CTRL_REG2_HPCF          1 << 4
#define LIS2DH_REGISTER_CTRL_REG2_HPM           1 << 6

#define LIS2DH_REGISTER_CTRL_REG3_I1_OVERRUN    1 << 1
#define LIS2DH_REGISTER_CTRL_REG3_I1_WTM        1 << 2
#define LIS2DH_REGISTER_CTRL_REG3_I1_DRDY       1 << 3
#define LIS2DH_REGISTER_CTRL_REG3_I1_AOI        1 << 5
#define LIS2DH_REGISTER_CTRL_REG3_I1_CLICK      1 << 7

#define LIS2DH_REGISTER_CTRL_REG4_SIM           1 << 0
#define LIS2DH_REGISTER_CTRL_REG4_ST            1 << 1
#define LIS2DH_REGISTER_CTRL_REG4_HR            1 << 3
#define LIS2DH_REGISTER_CTRL_REG4_FS            1 << 4
#define LIS2DH_REGISTER_CTRL_REG4_BLE           1 << 6
#define LIS2DH_REGISTER_CTRL_REG4_BDU           1 << 7

#define LIS2DH_REGISTER_CTRL_REG5_D4D_INT2      1 << 0
#define LIS2DH_REGISTER_CTRL_REG5_LIR_INT2      1 << 1
#define LIS2DH_REGISTER_CTRL_REG5_D4D_INT1      1 << 2
#define LIS2DH_REGISTER_CTRL_REG5_LIR_INT1      1 << 3
#define LIS2DH_REGISTER_CTRL_REG5_LIR_FIFO_EN   1 << 6
#define LIS2DH_REGISTER_CTRL_REG5_LIR_BOOT      1 << 7

#define LIS2DH_REGISTER_CTRL_REG6_H_LACTIVE     1 << 1
#define LIS2DH_REGISTER_CTRL_REG6_P2_ACT        1 << 3
#define LIS2DH_REGISTER_CTRL_REG6_BOOT_I2       1 << 4
#define LIS2DH_REGISTER_CTRL_REG6_I2_INT2       1 << 5
#define LIS2DH_REGISTER_CTRL_REG6_I2_INT1       1 << 6
#define LIS2DH_REGISTER_CTRL_REG6_I2_CLICK      1 << 7


enum lis2dh_filter_mode {
    LIS2DH_REGISTER_CTRL_REG2_HPM_NORMAL_RESET  = (0x00 << 6),
    LIS2DH_REGISTER_CTRL_REG2_HPM_REFERENCE     = (0x01 << 6),
    LIS2DH_REGISTER_CTRL_REG2_HPM_NORMAL        = (0x02 << 6),
    LIS2DH_REGISTER_CTRL_REG2_HPM_AUTORESET     = (0x03 << 6)
};

enum lis2dh_temp_mode {
    LIS2DH_REGISTER_TEMP_CFG_REG_OFF            = (0x00 << 6),
    LIS2DH_REGISTER_TEMP_CFG_REG_ON             = (0x03 << 6)
};

enum lis2dh_self_test_mode {
    LIS2DH_REGISTER_CTRL_REG4_SELF_TEST_NORMAL  = (0x00 << 1),
    LIS2DH_REGISTER_CTRL_REG4_SELF_TEST_0       = (0x01 << 1),
    LIS2DH_REGISTER_CTRL_REG4_SELF_TEST_1       = (0x02 << 1)
};

enum lis2dh_fifo_mode {
    LIS2DH_REGISTER_FIFO_CTRL_REG_FM_BYPASS     = (0x00 << 6),
    LIS2DH_REGISTER_FIFO_CTRL_REG_FM_FIFO       = (0x01 << 6),
    LIS2DH_REGISTER_FIFO_CTRL_REG_FM_STREAM     = (0x02 << 6),
    LIS2DH_REGISTER_FIFO_CTRL_REG_FM_TRIGGER    = (0x03 << 6)
};

enum lis2dh_interrupt_mode {
    LIS2DH_REGISTER_INT_CFG_OR                 = (0x00 << 6),
    LIS2DH_REGISTER_INT_CFG_6D_MOVEMENT        = (0x01 << 6),
    LIS2DH_REGISTER_INT_CFG_AND                = (0x02 << 6),
    LIS2DH_REGISTER_INT_CFG_6D_POSITION        = (0x03 << 6)
};

enum LIS2DH_registers {                         /* TYPE */
    /* RESERVED */
    LIS2DH_REGISTER_STATUS_REG_AUX      = 0x07, /* r  */
    /* RESERVED */
    LIS2DH_REGISTER_OUT_TEMP_L          = 0x0C, /* r  */
    LIS2DH_REGISTER_OUT_TEMP_H          = 0x0D, /* r  */
    LIS2DH_REGISTER_INT_COUNTER_REG     = 0x0E, /* r  */
    LIS2DH_REGISTER_WHO_AM_I            = 0x0F, /* r  */
    /* RESERVED */
    LIS2DH_REGISTER_TEMP_CFG_REG        = 0x1F, /* rw */
    LIS2DH_REGISTER_CTRL_REG1           = 0x20, /* rw */
    LIS2DH_REGISTER_CTRL_REG2           = 0x21, /* rw */
    LIS2DH_REGISTER_CTRL_REG3           = 0x22, /* rw */
    LIS2DH_REGISTER_CTRL_REG4           = 0x23, /* rw */
    LIS2DH_REGISTER_CTRL_REG5           = 0x24, /* rw */
    LIS2DH_REGISTER_CTRL_REG6           = 0x25, /* rw */
    LIS2DH_REGISTER_REFERENCE           = 0x26, /* rw */
    LIS2DH_REGISTER_STATUS_REG2         = 0x27, /* r  */
    LIS2DH_REGISTER_OUT_X_L             = 0x28, /* r  */
    LIS2DH_REGISTER_OUT_X_H             = 0x29, /* r  */
    LIS2DH_REGISTER_OUT_Y_L             = 0x2A, /* r  */
    LIS2DH_REGISTER_OUT_Y_H             = 0x2B, /* r  */
    LIS2DH_REGISTER_OUT_Z_L             = 0x2C, /* r  */
    LIS2DH_REGISTER_OUT_Z_H             = 0x2D, /* r  */
    LIS2DH_REGISTER_FIFO_CTRL_REG       = 0x2E, /* rw */
    LIS2DH_REGISTER_FIFO_SRC_REG        = 0x2F, /* r  */
    LIS2DH_REGISTER_INT1_CFG            = 0x30, /* rw */
    LIS2DH_REGISTER_INT1_SOURCE         = 0x31, /* r  */
    LIS2DH_REGISTER_INT1_THS            = 0x32, /* rw */
    LIS2DH_REGISTER_INT1_DURATION       = 0x33, /* rw */
    LIS2DH_REGISTER_INT2_CFG            = 0x34, /* rw */
    LIS2DH_REGISTER_INT2_SOURCE         = 0x35, /* r  */
    LIS2DH_REGISTER_INT2_THS            = 0x36, /* rw */
    LIS2DH_REGISTER_INT2_DURATION       = 0x37, /* rw */
    LIS2DH_REGISTER_CLICK_CFG           = 0x38, /* rw */
    LIS2DH_REGISTER_CLICK_SRC           = 0x39, /* r  */
    LIS2DH_REGISTER_CLICK_THS           = 0x3A, /* rw */
    LIS2DH_REGISTER_TIME_LIMIT          = 0x3B, /* rw */
    LIS2DH_REGISTER_TIME_LATENCY        = 0x3C, /* rw */
    LIS2DH_REGISTER_TIME_WINDOW         = 0x3D, /* rw */
    LIS2DH_REGISTER_Act_THS             = 0x3E, /* rw */
    LIS2DH_REGISTER_Act_DUR             = 0x3F, /* rw */
};

int
lis2dh_write8(uint8_t reg, uint8_t value);

int
lis2dh_readlen(uint8_t reg, uint8_t *value, uint8_t length);

int
lis2dh_read8(uint8_t reg, uint8_t *value);

int
lis2dh_clear8(uint8_t reg, uint8_t value);

int
lis2dh_get_click_cfg(struct lis2dh_cfg *cfg, uint8_t *value);

int
lis2dh_accel_configure(struct lis2dh_cfg *cfg);

int
lis2dh_click_configure(struct lis2dh_cfg *cfg);

int
lis2dh_verify_id();

#ifdef __cplusplus
}
#endif

#endif /* __LIS2DH_PRIV_H__ */
