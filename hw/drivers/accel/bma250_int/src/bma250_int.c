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
#include "bma250_int/bma250_int.h"
#include "bsp/bsp.h"
#include "hal/hal_gpio.h"

#if MYNEWT_VAL(BMA250_INT_LOG)
#include "log/log.h"
#endif

#if MYNEWT_VAL(BMA250_INT_STATS)
#include "stats/stats.h"
#endif

static struct bm250_int_cfg bma250cfg;

#if MYNEWT_VAL(BMA250_INT_STATS)
/* Define the stats section and records */
STATS_SECT_START(bma250_int_stat_section)
STATS_SECT_ENTRY(acc_orientation)
STATS_SECT_ENTRY(acc_slope_motion)
STATS_SECT_ENTRY(acc_tap_single)
STATS_SECT_ENTRY(acc_tap_double)
STATS_SECT_ENTRY(errors)
STATS_SECT_END

/* Define stat names for querying */
STATS_NAME_START(bma250_int_stat_section)
STATS_NAME(bma250_int_stat_section, acc_orientation)
STATS_NAME(bma250_int_stat_section, acc_slope_motion)
STATS_NAME(bma250_int_stat_section, acc_tap_single)
STATS_NAME(bma250_int_stat_section, acc_tap_double)
STATS_NAME(bma250_int_stat_section, errors)
STATS_NAME_END(bma250_int_stat_section)

/* Global variable used to hold stats data */
STATS_SECT_DECL(bma250_int_stat_section) g_bma250intstats;
#endif

#if MYNEWT_VAL(BMA250_INT_LOG)
#define LOG_MODULE_BMA250_INT (309)
#define BMA250_INT_INFO(...)  LOG_INFO(&_log, LOG_MODULE_BMA250_INT, __VA_ARGS__)
#define BMA250_INT_ERR(...)   LOG_ERROR(&_log, LOG_MODULE_BMA250_INT, __VA_ARGS__)
static struct log _log;
#else
#define BMA250_INT_INFO(...)
#define BMA250_INT_ERR(...)
#endif

#define BMA250_INT_PIN1     BMA250_INT_3
#define BMA250_INT_PIN2     BMA250_INT_2
#define BMA250_INT_PIN5     BMA250_INT_1
#define BMA250_INT_PIN10    BMA250_INT_5
#define BMA250_INT_PIN12    BMA250_INT_4
#define BMA250_INT_PIN6     BMA250_INT_6

struct bm250_int_int_event int_event;
static struct os_event gpio_ev2 = {
    .ev_arg = &int_event,
};

static void
interrupt_ev_cb(struct os_event *ev)
{
    // uint8_t source;
    // int pin = ev->ev_arg;
    // int status = hal_gpio_read(pin);

    switch (bma250cfg.mode) {
    case BMA250_INT_MODE_OFF:

        break;
    case BMA250_INT_MODE_TAP:

        break;
    case BMA250_INT_MODE_ORIENTATION:

        break;
    case BMA250_INT_MODE_SLOPE:

        break;
    default:
        goto error;
    }

    os_eventq_put(os_eventq_dflt_get(), &gpio_ev2);

error:
    return;
}

static struct os_event gpio_ev = {
    .ev_cb = interrupt_ev_cb,
};

//get out of irq asap
static void
my_gpio_irq(void *arg)
{
    gpio_ev.ev_arg = arg;
    os_eventq_put(os_eventq_dflt_get(), &gpio_ev);
}

int
bma250_int_config(struct bm250_int_cfg *cfg)
{
    int rc;

    memcpy(&bma250cfg, cfg, sizeof(*cfg));
    gpio_ev2.ev_cb = bma250cfg.int_cb;

    switch (bma250cfg.mode) {
    case BMA250_INT_MODE_OFF:
        //todo check what mode were in and dont release/disable stuff that isnt enabled
        //todo release the gpio pins?
        hal_gpio_irq_release(BMA250_INT_PIN1);
        hal_gpio_irq_disable(BMA250_INT_PIN1);
        hal_gpio_irq_release(BMA250_INT_PIN2);
        hal_gpio_irq_disable(BMA250_INT_PIN2);
        hal_gpio_irq_release(BMA250_INT_PIN5);
        hal_gpio_irq_disable(BMA250_INT_PIN5);
        hal_gpio_irq_release(BMA250_INT_PIN6);
        hal_gpio_irq_disable(BMA250_INT_PIN6);
        break;
    case BMA250_INT_MODE_TAP:
        hal_gpio_init_out(BMA250_INT_PIN1, 0);
        hal_gpio_init_out(BMA250_INT_PIN5, 1);
        hal_gpio_init_out(BMA250_INT_PIN6, 0);
        hal_gpio_init_out(BMA250_INT_PIN12, 1);
        hal_gpio_irq_init(BMA250_INT_PIN2, my_gpio_irq, NULL, HAL_GPIO_TRIG_BOTH, HAL_GPIO_PULL_UP);
        hal_gpio_irq_enable(BMA250_INT_PIN2);
        break;
    case BMA250_INT_MODE_ORIENTATION:
        hal_gpio_init_out(BMA250_INT_PIN5, 0);
        hal_gpio_init_out(BMA250_INT_PIN12, 1);
        hal_gpio_irq_init(BMA250_INT_PIN1, my_gpio_irq, NULL, HAL_GPIO_TRIG_BOTH, HAL_GPIO_PULL_UP);
        hal_gpio_irq_enable(BMA250_INT_PIN1);
        hal_gpio_irq_init(BMA250_INT_PIN2, my_gpio_irq, NULL, HAL_GPIO_TRIG_BOTH, HAL_GPIO_PULL_UP);
        hal_gpio_irq_enable(BMA250_INT_PIN2);
        break;
    case BMA250_INT_MODE_SLOPE:
        hal_gpio_init_out(BMA250_INT_PIN12, 0);
        hal_gpio_irq_init(BMA250_INT_PIN2, my_gpio_irq, NULL, HAL_GPIO_TRIG_BOTH, HAL_GPIO_PULL_UP);
        hal_gpio_irq_enable(BMA250_INT_PIN2);
        hal_gpio_irq_init(BMA250_INT_PIN1, my_gpio_irq, NULL, HAL_GPIO_TRIG_BOTH, HAL_GPIO_PULL_UP);
        hal_gpio_irq_enable(BMA250_INT_PIN1);
        hal_gpio_irq_init(BMA250_INT_PIN5, my_gpio_irq, NULL, HAL_GPIO_TRIG_BOTH, HAL_GPIO_PULL_UP);
        hal_gpio_irq_enable(BMA250_INT_PIN5);
        hal_gpio_irq_init(BMA250_INT_PIN6, my_gpio_irq, NULL, HAL_GPIO_TRIG_BOTH, HAL_GPIO_PULL_UP);
        hal_gpio_irq_enable(BMA250_INT_PIN6);
        break;
    default:
        rc = SYS_EINVAL;
        goto error;
    }

    return (0);
error:
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
bma250_int_init(struct os_dev *dev, void *arg)
{
    int rc;

#if MYNEWT_VAL(BMA250_INT_LOG)
    log_register("bma250_int", &_log, &log_console_handler, NULL, LOG_SYSLEVEL);
#endif

#if MYNEWT_VAL(BMA250_INT_STATS)
    /* Initialise the stats entry */
    rc = stats_init(
             STATS_HDR(g_bma250intstats),
             STATS_SIZE_INIT_PARMS(g_bma250intstats, STATS_SIZE_32),
             STATS_NAME_INIT_PARMS(bma250_int_stat_section));
    SYSINIT_PANIC_ASSERT(rc == 0);
    /* Register the entry with the stats registry */
    rc = stats_register("bma250_int", STATS_HDR(g_bma250intstats));
    SYSINIT_PANIC_ASSERT(rc == 0);
#endif

    return (0);
}
