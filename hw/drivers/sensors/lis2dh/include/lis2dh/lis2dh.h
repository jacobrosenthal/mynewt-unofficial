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

#ifndef __SENSOR_LIS2DH_H__
#define __SENSOR_LIS2DH_H__

#include "os/os.h"
#include "os/os_dev.h"
#include "sensor/sensor.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Power modes */
enum lis2dh_pwr_mode {
    LIS2DH_PWR_MODE_SUSPEND         = 0X00,
    LIS2DH_PWR_MODE_NORMAL          = 0X01,
    LIS2DH_PWR_MODE_LOWPOWER        = 0X02,
    LIS2DH_PWR_MODE_HIGHRESOLUTION  = 0X03
};

//CTRL_REG4
//FS1 FS0
// BDU BLE FS1 FS0 HR ST1 ST0 SIM
enum lis2dh_accel_range {
    LIS2DH_ACCEL_RANGE_2            = (0x00 << 4), /* +/- 2g  */
    LIS2DH_ACCEL_RANGE_4            = (0x01 << 4), /* +/- 4g  */
    LIS2DH_ACCEL_RANGE_8            = (0x02 << 4), /* +/- 8g  */
    LIS2DH_ACCEL_RANGE_16           = (0x03 << 4) /* +/- 16g */
};

//CTRL_REG1
//ODR<3:0>
enum lis2dh_accel_rate {
    LIS2DH_ACCEL_RATE_OFF           = (0x00 << 4), /* Power Down     */
    LIS2DH_ACCEL_RATE_1             = (0x01 << 4), /* 1hz     */
    LIS2DH_ACCEL_RATE_10            = (0x02 << 4), /* 10hz    */
    LIS2DH_ACCEL_RATE_25            = (0x03 << 4), /* 25hz    */
    LIS2DH_ACCEL_RATE_50            = (0x04 << 4), /* 50hz    */
    LIS2DH_ACCEL_RATE_100           = (0x05 << 4), /* 100hz   */
    LIS2DH_ACCEL_RATE_200           = (0x06 << 4), /* 200hz   */
    LIS2DH_ACCEL_RATE_400           = (0x07 << 4), /* 400hz   */
    LIS2DH_ACCEL_RATE_1620          = (0x08 << 4), /* 1620hz  */
    LIS2DH_ACCEL_RATE_1344_5376     = (0x09 << 4)  /* 1344hz HRnormal / 5376hz Low power mode  */
};

enum lis2dh_click_mode {
    LIS2DH_CLICK_OFF                = 0x00,
    LIS2DH_CLICK_SINGLE_POSITIVE    = 0x01,
    LIS2DH_CLICK_SINGLE_NEGATIVE    = 0x02,
    LIS2DH_CLICK_SINGLE_BOTH        = 0x03,
    LIS2DH_CLICK_DOUBLE_POSITIVE    = 0x04,
    LIS2DH_CLICK_DOUBLE_NEGATIVE    = 0x05,
    LIS2DH_CLICK_DOUBLE_BOTH        = 0x06,
};

enum lis2dh_click_dir {
    LIS2DH_CLICK_X                  = 0x00,
    LIS2DH_CLICK_Y                  = 0x01,
    LIS2DH_CLICK_Z                  = 0x02,
    LIS2DH_CLICK_ALL                = 0x03,
};

struct lis2dh_click_event {
    enum lis2dh_click_mode mode;
    enum lis2dh_click_dir direction;
};

#if MYNEWT_VAL(LIS2DH_CLI)
int
lis2dh_shell_init(void);
#endif

struct lis2dh_cfg {
    enum lis2dh_pwr_mode mode;
    enum lis2dh_accel_range accel_range;
    enum lis2dh_accel_rate accel_rate;
    enum lis2dh_click_mode click_mode;
    enum lis2dh_click_dir click_direction;
    //single
    uint8_t click_threshold;
    uint8_t click_time_limit;
    //double
    uint8_t click_time_window;
    uint8_t click_time_latency;
    os_event_fn *click_cb;
};

struct lis2dh {
    struct os_dev dev;
    struct sensor sensor;
    struct lis2dh_cfg cfg;
    os_time_t last_read_time;
};

int
lis2dh_init(struct os_dev *, void *);

int
lis2dh_config(struct lis2dh *, struct lis2dh_cfg *);

int
lis2dh_default_cfg(struct lis2dh_cfg *cfg);

int
lis2dh_get_vector_data(void *datastruct, struct lis2dh *lis);

/**
 * Get temperature from bno055 sensor
 *
 * @return temperature in degree celcius
 */
int
lis2dh_get_temp(int8_t *temp);

/**
 * Get chip ID from the sensor
 *
 * @param Pointer to the variable to fill up chip ID in
 * @return 0 on success, non-zero on failure
 */
int
lis2dh_get_chip_id(uint8_t *id);

#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_LIS2DH_H__ */
