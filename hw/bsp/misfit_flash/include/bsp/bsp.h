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

#ifdef __cplusplus
extern "C" {
#endif

#include <syscfg/syscfg.h>

#include "inttypes.h"
/* Define special stackos sections */
#define sec_data_core   __attribute__((section(".data.core")))
#define sec_bss_core    __attribute__((section(".bss.core")))

/* More convenient section placement macros. */
#define bssnz_t

extern uint8_t _ram_start;
#define RAM_SIZE        0x4000

#define LIS2DH_INT_1    0
#define LIS2DH_INT_2    1

#define LIS2DH_SS_PIN	2

/* LED pins */
#define LED_1          	21
#define LED_2          	25
#define LED_3          	23
#define LED_4          	19
#define LED_5          	18
#define LED_6          	12
#define LED_7          	10
#define LED_8          	9
#define LED_9          	30
#define LED_10         	28
#define LED_11         	29
#define LED_12         	22
#define LED_BLINK_PIN   LED_1

#define BUTTON_1 		17
#define BUTTON_1_PIN_CFG HAL_GPIO_PULL_NONE

/* UART info */
#define CONSOLE_UART    "uart0"

#if MYNEWT_VAL(BOOT_SERIAL)
#define BOOT_SERIAL_DETECT_PIN          BUTTON_1 /* Button 1 */
#define BOOT_SERIAL_DETECT_PIN_CFG      BUTTON_1_PIN_CFG
#define BOOT_SERIAL_DETECT_PIN_VAL      0

#define BOOT_SERIAL_REPORT_PIN          LED_BLINK_PIN
#define BOOT_SERIAL_REPORT_FREQ         (MYNEWT_VAL(OS_CPUTIME_FREQ) / 4)
#endif

#define NFFS_AREA_MAX   (8)

#ifdef __cplusplus
}
#endif

#endif  /* H_BSP_H */
