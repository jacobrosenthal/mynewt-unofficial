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

#ifndef H_BSP_H
#define H_BSP_H

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Define special stackos sections */
#define sec_data_core   __attribute__((section(".data.core")))
#define sec_bss_core    __attribute__((section(".bss.core")))

/* More convenient section placement macros. */
#define bssnz_t

extern uint8_t _ram_start;
#define RAM_SIZE            0x8000

#define BMA250_INT_1        12
#define BMA250_INT_2        13
#define BMA250_INT_3        14
#define BMA250_INT_4        15
#define BMA250_INT_5        16
#define BMA250_INT_6        -1

#define IQS263_RDY          5

#define SSD1306_DC          30
#define SSD1306_RESET       0
#define SSD1306_SS_PIN      2

#define W25Q80BL_SS_PIN     17

#define BATTERY_READ_PIN    1
#define PLUG_DETECT_PIN     6
#define CHARGE_DETECT_PIN   23

#define TEST_POINT_1        21
#define TEST_POINT_2        22

/* LED pins */
#define LED_1               -1
#define MOTOR_1             7

#define LED_BLINK_PIN       MOTOR_1

#define NFFS_AREA_MAX       (8)


#ifdef __cplusplus
}
#endif

#endif  /* H_BSP_H */
