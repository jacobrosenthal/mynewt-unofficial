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

#ifndef __SENSOR_BMA250_H__
#define __SENSOR_BMA250_H__

#include "os/os.h"
#include "os/os_dev.h"
#include "sensor/sensor.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BMA250_VALUE_BGW_CHIPID 0xF9

enum bma250_accel_range {
    BMA250_ACCEL_RANGE_2            = 0x03, /* +/- 2g  */
    BMA250_ACCEL_RANGE_4            = 0x05, /* +/- 4g  */
    BMA250_ACCEL_RANGE_8            = 0x08, /* +/- 8g  */
    BMA250_ACCEL_RANGE_16           = 0x0C  /* +/- 16g */
};

enum bma250_accel_rate {
    BMA250_ACCEL_RATE_781           = 0x08, /* 7.81hz  */
    BMA250_ACCEL_RATE_1563          = 0x09, /* 15.63hz */
    BMA250_ACCEL_RATE_3125          = 0x0A, /* 31.25hz */
    BMA250_ACCEL_RATE_6250          = 0x0B, /* 62.5hz  */
    BMA250_ACCEL_RATE_12500         = 0x0C, /* 125hz   */
    BMA250_ACCEL_RATE_25000         = 0x0D, /* 250hz   */
    BMA250_ACCEL_RATE_50000         = 0x0E, /* 500hz   */
    BMA250_ACCEL_RATE_100000        = 0x0F  /* 1000hz  */
};

// enum bma250_int_map_0 {
//     BMA250_INT1_LOW                 = 0x00,
//     BMA250_INT1_HIGH                = 0x01,
//     BMA250_INT1_SLOPE               = 0x02,
//     BMA250_INT1_SLO_NO_MOT          = 0x03,
//     BMA250_INT1_D_TAP               = 0x04,
//     BMA250_INT1_S_TAP               = 0x05,
//     BMA250_INT1_ORIENT              = 0x06,
//     BMA250_INT1_FLAT                = 0x07
// };

// enum bma250_int_map_1 {
//     BMA250_INT1_DATA                = 0x00,
//     BMA250_INT1_FWM                 = 0x01,
//     BMA250_INT1_FFULL               = 0x02,
//     BMA250_INT2_FFULL               = 0x05,
//     BMA250_INT2_FWM                 = 0x06,
//     BMA250_INT2_DATA                = 0x07
// };

// enum bma250_int_map_2 {
//     BMA250_INT2_LOW                 = 0x00,
//     BMA250_INT2_HIGH                = 0x01,
//     BMA250_INT2_SLOPE               = 0x02,
//     BMA250_INT2_SLO_NO_MOT          = 0x03,
//     BMA250_INT2_D_TAP               = 0x04,
//     BMA250_INT2_S_TAP               = 0x05,
//     BMA250_INT2_ORIENT              = 0x06,
//     BMA250_INT2_FLAT                = 0x07
// };

struct bma250_cfg {
    enum bma250_accel_range accel_range;
    enum bma250_accel_rate accel_rate;
};

struct bma250 {
    struct os_dev dev;
    struct sensor sensor;
    struct bma250_cfg cfg;
    os_time_t last_read_time;
};

int bma250_init(struct os_dev *, void *);
int bma250_config(struct bma250 *, struct bma250_cfg *);

#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_BMA250_H__ */
