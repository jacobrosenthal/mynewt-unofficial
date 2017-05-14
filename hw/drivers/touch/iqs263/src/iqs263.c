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
#include "hal/hal_gpio.h"
#include "iqs263/iqs263.h"
#include "iqs263_priv.h"
#include "bsp/bsp.h"

#if MYNEWT_VAL(IQS263_LOG)
#include "log/log.h"
#endif

#if MYNEWT_VAL(IQS263_STATS)
#include "stats/stats.h"
#endif

#if MYNEWT_VAL(IQS263_STATS)
/* Define the stats section and records */
STATS_SECT_START(iqs263_stat_section)
    STATS_SECT_ENTRY(proximity)
    STATS_SECT_ENTRY(touch)
    STATS_SECT_ENTRY(slide)
    STATS_SECT_ENTRY(ati)
    STATS_SECT_ENTRY(movement)
    STATS_SECT_ENTRY(tap)
    STATS_SECT_ENTRY(flickleft)
    STATS_SECT_ENTRY(flickright)
    STATS_SECT_ENTRY(irqs)
    STATS_SECT_ENTRY(errors)
STATS_SECT_END

/* Define stat names for querying */
STATS_NAME_START(iqs263_stat_section)
    STATS_NAME(iqs263_stat_section, proximity)
    STATS_NAME(iqs263_stat_section, touch)
    STATS_NAME(iqs263_stat_section, slide)
    STATS_NAME(iqs263_stat_section, ati)
    STATS_NAME(iqs263_stat_section, movement)
    STATS_NAME(iqs263_stat_section, tap)
    STATS_NAME(iqs263_stat_section, flickleft)
    STATS_NAME(iqs263_stat_section, flickright)
    STATS_NAME(iqs263_stat_section, irqs)
    STATS_NAME(iqs263_stat_section, errors)
STATS_NAME_END(iqs263_stat_section)

/* Global variable used to hold stats data */
STATS_SECT_DECL(iqs263_stat_section) g_iqs263stats;
#endif

#if MYNEWT_VAL(IQS263_LOG)
#define LOG_MODULE_IQS263 (311)
#define IQS263_INFO(...)  LOG_INFO(&_log, LOG_MODULE_IQS263, __VA_ARGS__)
#define IQS263_ERR(...)   LOG_ERROR(&_log, LOG_MODULE_IQS263, __VA_ARGS__)
static struct log _log;
#else
#define IQS263_INFO(...)
#define IQS263_ERR(...)
#endif

static void
interrupt_ev_cb(struct os_event *ev)
{
    int rc;
    uint8_t data_buffer[6];

#if MYNEWT_VAL(IQS263_STATS)
    STATS_INC(g_iqs263stats, irqs);
#endif

    rc = iqs263_event(data_buffer);
    if(rc){
        goto error;
    }

    uint8_t events = data_buffer[1];
    if(events & IQS263_EVENT_MASK_PROX)
    {
#if MYNEWT_VAL(IQS263_STATS)
        STATS_INC(g_iqs263stats, proximity);
#endif
        uint8_t prox = data_buffer[2];

        if (prox & IQS263_ACTIVE_CHANNELS_CH0)
        {

        }else
        {

        }
    }

    if(events & IQS263_EVENT_MASK_TOUCH)
    {
#if MYNEWT_VAL(IQS263_STATS)
        STATS_INC(g_iqs263stats, touch);
#endif
        uint8_t touch0 = data_buffer[2];

        if (touch0 != 0)
        {
            /* CHANNEL 1*/
            if (touch0 & IQS263_ACTIVE_CHANNELS_CH1)
            {

            }else
            {

            }

            /* CHANNEL 2 */
            if (touch0 & IQS263_ACTIVE_CHANNELS_CH2)
            {

            }else
            {

            }

            /* CHANNEL 3 */
            if (touch0 & IQS263_ACTIVE_CHANNELS_CH3)
            {

            }else
            {

            }
        }
    }

    if(events & IQS263_EVENT_MASK_SLIDE)
    {
#if MYNEWT_VAL(IQS263_STATS)
        STATS_INC(g_iqs263stats, slide);
#endif
        uint8_t sliderCoords = data_buffer[3];
        uint8_t touch0 = data_buffer[2];

        if (touch0 != 0)
        {
            if ((sliderCoords > 0 && sliderCoords < 85) && (touch0 & IQS263_ACTIVE_CHANNELS_CH1))
            {

            }else if ((sliderCoords > 0 && sliderCoords < 170) && (touch0 & IQS263_ACTIVE_CHANNELS_CH2))
            {

            }else if ((sliderCoords > 170 && sliderCoords < 255) && (touch0 & IQS263_ACTIVE_CHANNELS_CH3))
            {

            }else
            {
            }
        }
    }

    if(events & IQS263_EVENT_MASK_ATI)
    {
#if MYNEWT_VAL(IQS263_STATS)
        STATS_INC(g_iqs263stats, ati);
#endif
    }

    if(events & IQS263_EVENT_MASK_MOVEMENT)
    {
#if MYNEWT_VAL(IQS263_STATS)
        STATS_INC(g_iqs263stats, movement);
#endif
    }

    if(events & IQS263_EVENT_MASK_TAP)
    {
#if MYNEWT_VAL(IQS263_STATS)
        STATS_INC(g_iqs263stats, tap);
#endif
    }

    if(events & IQS263_EVENT_MASK_FLICK_RIGHT)
    {
#if MYNEWT_VAL(IQS263_STATS)
        STATS_INC(g_iqs263stats, flickright);
#endif
    }

    if(events & IQS263_EVENT_MASK_FLICK_LEFT)
    {
#if MYNEWT_VAL(IQS263_STATS)
        STATS_INC(g_iqs263stats, flickleft);
#endif
    }

error:
    return;
}

static struct os_event gpio_ev = {
    .ev_cb = interrupt_ev_cb,
};

static void
my_gpio_irq(void *arg)
{
    os_eventq_put(os_eventq_dflt_get(), &gpio_ev);
}

int
iqs263_write8(uint8_t reg, uint8_t value)
{
    int rc;
    uint8_t payload[2] = { reg, value};

    struct hal_i2c_master_data data_struct = {
        .address = IQS263_ADDR,
        .len = 2,
        .buffer = payload
    };

    rc = hal_i2c_master_write(MYNEWT_VAL(IQS263_I2CBUS), &data_struct,
                              OS_TICKS_PER_SEC, 1);
    if (rc) {
        IQS263_ERR("Failed to write to 0x%02X:0x%02X with value 0x%02X\n",
                       data_struct.address, reg, value);
#if MYNEWT_VAL(IQS263_STATS)
        STATS_INC(g_iqs263stats, errors);
#endif
    }

    return rc;
}

/**
 * Writes a multiple bytes to the specified register (MAX: 8 bytes)
 *
 * @param The register address to write to
 * @param The data buffer to write from
 *
 * @return 0 on success, non-zero error on failure.
 */
int
iqs263_writelen(uint8_t reg, uint8_t *buffer, uint8_t len)
{
    int rc;
    uint8_t payload[9] = { reg, 0, 0, 0, 0, 0, 0, 0, 0};

    struct hal_i2c_master_data data_struct = {
        .address = IQS263_ADDR,
        .len = 1,
        .buffer = payload
    };

    memcpy(&payload[1], buffer, len);

    /* Register write */
    rc = hal_i2c_master_write(MYNEWT_VAL(IQS263_I2CBUS), &data_struct,
                              OS_TICKS_PER_SEC / 10, 0);
    if (rc) {
        IQS263_ERR("I2C access failed at address 0x%02X:0x%02X\n", data_struct.address, reg);
#if MYNEWT_VAL(IQS263_STATS)
        STATS_INC(g_iqs263stats, errors);
#endif
        goto error;
    }

    memset(payload, 0, sizeof(payload));
    data_struct.len = len;
    rc = hal_i2c_master_write(MYNEWT_VAL(IQS263_I2CBUS), &data_struct,
                              OS_TICKS_PER_SEC / 10, 1);

    if (rc) {
        IQS263_ERR("Failed to read from 0x%02X:0x%02X\n", data_struct.address, reg);
#if MYNEWT_VAL(IQS263_STATS)
        STATS_INC(g_iqs263stats, errors);
#endif
        goto error;
    }

    return 0;
error:
    return rc;
}

/**
 * Reads a single byte from the specified register
 *
 * @param The register address to read from
 * @param Pointer to where the register value should be written
 *
 * @return 0 on success, non-zero error on failure.
 */
int
iqs263_read8(uint8_t reg, uint8_t *value)
{
    int rc;
    uint8_t payload;

    struct hal_i2c_master_data data_struct = {
        .address = IQS263_ADDR,
        .len = 1,
        .buffer = &payload
    };

    /* Register write */
    payload = reg;
    rc = hal_i2c_master_write(MYNEWT_VAL(IQS263_I2CBUS), &data_struct,
                              OS_TICKS_PER_SEC / 10, 0);
    if (rc) {
        IQS263_ERR("I2C register write failed at address 0x%02X:0x%02X\n",
                   data_struct.address, reg);
#if MYNEWT_VAL(IQS263_STATS)
        STATS_INC(g_iqs263stats, errors);
#endif
        goto error;
    }

    /* Read one byte back */
    payload = 0;
    rc = hal_i2c_master_read(MYNEWT_VAL(IQS263_I2CBUS), &data_struct,
                             OS_TICKS_PER_SEC / 10, 1);
    *value = payload;
    if (rc) {
        IQS263_ERR("Failed to read from 0x%02X:0x%02X\n", data_struct.address, reg);
#if MYNEWT_VAL(IQS263_STATS)
        STATS_INC(g_iqs263stats, errors);
#endif
    }

error:
    return rc;
}

/**
 * Read data from the sensor of variable length (MAX: 8 bytes)
 *
 *
 * @param Register to read from
 * @param Bufer to read into
 * @param Length of the buffer
 *
 * @return 0 on success and non-zero on failure
 */
int
iqs263_readlen(uint8_t reg, uint8_t *buffer, uint8_t len)
{
    int rc;
    uint8_t payload[9] = { reg, 0, 0, 0, 0, 0, 0, 0, 0};

    struct hal_i2c_master_data data_struct = {
        .address = IQS263_ADDR,
        .len = 1,
        .buffer = payload
    };

    /* Clear the supplied buffer */
    memset(buffer, 0, len);

    /* Register write */
    rc = hal_i2c_master_write(MYNEWT_VAL(IQS263_I2CBUS), &data_struct,
                              OS_TICKS_PER_SEC / 10, 0);
    if (rc) {
        IQS263_ERR("I2C access failed at address 0x%02X:0x%02X\n", data_struct.address, reg);
#if MYNEWT_VAL(IQS263_STATS)
        STATS_INC(g_iqs263stats, errors);
#endif
        goto error;
    }

    /* Read len bytes back */
    memset(payload, 0, sizeof(payload));
    data_struct.len = len;
    rc = hal_i2c_master_read(MYNEWT_VAL(IQS263_I2CBUS), &data_struct,
                             OS_TICKS_PER_SEC / 10, 1);

    if (rc) {
        IQS263_ERR("Failed to read from 0x%02X:0x%02X\n", data_struct.address, reg);
#if MYNEWT_VAL(IQS263_STATS)
        STATS_INC(g_iqs263stats, errors);
#endif
        goto error;
    }

    /* Copy the I2C results into the supplied buffer */
    memcpy(buffer, payload, len);

    return 0;
error:
    return rc;
}

int
iqs263_handshake(uint8_t retries)
{
    int rc;
    uint8_t data_buffer[2];

    //force ready low so we can talk to chip
    hal_gpio_init_out(IQS263_RDY, 0);

    //poll sadly
    do
    {
        rc = iqs263_readlen(IQS263_REGISTER_DEVICE_INFO, data_buffer, 2);
    }while(rc && retries--);

    if(rc){
        goto error;
    }

    if (data_buffer[0]!=IQS263_PROD_NUM || data_buffer[1]!=IQS263_VERSION_NUM) {
        goto error;
    }

    return (0);
error:
    return (rc);
}


int
iqs263_device_init()
{
    int rc;
    uint8_t data_buffer[8];

    rc = iqs263_handshake(100);
    if (rc) {
        goto error;
    }
    os_time_delay(5 * (OS_TICKS_PER_SEC / 100));

    /* Switch the IQS263 into projection mode - if necessary */
    // rc = iqs263_write8(SYS_FLAGS,
    //     0x00);
    // if (rc != 0) {
    //     goto error;
    // }
    // os_time_delay(2 * (OS_TICKS_PER_SEC / 100));

    /* Set active channels */
    rc = iqs263_write8(IQS263_REGISTER_ACTIVE_CHANNELS,
        IQS263_ACTIVE_CHANNELS_CH0 |
        IQS263_ACTIVE_CHANNELS_CH1 |
        IQS263_ACTIVE_CHANNELS_CH2 |
        IQS263_ACTIVE_CHANNELS_CH3);
    if (rc) {
        goto error;
    }
    os_time_delay(5 * (OS_TICKS_PER_SEC / 100));

    /* Setup touch and prox thresholds for each channel */
    data_buffer[0] = 0x08; //PROX_THRESHOLD
    data_buffer[1] = 0x20; //TOUCH_THRESHOLD_CH1
    data_buffer[2] = 0x20; //TOUCH_THRESHOLD_CH2
    data_buffer[3] = 0x20; //TOUCH_THRESHOLD_CH3
    data_buffer[4] = 0x03; //MOVEMENT_THRESHOLD
    data_buffer[5] = 0x00; //RESEED_BLOCK
    data_buffer[6] = 0x14; //HALT_TIME
    data_buffer[7] = 0x04; //I2C_TIMEOUT
    rc = iqs263_writelen(IQS263_REGISTER_THRESHOLDS,
        data_buffer, 8);
    if (rc) {
        goto error;
    }
    os_time_delay(5 * (OS_TICKS_PER_SEC / 100));

    /* Set the ATI Targets (Target Counts) */
    // data_buffer[0] = 0x00; //Low power timer value x 16ms
    data_buffer[0] = 0x30; //ATI target for touch value x 8
    data_buffer[1] = 0x40; //ATI target for proximity value x 8
    rc = iqs263_writelen(IQS263_REGISTER_TIMINGS_AND_TARGETS,
        data_buffer, 2);
    if (rc) {
        goto error;
    }
    os_time_delay(5 * (OS_TICKS_PER_SEC / 100));

    /* Set the BASE value for each channel */
    data_buffer[0] = 0x08; //CH0 Multipliers
    data_buffer[1] = 0x08; //CH1 Multipliers
    data_buffer[2] = 0x08; //CH2 Multipliers
    data_buffer[3] = 0x08; //CH3 Multipliers
    rc = iqs263_writelen(IQS263_REGISTER_MULTIPLIERS,
        data_buffer, 4);
    if (rc) {
        goto error;
    }
    os_time_delay(5 * (OS_TICKS_PER_SEC / 100));

    /* Setup prox settings */
    data_buffer[0] = IQS263_PROX_SETTINGS_0_ATI_OFF; //PROX_SETTINGS_0
    data_buffer[1] = 0x00; //PROX_SETTINGS_1
    data_buffer[2] = 0x00; //PROX_SETTINGS_2
    data_buffer[3] = 0x00; //PROX_SETTINGS_3
    data_buffer[4] = 0x00; //EVENT MASK
    rc = iqs263_writelen(IQS263_REGISTER_PROX_SETTINGS,
        data_buffer, 5);
    if (rc) {
        goto error;
    }
    os_time_delay(5 * (OS_TICKS_PER_SEC / 100));

    /* Setup Compensation (PCC) */
    data_buffer[0] = 0x51; //COMPENSATION_CH0
    data_buffer[1] = 0x49; //COMPENSATION_CH1
    data_buffer[2] = 0x4A; //COMPENSATION_CH2
    data_buffer[3] = 0x49; //COMPENSATION_CH3
    rc = iqs263_writelen(IQS263_REGISTER_COMPENSATION,
        data_buffer, 4);
    if (rc) {
        goto error;
    }
    os_time_delay(5 * (OS_TICKS_PER_SEC / 100));

    /* Set timings on the IQS263 */
    data_buffer[0] = 0x05; //TAP TIMER LIMIT
    data_buffer[1] = 0x51; //FLICK TIMER LIMIT
    data_buffer[2] = 0x33; //FLICK THRESHOLD VALUE
    rc = iqs263_writelen(IQS263_REGISTER_GESTURE_TIMERS,
        data_buffer, 3);
    if (rc) {
        goto error;
    }
    os_time_delay(5 * (OS_TICKS_PER_SEC / 100));

    /* Redo ati */
    rc = iqs263_write8(IQS263_REGISTER_PROX_SETTINGS,
        IQS263_PROX_SETTINGS_0_REDO_ATI);
    if (rc != 0) {
        goto error;
    }
    os_time_delay(5 * (OS_TICKS_PER_SEC / 100));

    // Wait until the ATI algorithm is done
    do
    {
        rc = iqs263_readlen(IQS263_REGISTER_SYS_FLAGS, data_buffer, 1);
        if (rc) {
            goto error;
        }
        os_time_delay(5 * (OS_TICKS_PER_SEC / 100));
    }
    while (data_buffer[0] & IQS263_SYS_FLAGS_ATI_BUSY);

    /* read the error bit to determine if ATI error occured */
    if (data_buffer[0] & IQS263_SYS_FLAGS_ATI_ERROR) {
        goto error;
    }

    /* Setup prox settings */
    data_buffer[0] = 0x00; //PROX_SETTINGS_0
    uint8_t dogs = (IQS263_PROX_SETTINGS_1_EVENT_MODE |
                        IQS263_PROX_SETTINGS_1_CF_BETA_1 |
                        IQS263_PROX_SETTINGS_1_SLIDER_3CH |
                        IQS263_PROX_SETTINGS_1_LTA_BETA_2_8); //PROX_SETTINGS_1
    assert(dogs == 0x5d);
    data_buffer[1] = dogs;
    data_buffer[2] = IQS263_PROX_SETTINGS_2_MOVEMENT; //PROX_SETTINGS_2
    data_buffer[3] = 0x00; //PROX_SETTINGS_3
    data_buffer[4] = IQS263_EVENT_MASK_PROX |
                        // IQS263_EVENT_MASK_TOUCH | 
                        // IQS263_EVENT_MASK_SLIDE |
                        // IQS263_EVENT_MASK_ATI |
                        // IQS263_EVENT_MASK_MOVEMENT |
                        // IQS263_EVENT_MASK_TAP |
                        // IQS263_EVENT_MASK_FLICK_RIGHT |
                        IQS263_EVENT_MASK_FLICK_LEFT; //EVENT MASK

    rc = iqs263_writelen(IQS263_REGISTER_PROX_SETTINGS,
        data_buffer, 5);
    if (rc) {
        goto error;
    }

    hal_gpio_irq_init(IQS263_RDY, my_gpio_irq, NULL, HAL_GPIO_TRIG_FALLING, HAL_GPIO_PULL_UP);
    hal_gpio_irq_enable(IQS263_RDY);
    return (0);
error:
    hal_gpio_init_in(IQS263_RDY, HAL_GPIO_PULL_NONE);
    return (rc);
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
iqs263_init(struct os_dev *dev, void *arg)
{
    int rc;

#if MYNEWT_VAL(IQS263_LOG)
    log_register("iqs263", &_log, &log_console_handler, NULL, LOG_SYSLEVEL);
#endif

#if MYNEWT_VAL(IQS263_STATS)
    /* Initialise the stats entry */
    rc = stats_init(
        STATS_HDR(g_iqs263stats),
        STATS_SIZE_INIT_PARMS(g_iqs263stats, STATS_SIZE_32),
        STATS_NAME_INIT_PARMS(iqs263_stat_section));
    SYSINIT_PANIC_ASSERT(rc == 0);
    /* Register the entry with the stats registry */
    rc = stats_register("iqs263", STATS_HDR(g_iqs263stats));
    SYSINIT_PANIC_ASSERT(rc == 0);
#endif

    rc = iqs263_device_init();
    if (rc) {
        goto error;
    }

    return (0);
error:
    return (rc);
}


// int
// bma250_config(struct bma250 *lsm, struct bma250_cfg *cfg)
// {
//     int rc;

//     /* Overwrite the configuration data. */
//     memcpy(&lsm->cfg, cfg, sizeof(*cfg));

//     /* Set accel data rate and enable XYZ output */
//     rc = bma250_write8(IQS263_ADDR,
//         BMA250_REGISTER_PMU_BW,
//         lsm->cfg.accel_rate | 0x1F);
//     if (rc != 0) {
//         goto error;
//     }

//     /* Set accel scale */
//     rc = bma250_write8(IQS263_ADDR,
//         BMA250_REGISTER_PMU_RANGE,
//         lsm->cfg.accel_range | 0x0F);
//     if (rc != 0) {
//         goto error;
//     }

// error:
//     return (rc);
// }

int
iqs263_event(uint8_t *value)
{
    uint8_t data_buffer[6];
    int rc;

    /* Read the system flags register to enable all events */
    rc = iqs263_readlen(IQS263_REGISTER_SYS_FLAGS, &data_buffer[0], 2);
    if (rc) {
        goto error;
    }
    os_time_delay(5 * (OS_TICKS_PER_SEC / 100));

    /* Read from the touch bytes register to enable touch events */
    rc = iqs263_readlen(IQS263_REGISTER_TOUCH_BYTES, &data_buffer[2], 1);
    if (rc) {
        goto error;
    }
    os_time_delay(5 * (OS_TICKS_PER_SEC / 100));

    /* Read the coordinates register to get slider coordinates */
    rc = iqs263_readlen(IQS263_REGISTER_COORDINATES, &data_buffer[3], 3);
    if (rc) {
        goto error;
    }

    memcpy(data_buffer, value, 6);
    return (0);
error:
    return (rc);
}