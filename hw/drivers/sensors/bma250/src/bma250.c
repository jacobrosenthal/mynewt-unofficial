/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * resarding copyright ownership.  The ASF licenses this file
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
#include "hal/hal_i2c.h"
#include "sensor/sensor.h"
#include "sensor/accel.h"
#include "bma250/bma250.h"
#include "bma250_priv.h"

#if MYNEWT_VAL(BMA250_LOG)
#include "log/log.h"
#endif

#if MYNEWT_VAL(BMA250_STATS)
#include "stats/stats.h"
#endif

#if MYNEWT_VAL(BMA250_STATS)
/* Define the stats section and records */
STATS_SECT_START(bma250_stat_section)
    STATS_SECT_ENTRY(samples_acc_2g)
    STATS_SECT_ENTRY(samples_acc_4g)
    STATS_SECT_ENTRY(samples_acc_8g)
    STATS_SECT_ENTRY(samples_acc_16g)
    STATS_SECT_ENTRY(errors)
STATS_SECT_END

/* Define stat names for querying */
STATS_NAME_START(bma250_stat_section)
    STATS_NAME(bma250_stat_section, samples_acc_2g)
    STATS_NAME(bma250_stat_section, samples_acc_4g)
    STATS_NAME(bma250_stat_section, samples_acc_8g)
    STATS_NAME(bma250_stat_section, samples_acc_16g)
    STATS_NAME(bma250_stat_section, errors)
STATS_NAME_END(bma250_stat_section)

/* Global variable used to hold stats data */
STATS_SECT_DECL(bma250_stat_section) g_bma250stats;
#endif

#if MYNEWT_VAL(BMA250_LOG)
#define LOG_MODULE_BMA250 (303)
#define BMA250_INFO(...)  LOG_INFO(&_log, LOG_MODULE_BMA250, __VA_ARGS__)
#define BMA250_ERR(...)   LOG_ERROR(&_log, LOG_MODULE_BMA250, __VA_ARGS__)
static struct log _log;
#else
#define BMA250_INFO(...)
#define BMA250_ERR(...)
#endif

/* Exports for the sensor interface.
 */
static void *bma250_sensor_get_interface(struct sensor *, sensor_type_t);
static int bma250_sensor_read(struct sensor *, sensor_type_t,
        sensor_data_func_t, void *, uint32_t);
static int bma250_sensor_get_config(struct sensor *, sensor_type_t,
        struct sensor_cfg *);

static const struct sensor_driver g_bma250_sensor_driver = {
    bma250_sensor_get_interface,
    bma250_sensor_read,
    bma250_sensor_get_config
};

/**
 * Writes a single byte to the specified register
 *
 * @param The I2C address to use
 * @param The register address to write to
 * @param The value to write
 *
 * @return 0 on success, non-zero error on failure.
 */
int
bma250_write8(uint8_t addr, uint8_t reg, uint32_t value)
{
    int rc;
    uint8_t payload[2] = { reg, value & 0xFF };

    struct hal_i2c_master_data data_struct = {
        .address = addr,
        .len = 2,
        .buffer = payload
    };

    rc = hal_i2c_master_write(MYNEWT_VAL(BMA250_I2CBUS), &data_struct,
                              OS_TICKS_PER_SEC / 10, 1);
    if (rc) {
        BMA250_ERR("Failed to write to 0x%02X:0x%02X with value 0x%02X\n",
                       addr, reg, value);
#if MYNEWT_VAL(BMA250_STATS)
        STATS_INC(g_bma250stats, errors);
#endif
    }

    return rc;
}

/**
 * Reads a single byte from the specified register
 *
 * @param The I2C address to use
 * @param The register address to read from
 * @param Pointer to where the register value should be written
 *
 * @return 0 on success, non-zero error on failure.
 */
int
bma250_read8(uint8_t addr, uint8_t reg, uint8_t *value)
{
    int rc;
    uint8_t payload;

    struct hal_i2c_master_data data_struct = {
        .address = addr,
        .len = 1,
        .buffer = &payload
    };

    /* Register write */
    payload = reg;
    rc = hal_i2c_master_write(MYNEWT_VAL(BMA250_I2CBUS), &data_struct,
                              OS_TICKS_PER_SEC / 10, 1);
    if (rc) {
        BMA250_ERR("I2C access failed at address 0x%02X\n", addr);
#if MYNEWT_VAL(BMA250_STATS)
        STATS_INC(g_bma250stats, errors);
#endif
        goto error;
    }

    /* Read one byte back */
    payload = 0;
    rc = hal_i2c_master_read(MYNEWT_VAL(BMA250_I2CBUS), &data_struct,
                             OS_TICKS_PER_SEC / 10, 1);
    *value = payload;
    if (rc) {
        BMA250_ERR("Failed to read from 0x%02X:0x%02X\n", addr, reg);
#if MYNEWT_VAL(BMA250_STATS)
        STATS_INC(g_bma250stats, errors);
#endif
    }

error:
    return rc;
}

int
bma250_read48(uint8_t addr, uint8_t reg, uint8_t *buffer)
{
    int rc;
    uint8_t payload[7] = { reg, 0, 0, 0, 0, 0, 0 };

    struct hal_i2c_master_data data_struct = {
        .address = addr,
        .len = 1,
        .buffer = payload
    };

    /* Clear the supplied buffer */
    memset(buffer, 0, 6);

    /* Register write */
    rc = hal_i2c_master_write(MYNEWT_VAL(BMA250_I2CBUS), &data_struct,
                              OS_TICKS_PER_SEC / 10, 1);
    if (rc) {
        BMA250_ERR("I2C access failed at address 0x%02X\n", addr);
#if MYNEWT_VAL(BMA250_STATS)
        STATS_INC(g_bma250stats, errors);
#endif
        goto error;
    }

    /* Read six bytes back */
    memset(payload, 0, sizeof(payload));
    data_struct.len = 6;
    rc = hal_i2c_master_read(MYNEWT_VAL(BMA250_I2CBUS), &data_struct,
                             OS_TICKS_PER_SEC / 10, 1);

    if (rc) {
        BMA250_ERR("Failed to read from 0x%02X:0x%02X\n", addr, reg);
#if MYNEWT_VAL(BMA250_STATS)
        STATS_INC(g_bma250stats, errors);
#endif
        goto error;
    }

    /* Copy the I2C results into the supplied buffer */
    memcpy(buffer, payload, 6);

    /* ToDo: Log raw reads */
    // console_printf("0x%04X\n", (uint16_t)payload[0] | ((uint16_t)payload[1] << 8));

error:
    return rc;
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
bma250_init(struct os_dev *dev, void *arg)
{
    struct bma250 *lsm;
    struct sensor *sensor;
    int rc;

    lsm = (struct bma250 *) dev;

#if MYNEWT_VAL(BMA250_LOG)
    log_register("bma250", &_log, &log_console_handler, NULL, LOG_SYSLEVEL);
#endif

    sensor = &lsm->sensor;

#if MYNEWT_VAL(BMA250_STATS)
    /* Initialise the stats entry */
    rc = stats_init(
        STATS_HDR(g_bma250stats),
        STATS_SIZE_INIT_PARMS(g_bma250stats, STATS_SIZE_32),
        STATS_NAME_INIT_PARMS(bma250_stat_section));
    SYSINIT_PANIC_ASSERT(rc == 0);
    /* Register the entry with the stats registry */
    rc = stats_register("bma250", STATS_HDR(g_bma250stats));
    SYSINIT_PANIC_ASSERT(rc == 0);
#endif

    rc = sensor_init(sensor, dev);
    if (rc != 0) {
        goto err;
    }

    /* Add the accelerometer driver */
    rc = sensor_set_driver(sensor, SENSOR_TYPE_ACCELEROMETER,
            (struct sensor_driver *) &g_bma250_sensor_driver);
    if (rc != 0) {
        goto err;
    }

    rc = sensor_mgr_register(sensor);
    if (rc != 0) {
        goto err;
    }

    return (0);
err:
    return (rc);
}

int
bma250_config(struct bma250 *lsm, struct bma250_cfg *cfg)
{
    int rc;

    /* Overwrite the configuration data. */
    memcpy(&lsm->cfg, cfg, sizeof(*cfg));

    /* Set accel data rate and enable XYZ output */
    rc = bma250_write8(BMA250_ADDR_ACCEL,
        BMA250_REGISTER_PMU_BW,
        lsm->cfg.accel_rate | 0x1F);
    if (rc != 0) {
        goto err;
    }

    /* Set accel scale */
    rc = bma250_write8(BMA250_ADDR_ACCEL,
        BMA250_REGISTER_PMU_RANGE,
        lsm->cfg.accel_range | 0x0F);
    if (rc != 0) {
        goto err;
    }

err:
    return (rc);
}

static void *
bma250_sensor_get_interface(struct sensor *sensor, sensor_type_t type)
{
    return (NULL);
}

static int
bma250_sensor_read(struct sensor *sensor, sensor_type_t type,
        sensor_data_func_t data_func, void *data_arg, uint32_t timeout)
{
    struct bma250 *lsm;
    struct sensor_accel_data sad;
    int rc;
    int16_t x, y, z;
    float mg_lsb;
    uint8_t payload[6];

    /* If the read isn't looking for accel data, don't do anything. */
    if (!(type & SENSOR_TYPE_ACCELEROMETER)) {
        rc = SYS_EINVAL;
        goto err;
    }

    lsm = (struct bma250 *) SENSOR_GET_DEVICE(sensor);

    /* Get a new accelerometer sample */
    if (type & SENSOR_TYPE_ACCELEROMETER) {
        x = y = z = 0;
        rc = bma250_read48(BMA250_ADDR_ACCEL,
                            BMA250_REGISTER_ACCD_X_LSB,
                            payload);
        if (rc != 0) {
            goto err;
        }

        /* Shift 10-bit left-aligned accel values into 16-bit int */
        x = ((int16_t)(payload[0] | (payload[1] << 8))) >> 6;
        y = ((int16_t)(payload[2] | (payload[3] << 8))) >> 6;
        z = ((int16_t)(payload[4] | (payload[5] << 8))) >> 6;

        /* Determine mg per lsb based on range */
        switch(lsm->cfg.accel_range) {
            case BMA250_ACCEL_RANGE_2:
#if MYNEWT_VAL(BMA250_STATS)
                STATS_INC(g_bma250stats, samples_acc_2g);
#endif
                mg_lsb = 0.001F;
                break;
            case BMA250_ACCEL_RANGE_4:
#if MYNEWT_VAL(BMA250_STATS)
                STATS_INC(g_bma250stats, samples_acc_4g);
#endif
                mg_lsb = 0.002F;
                break;
            case BMA250_ACCEL_RANGE_8:
#if MYNEWT_VAL(BMA250_STATS)
                STATS_INC(g_bma250stats, samples_acc_8g);
#endif
                mg_lsb = 0.004F;
                break;
            case BMA250_ACCEL_RANGE_16:
#if MYNEWT_VAL(BMA250_STATS)
                STATS_INC(g_bma250stats, samples_acc_16g);
#endif
                mg_lsb = 0.012F;
                break;
            default:
                BMA250_ERR("Unknown accel range: 0x%02X. Assuming +/-2G.\n",
                    lsm->cfg.accel_range);
                mg_lsb = 0.001F;
                break;
        }

        /* Convert from mg to Earth gravity in m/s^2 */
        sad.sad_x = (float)x * mg_lsb * 9.80665F;
        sad.sad_y = (float)y * mg_lsb * 9.80665F;
        sad.sad_z = (float)z * mg_lsb * 9.80665F;

        sad.sad_x_is_valid = 1;
        sad.sad_y_is_valid = 1;
        sad.sad_z_is_valid = 1;

        /* Call data function */
        rc = data_func(sensor, data_arg, &sad);
        if (rc != 0) {
            goto err;
        }
    }

    return (0);
err:
    return (rc);
}

static int
bma250_sensor_get_config(struct sensor *sensor, sensor_type_t type,
        struct sensor_cfg *cfg)
{
    int rc;

    if ((type != SENSOR_TYPE_ACCELEROMETER)) {
        rc = SYS_EINVAL;
        goto err;
    }

    cfg->sc_valtype = SENSOR_VALUE_TYPE_FLOAT_TRIPLET;

    return (0);
err:
    return (rc);
}
