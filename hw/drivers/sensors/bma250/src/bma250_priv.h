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

#ifndef __BMA250_PRIV_H__
#define __BMA250_PRIV_H__

#define BMA250_ADDR_ACCEL                     (0x18) /* 0011000 */

#ifdef __cplusplus
extern "C" {
#endif

enum bma250_registers {                         /* TYPE */
    BMA250_REGISTER_BGW_CHIPID          = 0x00, /* r   */
    /* RESERVED */
    BMA250_REGISTER_ACCD_X_LSB          = 0x02, /* r  */
    BMA250_REGISTER_ACCD_X_MSB          = 0x03, /* r  */
    BMA250_REGISTER_ACCD_Y_LSB          = 0x04, /* r  */
    BMA250_REGISTER_ACCD_Y_MSB          = 0x05, /* r  */
    BMA250_REGISTER_ACCD_Z_LSB          = 0x06, /* r  */
    BMA250_REGISTER_ACCD_Z_MSB          = 0x07, /* r  */
    BMA250_REGISTER_ACCD_TEMP           = 0x08, /* r  */
    BMA250_REGISTER_INT_STATUS_0        = 0x09, /* r  */
    BMA250_REGISTER_INT_STATUS_1        = 0x0A, /* r  */
    BMA250_REGISTER_INT_STATUS_2        = 0x0B, /* r  */
    BMA250_REGISTER_INT_STATUS_3        = 0x0C, /* r  */
    /* RESERVED */
    BMA250_REGISTER_FIFO_STATUS         = 0x0E, /* r  */
    BMA250_REGISTER_PMU_RANGE           = 0x0F, /* rw */
    BMA250_REGISTER_PMU_BW              = 0x10, /* rw */
    BMA250_REGISTER_PMU_LPW             = 0x11, /* rw */
    BMA250_REGISTER_PMU_LOW_NOISE       = 0x12, /* rw */
    BMA250_REGISTER_ACCD_HBW            = 0x13, /* rw */
    BMA250_REGISTER_BGW_SOFTRESET       = 0x14, /* w  */
    /* RESERVED */
    BMA250_REGISTER_INT_EN_0            = 0x16, /* rw */
    BMA250_REGISTER_INT_EN_1            = 0x17, /* rw */
    BMA250_REGISTER_INT_EN_2            = 0x18, /* rw */
    BMA250_REGISTER_INT_MAP_0           = 0x19, /* rw */
    BMA250_REGISTER_INT_MAP_1           = 0x1A, /* rw */
    BMA250_REGISTER_INT_MAP_2           = 0x1B, /* rw */
    /* RESERVED */
    /* RESERVED */
    BMA250_REGISTER_INT_SRC             = 0x1E, /* rw */
    /* RESERVED */
    BMA250_REGISTER_INT_OUT_CTRL        = 0x20, /* rw */
    BMA250_REGISTER_INT_RST_LATCH       = 0x21, /* rw */
    BMA250_REGISTER_INT_0               = 0x22, /* rw */
    BMA250_REGISTER_INT_1               = 0x23, /* rw */
    BMA250_REGISTER_INT_2               = 0x24, /* rw */
    BMA250_REGISTER_INT_3               = 0x25, /* rw */
    BMA250_REGISTER_INT_4               = 0x26, /* rw */
    BMA250_REGISTER_INT_5               = 0x27, /* rw */
    BMA250_REGISTER_INT_6               = 0x28, /* rw */
    BMA250_REGISTER_INT_7               = 0x29, /* rw */
    BMA250_REGISTER_INT_8               = 0x2A, /* rw */
    BMA250_REGISTER_INT_9               = 0x2B, /* rw */
    BMA250_REGISTER_INT_A               = 0x2C, /* rw */
    BMA250_REGISTER_INT_B               = 0x2D, /* rw */
    BMA250_REGISTER_INT_C               = 0x2E, /* rw */
    BMA250_REGISTER_INT_D               = 0x2F, /* rw */
    BMA250_REGISTER_FIFO_CONFIG_0       = 0x30, /* rw */
    /* RESERVED */
    BMA250_REGISTER_PMU_SELF_TEST       = 0x32, /* rw */
    BMA250_REGISTER_TRIM_NVM_CTRL       = 0x33, /* rw */
    BMA250_REGISTER_BGW_SPI3_WDT        = 0x34, /* rw */
    /* RESERVED */
    BMA250_REGISTER_OFC_CTRL            = 0x36, /* rw */
    BMA250_REGISTER_OFC_SETTING         = 0x37, /* rw */
    BMA250_REGISTER_OFC_OFFSET_X        = 0x38, /* rw */
    BMA250_REGISTER_OFC_OFFSET_Y        = 0x39, /* rw */
    BMA250_REGISTER_OFC_OFFSET_Z        = 0x3A, /* rw */
    BMA250_REGISTER_TRIM_GP0            = 0x3B, /* rw */
    BMA250_REGISTER_TRIM_GP1            = 0x3C, /* rw */
    /* RESERVED */
    BMA250_REGISTER_FIFO_CONFIG_1       = 0x3E, /* rw */
    BMA250_REGISTER_FIFO_DATA           = 0x3F  /* r  */
};


int bma250_write8(uint8_t addr, uint8_t reg, uint32_t value);
int bma250_read8(uint8_t addr, uint8_t reg, uint8_t *value);
int bma250_read48(uint8_t addr, uint8_t reg, uint8_t *buffer);

#ifdef __cplusplus
}
#endif

#endif /* __BMA250_PRIV_H__ */
