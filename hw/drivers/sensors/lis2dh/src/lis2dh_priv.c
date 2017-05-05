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

#if MYNEWT_VAL(LIS2DH_STATS)
#include "stats/stats.h"
#endif


struct lis2dh_click_event click_event;
static struct os_event gpio_ev2 = {
    .ev_arg = &click_event,
};


static void
interrupt_ev_cb(struct os_event *ev)
{
    uint8_t rc;
    uint8_t source;

    rc = lis2dh_read8(LIS2DH_REGISTER_CLICK_SRC, &source);
    if (rc) {
        goto done;
    }

    bool negative = source && LIS2DH_REGISTER_CLICK_SRC_Sign;
    bool s_click = source && LIS2DH_REGISTER_CLICK_SRC_SClick;
    bool d_click = source && LIS2DH_REGISTER_CLICK_SRC_DClick;

    bool x = (LIS2DH_CLICK_X || LIS2DH_CLICK_ALL) && (source && LIS2DH_REGISTER_CLICK_SRC_X);
    bool y = (LIS2DH_CLICK_Y || LIS2DH_CLICK_ALL) && (source && LIS2DH_REGISTER_CLICK_SRC_Y);
    bool z = (LIS2DH_CLICK_Z || LIS2DH_CLICK_ALL) && (source && LIS2DH_REGISTER_CLICK_SRC_Z);

    if(s_click && negative)
    {
        click_event.mode = LIS2DH_CLICK_SINGLE_NEGATIVE;
    }else if(s_click && !negative)
    {
        click_event.mode = LIS2DH_CLICK_SINGLE_POSITIVE;
    }else if(d_click && negative)
    {
        click_event.mode = LIS2DH_CLICK_DOUBLE_NEGATIVE;
    }else if(d_click && !negative)
    {
        click_event.mode = LIS2DH_CLICK_DOUBLE_POSITIVE;
    }else
    {
        click_event.mode = LIS2DH_CLICK_OFF;
    }

    if(x)
    {
        click_event.direction = LIS2DH_CLICK_X;
    }else if(y)
    {
        click_event.direction = LIS2DH_CLICK_Y;
    }else if(z)
    {
        click_event.direction = LIS2DH_CLICK_Z;
    }else
    {
        click_event.direction = LIS2DH_CLICK_ALL;
    }

    os_eventq_put(os_eventq_dflt_get(), &gpio_ev2);

    done:
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

/**
 * Clears a single byte to the specified register
 *
 * @param The register address to write to
 * @param The value to clear
 *
 * @return 0 on success, non-zero error on failure.
 */
int
lis2dh_clear8(uint8_t reg, uint8_t value)
{
    uint8_t rc;
    uint8_t current;

    rc = lis2dh_read8(reg, &current);
    if (rc) {
        goto error;
    }

    current &= ~(value);

    rc = lis2dh_write8(reg, current);
    if (rc != 0) {
        goto error;
    }

    return 0;
error:
    return rc;
}

/**
 * Writes a single byte to the specified register
 *
 * @param The register address to write to
 * @param The value to write
 *
 * @return 0 on success, non-zero error on failure.
 */
int
lis2dh_write8(uint8_t reg, uint8_t value)
{
    int rc;

    uint8_t spi_tx_buf[2];
    spi_tx_buf[0] = reg;
    spi_tx_buf[1] = value;

    hal_gpio_write(LIS2DH_SS_PIN, 0);

    /* Register write */
    rc = hal_spi_txrx(MYNEWT_VAL(LIS2DH_SPIBUS), spi_tx_buf, NULL, 2);
    hal_gpio_write(LIS2DH_SS_PIN, 1);

    if (rc) {
//         LIS2DH_ERR("SPI access failed spi bus %d\n", MYNEWT_VAL(LIS2DH_SPIBUS));
// #if MYNEWT_VAL(LIS2DH_STATS)
//         STATS_INC(g_lis2dhstats, errors);
// #endif
        goto error;
    }

    return 0;
error:
    return rc;
}

int
lis2dh_read8(uint8_t reg, uint8_t *value)
{
    return lis2dh_readlen(reg, value, 1);
}

int
lis2dh_readlen(uint8_t reg, uint8_t *value, uint8_t length)
{
    int rc;
    uint8_t spi_tx_buf[1 + length];
    uint8_t spi_rx_buf[1 + length];

    spi_tx_buf[0] = reg | LIS2DH_READ | LIS2DH_MULTIPLE;

    hal_gpio_write(LIS2DH_SS_PIN, 0);

    /* Register write 1, read 2 */
    rc = hal_spi_txrx(MYNEWT_VAL(LIS2DH_SPIBUS), spi_tx_buf, spi_rx_buf, 1 + length);
    hal_gpio_write(LIS2DH_SS_PIN, 1);

    if (rc) {
//         LIS2DH_ERR("SPI access failed spi bus %d\n", MYNEWT_VAL(LIS2DH_SPIBUS));
// #if MYNEWT_VAL(LIS2DH_STATS)
//         STATS_INC(g_lis2dhstats, errors);
// #endif
        goto error;
    }

    *value = spi_rx_buf[1];

    return 0;
error:
    return rc;
}

int
lis2dh_get_click_cfg(struct lis2dh_cfg *cfg, uint8_t *value) {
    uint8_t rc;

    switch (cfg->click_mode) {
    case LIS2DH_CLICK_SINGLE_POSITIVE:
    case LIS2DH_CLICK_SINGLE_NEGATIVE:
    case LIS2DH_CLICK_SINGLE_BOTH:

        switch (cfg->click_direction) {
        case LIS2DH_CLICK_X:
            *value = LIS2DH_REGISTER_CLICK_CFG_XS;
            break;
        case LIS2DH_CLICK_Y:
            *value = LIS2DH_REGISTER_CLICK_CFG_YS;
            break;
        case LIS2DH_CLICK_Z:
            *value = LIS2DH_REGISTER_CLICK_CFG_ZS;
            break;
        case LIS2DH_CLICK_ALL:
            *value = LIS2DH_REGISTER_CLICK_CFG_XS |
                     LIS2DH_REGISTER_CLICK_CFG_YS |
                     LIS2DH_REGISTER_CLICK_CFG_ZS;
            break;
        default:
            rc = SYS_EINVAL;
            goto error;
        }
        break;

    case LIS2DH_CLICK_DOUBLE_POSITIVE:
    case LIS2DH_CLICK_DOUBLE_NEGATIVE:
    case LIS2DH_CLICK_DOUBLE_BOTH:
        switch (cfg->click_direction) {
        case LIS2DH_CLICK_X:
            *value = LIS2DH_REGISTER_CLICK_CFG_XD;
            break;
        case LIS2DH_CLICK_Y:
            *value = LIS2DH_REGISTER_CLICK_CFG_YD;
            break;
        case LIS2DH_CLICK_Z:
            *value = LIS2DH_REGISTER_CLICK_CFG_ZD;
            break;
        case LIS2DH_CLICK_ALL:
            *value = LIS2DH_REGISTER_CLICK_CFG_XD |
                     LIS2DH_REGISTER_CLICK_CFG_YD |
                     LIS2DH_REGISTER_CLICK_CFG_ZD;
            break;
        default:
            rc = SYS_EINVAL;
            goto error;
        }
        break;
    default:
        rc = SYS_EINVAL;
        goto error;
    }

    return 0;
error:
    return rc;
}

int
lis2dh_accel_configure(struct lis2dh_cfg *cfg)
{
    uint8_t rc;
    uint8_t ctrl_reg4 = cfg->accel_range;
    uint8_t ctrl_reg1 = cfg->accel_rate;

    switch(cfg->accel_mode)
    {
        case LIS2DH_PWR_MODE_NORMAL:
            ctrl_reg1 = ctrl_reg1 |
                LIS2DH_REGISTER_CTRL_REG1_ZEN |
                LIS2DH_REGISTER_CTRL_REG1_YEN |
                LIS2DH_REGISTER_CTRL_REG1_XEN;
            break;
        case LIS2DH_PWR_MODE_LOWPOWER:
            ctrl_reg1 = ctrl_reg1 |
                LIS2DH_REGISTER_CTRL_REG1_LPEN |
                LIS2DH_REGISTER_CTRL_REG1_ZEN |
                LIS2DH_REGISTER_CTRL_REG1_YEN |
                LIS2DH_REGISTER_CTRL_REG1_XEN;
            break;
        case LIS2DH_PWR_MODE_HIGHRESOLUTION:
            ctrl_reg4 = ctrl_reg4 | LIS2DH_REGISTER_CTRL_REG4_HR;
            ctrl_reg1 = ctrl_reg1 |
                LIS2DH_REGISTER_CTRL_REG1_ZEN |
                LIS2DH_REGISTER_CTRL_REG1_YEN |
                LIS2DH_REGISTER_CTRL_REG1_XEN;
            break;
        case LIS2DH_PWR_MODE_SUSPEND:
            break;
        default:
            rc = SYS_EINVAL;
            goto error;
    }

    /* Set normal mode, accel data rate and enable XYZ output */
    rc = lis2dh_write8(LIS2DH_REGISTER_CTRL_REG1, ctrl_reg1);
    if (rc != 0) {
        goto error;
    }

    /* Set accel scale */
    rc = lis2dh_write8(LIS2DH_REGISTER_CTRL_REG4, ctrl_reg4);
    if (rc != 0) {
        goto error;
    }

    return 0;
error:
    return rc;
}

int
lis2dh_click_configure(struct lis2dh_cfg *cfg)
{
    uint8_t rc;
    uint8_t click_cfg;

    // TODO do I care if this is a second call and we already have a callback function?

    if (cfg->click_mode == LIS2DH_CLICK_OFF) {
        hal_gpio_irq_release(LIS2DH_INT_2);
        hal_gpio_irq_disable(LIS2DH_INT_2);

        // hal_gpio_irq_release(LIS2DH_INT_1);
        // hal_gpio_irq_disable(LIS2DH_INT_1);

        // rc = lis2dh_clear8(LIS2DH_REGISTER_CTRL_REG3, LIS2DH_REGISTER_CTRL_REG3_I1_CLICK);
        // if (rc != 0) {
        //     goto error;
        // }

        rc = lis2dh_clear8(LIS2DH_REGISTER_CTRL_REG6, LIS2DH_REGISTER_CTRL_REG6_I2_CLICK);
        if (rc != 0) {
            goto error;
        }
        goto done;
    }

    rc = lis2dh_write8(LIS2DH_REGISTER_CTRL_REG6,
        LIS2DH_REGISTER_CTRL_REG6_I2_CLICK);//click on int2
    if (rc != 0) {
        goto error;
    }

    // rc = lis2dh_write8(LIS2DH_REGISTER_CTRL_REG3,
    //                    LIS2DH_REGISTER_CTRL_REG3_I1_CLICK); //click on int1
    // if (rc != 0) {
    //     goto error;
    // }

    //latch until read
    // rc = lis2dh_write8(LIS2DH_REGISTER_CTRL_REG5,
    //                    // LIS2DH_REGISTER_CTRL_REG5_LIR_INT1
    //                     // |
    //                    LIS2DH_REGISTER_CTRL_REG5_LIR_INT2
    //                   );
    // if (rc != 0) {
    //     goto error;
    // }

    rc = lis2dh_get_click_cfg(cfg, &click_cfg);
    if (rc != 0) {
        goto error;
    }

    rc = lis2dh_write8(LIS2DH_REGISTER_CLICK_CFG, click_cfg);
    if (rc != 0) {
        goto error;
    }

    rc = lis2dh_write8(LIS2DH_REGISTER_CLICK_THS,
                       cfg->click_threshold);
    if (rc != 0) {
        goto error;
    }

    /*  the maximum time interval that can elapse between the start of
        the click-detection procedure (the acceleration on the selected channel exceeds the
        programmed threshold) and when the acceleration falls back below the threshold. */
    rc = lis2dh_write8(LIS2DH_REGISTER_TIME_LIMIT,
                       cfg->click_time_limit);
    if (rc != 0) {
        goto error;
    }

    /*  the time interval that starts after the first click detection where
        the click-detection procedure is disabled, in cases where the device is configured for
        double-click detection. */
    rc = lis2dh_write8(LIS2DH_REGISTER_TIME_LATENCY,
                       cfg->click_time_latency);
    if (rc != 0) {
        goto error;
    }

    /*  the maximum interval of time that can elapse after the end of the
        latency interval in which the click-detection procedure can start, in cases where the device
        is configured for double-click detection */
    rc = lis2dh_write8(LIS2DH_REGISTER_TIME_WINDOW,
                       cfg->click_time_window);
    if (rc != 0) {
        goto error;
    }

    gpio_ev2.ev_cb = cfg->click_cb;

    // hal_gpio_irq_init(LIS2DH_INT_1, my_gpio_irq, NULL, HAL_GPIO_TRIG_RISING, HAL_GPIO_PULL_NONE);
    // hal_gpio_irq_enable(LIS2DH_INT_1);

    hal_gpio_irq_init(LIS2DH_INT_2, my_gpio_irq, NULL, HAL_GPIO_TRIG_RISING, HAL_GPIO_PULL_NONE);
    hal_gpio_irq_enable(LIS2DH_INT_2);


done:
    return 0;
error:
    return rc;
}

int
lis2dh_verify_id() {
    uint8_t id;
    int rc;

    /* Check if we can read the chip address */
    rc = lis2dh_get_chip_id(&id);
    if (rc) {
        goto error;
    }

    if (id != LIS2DH_ID) {
        os_time_delay((OS_TICKS_PER_SEC * 100) / 1000 + 1);

        rc = lis2dh_get_chip_id(&id);
        if (rc) {
            goto error;
        }

        if (id != LIS2DH_ID) {
            rc = SYS_EINVAL;
            goto error;
        }
    }

    return 0;
error:
    return (rc);
}
