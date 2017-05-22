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

#include <string.h>
#include <errno.h>
#include <assert.h>

#include "defs/error.h"
#include "os/os.h"
#include "sysinit/sysinit.h"
#include "bsp/bsp.h"
#include "hal/hal_gpio.h"
#include "hal/hal_spi.h"
#include "sensor/sensor.h"
#include "sensor/accel.h"
#include "lis2dh/lis2dh.h"
#include "lis2dh_priv.h"

#if MYNEWT_VAL(LIS2DH_LOG)
#include "log/log.h"
#endif
#if MYNEWT_VAL(LIS2DH_STATS)
#include "stats/stats.h"
#endif

#if MYNEWT_VAL(LIS2DH_STATS)
/* Define the stats section and records */
STATS_SECT_START(lis2dh_stat_section)
STATS_SECT_ENTRY(samples_acc_2g)
STATS_SECT_ENTRY(samples_acc_4g)
STATS_SECT_ENTRY(samples_acc_8g)
STATS_SECT_ENTRY(samples_acc_16g)
STATS_SECT_ENTRY(errors)
STATS_SECT_END

/* Define stat names for querying */
STATS_NAME_START(lis2dh_stat_section)
STATS_NAME(lis2dh_stat_section, samples_acc_2g)
STATS_NAME(lis2dh_stat_section, samples_acc_4g)
STATS_NAME(lis2dh_stat_section, samples_acc_8g)
STATS_NAME(lis2dh_stat_section, samples_acc_16g)
STATS_NAME(lis2dh_stat_section, errors)
STATS_NAME_END(lis2dh_stat_section)

/* Global variable used to hold stats data */
STATS_SECT_DECL(lis2dh_stat_section) g_lis2dhstats;
#endif

#if MYNEWT_VAL(LIS2DH_LOG)
#define LOG_MODULE_LIS2DH (308)
#define LIS2DH_INFO(...)  LOG_INFO(&_log, LOG_MODULE_LIS2DH, __VA_ARGS__)
#define LIS2DH_ERR(...)   LOG_ERROR(&_log, LOG_MODULE_LIS2DH, __VA_ARGS__)
static struct log _log;
#else
#define LIS2DH_INFO(...)
#define LIS2DH_ERR(...)
#endif

/* Exports for the sensor interface.
 */
static void *lis2dh_sensor_get_interface(struct sensor *, sensor_type_t);
static int lis2dh_sensor_read(struct sensor *, sensor_type_t,
                              sensor_data_func_t, void *, uint32_t);
static int lis2dh_sensor_get_config(struct sensor *, sensor_type_t,
                                    struct sensor_cfg *);

static const struct sensor_driver g_lis2dh_sensor_driver = {
    lis2dh_sensor_get_interface,
    lis2dh_sensor_read,
    lis2dh_sensor_get_config
};

int
lis2dh_default_cfg(struct lis2dh_cfg *cfg)
{
    cfg->accel_mode = LIS2DH_PWR_MODE_NORMAL;
    cfg->accel_range = LIS2DH_ACCEL_RANGE_2;
    cfg->accel_rate = LIS2DH_ACCEL_RATE_1;
    cfg->click_mode = LIS2DH_CLICK_OFF;
    cfg->click_threshold = 127; //if getting single clicks randomly, move up?, max 127
    cfg->click_time_limit = 127; //max 127
    cfg->click_time_window = 0x7f;
    cfg->click_time_latency = 128; //if getting double clicks randomly, move up?
    cfg->accel_wake_threshold = 0; //threshold to surpass to bring out of sleep, anythin other than 0 to enable, max 127
    cfg->accel_sleep_time_limit = 255; //time resting before goes back to sleep, max 255
    return 0;
}

/**
 * Expects to be called back through os_dev_create().
 *
 * @param The device object associated with this accellerometer
 * @param Argument passed to OS device init, unused
 *
 * @return 0 on success, non-zero error on failure.
 */
int
lis2dh_init(struct os_dev *dev, void *arg)
{
    struct lis2dh *lis;
    struct sensor *sensor;
    int rc;

    lis = (struct lis2dh *) dev;

    rc = lis2dh_default_cfg(&lis->cfg);
    if (rc) {
        goto error;
    }

    hal_gpio_init_out(LIS2DH_SS_PIN, 1);

    struct hal_spi_settings data_struct;
    data_struct.data_order = HAL_SPI_MSB_FIRST;
    data_struct.data_mode = HAL_SPI_MODE0;
    data_struct.baudrate = SPI_BAUDRATE;
    data_struct.word_size = HAL_SPI_WORD_SIZE_8BIT;

    rc = hal_spi_config(MYNEWT_VAL(LIS2DH_SPIBUS), &data_struct);
    if (rc) {
        return (rc);
    }

    hal_spi_set_txrx_cb(MYNEWT_VAL(LIS2DH_SPIBUS), NULL, NULL);
    hal_spi_enable(MYNEWT_VAL(LIS2DH_SPIBUS));

#if MYNEWT_VAL(LIS2DH_LOG)
    log_register("lis2dh", &_log, &log_console_handler, NULL, LOG_SYSLEVEL);
#endif

    sensor = &lis->sensor;

#if MYNEWT_VAL(LIS2DH_STATS)
    /* Initialise the stats entry */
    rc = stats_init(
             STATS_HDR(g_lis2dhstats),
             STATS_SIZE_INIT_PARMS(g_lis2dhstats, STATS_SIZE_32),
             STATS_NAME_INIT_PARMS(lis2dh_stat_section));
    SYSINIT_PANIC_ASSERT(rc == 0);
    /* Register the entry with the stats registry */
    rc = stats_register("lis2dh", STATS_HDR(g_lis2dhstats));
    SYSINIT_PANIC_ASSERT(rc == 0);
#endif

    rc = sensor_init(sensor, dev);
    if (rc != 0) {
        goto error;
    }

    /* Add the accelerometer driver */
    rc = sensor_set_driver(sensor, SENSOR_TYPE_ACCELEROMETER,
                           (struct sensor_driver *) &g_lis2dh_sensor_driver);
    if (rc != 0) {
        goto error;
    }

    rc = sensor_mgr_register(sensor);
    if (rc != 0) {
        goto error;
    }

    return (0);
error:
    return (rc);
}

/**
 * Get chip ID from the sensor
 *
 * @param Pointer to the variable to fill up chip ID in
 * @return 0 on success, non-zero on failure
 */
int
lis2dh_get_chip_id(uint8_t *id)
{
    int rc;
    uint8_t idtmp;

    /* Check if we can read the chip address */
    rc = lis2dh_read8(LIS2DH_REGISTER_WHO_AM_I, &idtmp);
    if (rc) {
        goto error;
    }

    *id = idtmp;

    return 0;
error:
    return rc;
}

int
lis2dh_config(struct lis2dh *lis, struct lis2dh_cfg *cfg)
{
    int rc;

    /* Overwrite the configuration data. */
    memcpy(&lis->cfg, cfg, sizeof(*cfg));

    rc = lis2dh_verify_id();
    if (rc != 0) {
        goto error;
    }

    rc = lis2dh_click_configure(&lis->cfg);
    if (rc != 0) {
        goto error;
    }

    rc = lis2dh_accel_configure(&lis->cfg);
    if (rc != 0) {
        goto error;
    }

    return 0;
error:
    return (rc);
}

/**
 * Get vector data from sensor
 *
 * @param pointer to the structure to be filled up
 * @param Type of sensor
 * @return 0 on success, non-zero on error
 */
int
lis2dh_get_vector_data(void *datastruct, struct lis2dh_cfg *cfg)
{
    uint8_t payload[6];
    int16_t x, y, z;
    struct sensor_accel_data *sad;
    float mg_lsb;
    struct lis2dh_cfg liscfg = *cfg;
    int rc;

    memset (payload, 0, 6);

    x = y = z = 0;

    rc = lis2dh_readlen(LIS2DH_REGISTER_OUT_X_L,
                       payload, 6);
    if (rc) {
        goto error;
    }

    uint8_t resolution_shift;

    switch(liscfg.accel_mode) {
        case LIS2DH_PWR_MODE_HIGHRESOLUTION:
            // 12-bit
            resolution_shift = 4;
            break;
        case LIS2DH_PWR_MODE_NORMAL:
            // 10-bit
            resolution_shift = 6;
            break;
        case LIS2DH_PWR_MODE_LOWPOWER:
            //8-bit
            resolution_shift = 8;
            break;
        default:
            rc = SYS_EINVAL;
            goto error;
    }

    /* Shift n-bit left-aligned accel values into 16-bit int */
    x = ((int16_t)(payload[0] | (payload[1] << 8))) >> resolution_shift;
    y = ((int16_t)(payload[2] | (payload[3] << 8))) >> resolution_shift;
    z = ((int16_t)(payload[4] | (payload[5] << 8))) >> resolution_shift;

    // LIS2DH_INFO("x:%u\ty:%u\tz:%u\n",
    //             x, y, z);

    sad = datastruct;

    switch(liscfg.accel_range) {
        case LIS2DH_ACCEL_RANGE_2:
#if MYNEWT_VAL(LIS2DH_STATS)
            STATS_INC(g_lis2dhstats, samples_acc_2g);
#endif
            mg_lsb = 0.005F;
            break;
        case LIS2DH_ACCEL_RANGE_4:
#if MYNEWT_VAL(LIS2DH_STATS)
            STATS_INC(g_lis2dhstats, samples_acc_4g);
#endif
            mg_lsb = 0.008F;
            break;
        case LIS2DH_ACCEL_RANGE_8:
#if MYNEWT_VAL(LIS2DH_STATS)
            STATS_INC(g_lis2dhstats, samples_acc_8g);
#endif
            mg_lsb = 0.016F;
            break;
        case LIS2DH_ACCEL_RANGE_16:
#if MYNEWT_VAL(LIS2DH_STATS)
            STATS_INC(g_lis2dhstats, samples_acc_16g);
#endif
            mg_lsb = 0.048F;
            break;
        default:
            LIS2DH_ERR("Unknown accel range: 0x%02X. Assuming +/-2G.\n",
                liscfg.accel_range);
            mg_lsb = 0.005F;
            break;
    }

    /* Convert from mg to Earth gravity in m/s^2 */
    sad->sad_x = (float)x * mg_lsb * 9.80665F;
    sad->sad_y = (float)y * mg_lsb * 9.80665F;
    sad->sad_z = (float)z * mg_lsb * 9.80665F;

    // char tmpstr[13];
    // LIS2DH_INFO("x: %s", sensor_ftostr(sad->sad_x, tmpstr, 13));
    // LIS2DH_INFO("y: %s", sensor_ftostr(sad->sad_y, tmpstr, 13));
    // LIS2DH_INFO("z: %s", sensor_ftostr(sad->sad_z, tmpstr, 13));

    sad->sad_x_is_valid = 1;
    sad->sad_y_is_valid = 1;
    sad->sad_z_is_valid = 1;

    return 0;
error:
    return rc;
}

/**
 * Get temperature from lis2dh sensor
 *
 * @param pointer to the temperature variable to be filled up
 * @return 0 on success, non-zero on error
 */
//TODO, turns out not suited for absolute temperature, would need to calibrate for this value
int
lis2dh_get_temp(int8_t *temp)
{
    int rc;
    uint8_t rx[2];

    /* Both OUT_TEMP_H and OUT_TEMP_L registers must be read. */
    rc = lis2dh_readlen(LIS2DH_REGISTER_OUT_TEMP_L, rx, 2);
    if (rc) {
        goto error;
    }

    /* Temperature data is stored inside OUT_TEMP_H as 2’s complement data in 8 bit format left
    justified. */
    *temp = (int8_t)rx[1] + 25; //todo, not actually calibrated

    return 0;
error:
    return rc;
}

int
lis2dh_sensor_read(struct sensor *sensor, sensor_type_t type,
                   sensor_data_func_t data_func, void *data_arg, uint32_t timeout)
{
    void *databuf;
    struct lis2dh *lis;
    int rc;
        
    lis = (struct lis2dh *) SENSOR_GET_DEVICE(sensor);

    /* Since this is the biggest struct, malloc space for it */
    databuf = malloc(sizeof(struct sensor_accel_data));
    assert(databuf != NULL);

    if (type == SENSOR_TYPE_ACCELEROMETER) {
        /* Get vector data accel values */
        rc = lis2dh_get_vector_data(databuf, &lis->cfg);
        if (rc) {
            goto err;
        }
    } else{
        rc = SYS_EINVAL;
        goto err;
    }

    /* Call data function */
    rc = data_func(sensor, data_arg, databuf);
    if (rc) {
        goto err;
    }

    /* Free the data buffer */
    free(databuf);

    return 0;
err:
    return rc;
}

static void *
lis2dh_sensor_get_interface(struct sensor *sensor, sensor_type_t type)
{
    return (NULL);
}

static int
lis2dh_sensor_get_config(struct sensor *sensor, sensor_type_t type,
                         struct sensor_cfg *cfg)
{
    int rc;

    if ((type != SENSOR_TYPE_ACCELEROMETER)) {
        rc = SYS_EINVAL;
        goto error;
    }

    cfg->sc_valtype = SENSOR_VALUE_TYPE_FLOAT_TRIPLET;

    return 0;
error:
    return rc;
}
