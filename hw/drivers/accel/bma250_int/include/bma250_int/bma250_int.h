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

#ifndef __SENSOR_BMA250_INT_H__
#define __SENSOR_BMA250_INT_H__

#include "os/os.h"
#include "os/os_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

enum bma250_int_map_0 {
    BMA250_INT_ORIENTATION_PORTRAIT     = 0x00,
    BMA250_INT_ORIENTATION_LANDCAPE     = 0x01,
    BMA250_INT_ORIENTATION_UPRIGHT_LEFT = 0x02,
    BMA250_INT_ORIENTATION_UPSIDE_RIGHT = 0x03,
    BMA250_INT_ORIENTATION_Z_UP         = 0x04,
    BMA250_INT_ORIENTATION_Z_DOWN       = 0x05,
    BMA250_INT_ORIENTATION_FLAT         = 0x06,
    BMA250_INT_ORIENTATION_NON_FLAT     = 0x07,
    BMA250_INT_TAP_NO_SINGLE            = 0x08,
    BMA250_INT_TAP_SINGLE               = 0x09,
    BMA250_INT_TAP_NO_DOUBLE            = 0x0A,
    BMA250_INT_TAP_DOUBLE               = 0x0B,
    BMA250_INT_SLOPE_NO_MOTION          = 0x0C,
    BMA250_INT_SLOPE_MOTION             = 0x0D
};

enum bm250_int_mode {
    BMA250_INT_MODE_OFF                 = 0x00,
    BMA250_INT_MODE_TAP                 = 0x01,
    BMA250_INT_MODE_ORIENTATION         = 0x02,
    BMA250_INT_MODE_SLOPE               = 0x03
};

struct bm250_int_int_event {
    enum bma250_int_map_0 event;
};

struct bm250_int_cfg {
    enum bm250_int_mode mode;
    os_event_fn *int_cb;
};

int bma250_int_config(struct bm250_int_cfg *);

int bma250_int_init(struct os_dev *, void *);

#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_BMA250_INT_H__ */
