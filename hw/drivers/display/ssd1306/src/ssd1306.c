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
#include "ssd1306/ssd1306.h"
#include "bsp/bsp.h"
#include "hal/hal_gpio.h"
#include "hal/hal_spi.h"

#if MYNEWT_VAL(SSD1306_LOG)
#include "log/log.h"
#endif

#if MYNEWT_VAL(SSD1306_STATS)
#include "stats/stats.h"
#endif

#if MYNEWT_VAL(SSD1306_STATS)
/* Define the stats section and records */
STATS_SECT_START(ssd1306_stat_section)
STATS_SECT_ENTRY(errors)
STATS_SECT_END

/* Define stat names for querying */
STATS_NAME_START(ssd1306_stat_section)
STATS_NAME(ssd1306_stat_section, errors)
STATS_NAME_END(ssd1306_stat_section)

/* Global variable used to hold stats data */
STATS_SECT_DECL(ssd1306_stat_section) g_ssd1306stats;
#endif

#if MYNEWT_VAL(SSD1306_LOG)
#define LOG_MODULE_SSD1306 (310)
#define SSD1306_INFO(...)  LOG_INFO(&_log, LOG_MODULE_SSD1306, __VA_ARGS__)
#define SSD1306_ERR(...)   LOG_ERROR(&_log, LOG_MODULE_SSD1306, __VA_ARGS__)
static struct log _log;
#else
#define SSD1306_INFO(...)
#define SSD1306_ERR(...)
#endif

int
ssd1306_default_cfg(struct ssd1306_cfg *cfg)
{
    cfg->height = 96;
    cfg->width = 16;
    cfg->pwr_mode = SSD1306_SWITCHCAPVCC;
    return 0;
}

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
int
ssd1306_startscrollright(struct ssd1306 *ssd, uint8_t start, uint8_t stop)
{
    int rc;

    ssd1306_enable_command();

    rc = ssd1306_write8(SSD1306_RIGHT_HORIZONTAL_SCROLL);
    if(rc) goto error;

    rc = ssd1306_write8(0X00);
    if(rc) goto error;

    rc = ssd1306_write8(start);
    if(rc) goto error;

    rc = ssd1306_write8(0X00);
    if(rc) goto error;

    rc = ssd1306_write8(stop);
    if(rc) goto error;

    rc = ssd1306_write8(0X00);
    if(rc) goto error;

    rc = ssd1306_write8(0XFF);
    if(rc) goto error;

    rc = ssd1306_write8(SSD1306_ACTIVATE_SCROLL);
    if(rc) goto error;

    return 0;
error:
    return rc;
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
int
ssd1306_startscrollleft(struct ssd1306 *ssd, uint8_t start, uint8_t stop)
{
    int rc;

    ssd1306_enable_command();

    rc = ssd1306_write8(SSD1306_LEFT_HORIZONTAL_SCROLL);
    if(rc) goto error;

    rc = ssd1306_write8(0X00);
    if(rc) goto error;

    rc = ssd1306_write8(start);
    if(rc) goto error;

    rc = ssd1306_write8(0X00);
    if(rc) goto error;

    rc = ssd1306_write8(stop);
    if(rc) goto error;

    rc = ssd1306_write8(0X00);
    if(rc) goto error;

    rc = ssd1306_write8(0XFF);
    if(rc) goto error;

    rc = ssd1306_write8(SSD1306_ACTIVATE_SCROLL);
    if(rc) goto error;

    return 0;
error:
    return rc;
}

// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
int
ssd1306_startscrolldiagright(struct ssd1306 *ssd, uint8_t start, uint8_t stop)
{
    int rc;

    ssd1306_enable_command();

    rc = ssd1306_write8(SSD1306_SET_VERTICAL_SCROLL_AREA);
    if(rc) goto error;

    rc = ssd1306_write8(0X00);
    if(rc) goto error;

    rc = ssd1306_write8(ssd->cfg.height);
    if(rc) goto error;

    rc = ssd1306_write8(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
    if(rc) goto error;

    rc = ssd1306_write8(0X00);
    if(rc) goto error;

    rc = ssd1306_write8(start);
    if(rc) goto error;

    rc = ssd1306_write8(0X00);
    if(rc) goto error;

    rc = ssd1306_write8(stop);
    if(rc) goto error;

    rc = ssd1306_write8(0X01);
    if(rc) goto error;

    rc = ssd1306_write8(SSD1306_ACTIVATE_SCROLL);
    if(rc) goto error;

    return 0;
error:
    return rc;
}

// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
int
ssd1306_startscrolldiagleft(struct ssd1306 *ssd, uint8_t start, uint8_t stop)
{
    int rc;

    ssd1306_enable_command();

    rc = ssd1306_write8(SSD1306_SET_VERTICAL_SCROLL_AREA);
    if(rc) goto error;

    rc = ssd1306_write8(0X00);
    if(rc) goto error;

    rc = ssd1306_write8(ssd->cfg.height);
    if(rc) goto error;

    rc = ssd1306_write8(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
    if(rc) goto error;

    rc = ssd1306_write8(0X00);
    if(rc) goto error;

    rc = ssd1306_write8(start);
    if(rc) goto error;

    rc = ssd1306_write8(0X00);
    if(rc) goto error;

    rc = ssd1306_write8(stop);
    if(rc) goto error;

    rc = ssd1306_write8(0X01);
    if(rc) goto error;
    
    rc = ssd1306_write8(SSD1306_ACTIVATE_SCROLL);
    if(rc) goto error;

    return 0;
error:
    return rc;
}

int
ssd1306_stopscroll(struct ssd1306 *ssd)
{
    return ssd1306_write8(SSD1306_DEACTIVATE_SCROLL);
}

// // Dim the display
// // dim = true: display is dimmed
// // dim = false: display is normal
// int
// ssd1306_dim(boolean dim)
// {
//     uint8_t contrast;

//     if (dim) {
//         contrast = 0; // Dimmed display
//     } else {
//         if (_vccstate == SSD1306_EXTERNALVCC) {
//             contrast = 0x9F;
//         } else {
//             contrast = 0xCF;
//         }
//     }
//     // the range of contrast to too small to be really useful
//     // it is useful to dim the display
//     ssd1306_write8(SSD1306_SETCONTRAST);
//     ssd1306_write8(contrast);
// }

int
ssd1306_display(struct ssd1306 *ssd, uint8_t *buffer, uint16_t len)
{
    int rc;

    ssd1306_enable_command();

    rc = ssd1306_write8(SSD1306_COLUMNADDR);
    if(rc) goto error;

    rc = ssd1306_write8(0);   // Column start address (0 = reset)
    if(rc) goto error;

    rc = ssd1306_write8(ssd->cfg.width-1); // Column end address (127 = reset)
    if(rc) goto error;

    rc = ssd1306_write8(SSD1306_PAGEADDR);
    if(rc) goto error;

    rc = ssd1306_write8(0); // Page start address (0 = reset)
    if(rc) goto error;

    switch(ssd->cfg.width){
        case 64:
            rc = ssd1306_write8(7); // Page end address
            if(rc) goto error;
            break;
        case 32:
            rc = ssd1306_write8(3); // Page end address
            if(rc) goto error;
            break;
        case 16:
        default:
            rc = ssd1306_write8(1); // Page end address
            if(rc) goto error;
            break;
    }

    ssd1306_enable_data();

    rc = ssd1306_writelen(buffer, len);
    if(rc) goto error;

    return 0;
error:
    return rc;
}

int
ssd1306_invert(struct ssd1306 *ssd, bool invert)
{
    int rc;
    ssd1306_enable_command();
    if (invert) {
        rc = ssd1306_write8(SSD1306_INVERTDISPLAY);
    } else {
        rc = ssd1306_write8(SSD1306_NORMALDISPLAY);
    }

    return rc;
}

int
ssd1306_config(struct ssd1306 *ssd, struct ssd1306_cfg *cfg)
{
    int rc;

    /* Overwrite the configuration data. */
    memcpy(&ssd->cfg, cfg, sizeof(*cfg));

    ssd1306_reset();

    ssd1306_enable_command();

    // Init sequence
    rc = ssd1306_write8(SSD1306_DISPLAYOFF);                    // 0xAE
    if(rc) goto error;

    rc = ssd1306_write8(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    if(rc) goto error;

    rc = ssd1306_write8(0x80);                                  // the suggested ratio 0x80
    if(rc) goto error;

    rc = ssd1306_write8(SSD1306_SETMULTIPLEX);                  // 0xA8
    if(rc) goto error;

    rc = ssd1306_write8(cfg->height - 1);
    if(rc) goto error;

    rc = ssd1306_write8(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    if(rc) goto error;

    rc = ssd1306_write8(0x0);                                   // no offset
    if(rc) goto error;

    rc = ssd1306_write8(SSD1306_SETSTARTLINE | 0x0);            // line #0
    if(rc) goto error;

    rc = ssd1306_write8(SSD1306_CHARGEPUMP);                    // 0x8D
    if(rc) goto error;

    if (ssd->cfg.pwr_mode == SSD1306_EXTERNALVCC)
    { 
        rc = ssd1306_write8(0x10);
    }else
    {
        rc = ssd1306_write8(0x14);
    }
    if(rc) goto error;

    rc = ssd1306_write8(SSD1306_MEMORYMODE);                    // 0x20
    if(rc) goto error;

    rc = ssd1306_write8(0x00);                                  // 0x0 act like ks0108
    if(rc) goto error;

    rc = ssd1306_write8(SSD1306_SEGREMAP | 0x1);
    if(rc) goto error;

    rc = ssd1306_write8(SSD1306_COMSCANDEC);
    if(rc) goto error;

    switch(ssd->cfg.width){
        case 64:
            rc = ssd1306_write8(SSD1306_SETCOMPINS);                    // 0xDA
            if(rc) goto error;

            rc = ssd1306_write8(0x12);
            if(rc) goto error;

            rc = ssd1306_write8(SSD1306_SETCONTRAST);                   // 0x81
            if(rc) goto error;

            if (ssd->cfg.pwr_mode == SSD1306_EXTERNALVCC)
            {
                rc = ssd1306_write8(0x9F);
            }else
            {
                rc = ssd1306_write8(0xCF);
            }
            if(rc) goto error;

            break;
        case 32:
            rc = ssd1306_write8(SSD1306_SETCOMPINS);                    // 0xDA
            if(rc) goto error;

            rc = ssd1306_write8(0x02);
            if(rc) goto error;

            rc = ssd1306_write8(SSD1306_SETCONTRAST);                   // 0x81
            if(rc) goto error;

            rc = ssd1306_write8(0x8F);
            if(rc) goto error;

            break;
        case 16:
        default:
            rc = ssd1306_write8(SSD1306_SETCOMPINS);                    // 0xDA
            if(rc) goto error;

            rc = ssd1306_write8(0x2);   //ada x12
            if(rc) goto error;

            rc = ssd1306_write8(SSD1306_SETCONTRAST);                   // 0x81
            if(rc) goto error;

            if (ssd->cfg.pwr_mode == SSD1306_EXTERNALVCC)
            {
                rc = ssd1306_write8(0x10);
            }else
            {
                rc = ssd1306_write8(0xAF);
            }
            if(rc) goto error;

            break;
    }

    rc = ssd1306_write8(SSD1306_SETPRECHARGE);                  // 0xd9
    if(rc) goto error;

    if (ssd->cfg.pwr_mode == SSD1306_EXTERNALVCC)
    {
        rc = ssd1306_write8(0x22);
    }
    else
    {
        rc = ssd1306_write8(0xF1);
    }
    if(rc) goto error;

    rc = ssd1306_write8(SSD1306_SETVCOMDETECT);                 // 0xDB
    if(rc) goto error;

    rc = ssd1306_write8(0x40);
    if(rc) goto error;

    rc = ssd1306_write8(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    if(rc) goto error;

    rc = ssd1306_write8(SSD1306_NORMALDISPLAY);                 // 0xA6
    if(rc) goto error;

    rc = ssd1306_write8(SSD1306_DEACTIVATE_SCROLL);
    if(rc) goto error;

    rc = ssd1306_write8(SSD1306_DISPLAYON);//--turn on oled panel
    if(rc) goto error;

    return (0);
error:
    return (rc);
}

// write command cs low dc low, 
void
ssd1306_enable_command()
{
    hal_gpio_write(SSD1306_DC, 0);
}

//write data cs l dc h
void
ssd1306_enable_data()
{
    hal_gpio_write(SSD1306_DC, 1);
}

void
ssd1306_reset()
{
    hal_gpio_write(SSD1306_RESET, 0);
    os_time_delay(10 * (OS_TICKS_PER_SEC / 100));
    hal_gpio_write(SSD1306_RESET, 1);
}

// makescrollbuffercommand(uint8_t startpage, uint8_t endpage, uint8_t interval, uint8_t *buffer)
// {
//     buffer[0] = 0x00
//     buffer[1] = startpage & 0x7
//     buffer[2] = interval & 0x7
//     buffer[3] = endpage & 0x7
//     buffer[4] = 0x00
//     buffer[5] = 0xff
// }

int
ssd1306_write8(uint8_t value)
{
    uint8_t spi_tx_buf[1];
    spi_tx_buf[0] = value;

    return ssd1306_writelen(spi_tx_buf, 1);
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
ssd1306_writelen(uint8_t* buffer, uint16_t len)
{
    int rc;

    hal_gpio_write(SSD1306_SS_PIN, 0);

    /* Register write with hal_spi_txrx so we get a return code*/
    rc = hal_spi_txrx(MYNEWT_VAL(SSD1306_SPIBUS), buffer, NULL, len);
    hal_gpio_write(SSD1306_SS_PIN, 1);

    if (rc) {
        SSD1306_ERR("SPI access failed spi bus %d\n", MYNEWT_VAL(SSD1306_SPIBUS));
#if MYNEWT_VAL(SSD1306_STATS)
        STATS_INC(g_ssd1306stats, errors);
#endif
        goto error;
    }

    return 0;
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
ssd1306_init(struct os_dev *dev, void *arg)
{
    struct ssd1306 *ssd;
    int rc;

    ssd = (struct ssd1306 *) dev;

    rc = ssd1306_default_cfg(&ssd->cfg);
    if (rc) {
        goto error;
    }

    hal_gpio_init_out(SSD1306_DC, 1);
    hal_gpio_init_out(SSD1306_RESET, 1);
    hal_gpio_init_out(SSD1306_SS_PIN, 1);

    hal_gpio_init_out(W25Q80BL_SS_PIN, 1);


    struct hal_spi_settings data_struct;
    data_struct.data_order = HAL_SPI_MSB_FIRST;
    data_struct.data_mode = HAL_SPI_MODE0;
    data_struct.baudrate = MYNEWT_VAL(SSD1306_SPI_BAUDRATE);
    data_struct.word_size = HAL_SPI_WORD_SIZE_8BIT;

    rc = hal_spi_config(MYNEWT_VAL(SSD1306_SPIBUS), &data_struct);
    if (rc) {
        return (rc);
    }

    hal_spi_set_txrx_cb(MYNEWT_VAL(SSD1306_SPIBUS), NULL, NULL);
    hal_spi_enable(MYNEWT_VAL(SSD1306_SPIBUS));

#if MYNEWT_VAL(SSD1306_LOG)
    log_register("ssd1306", &_log, &log_console_handler, NULL, LOG_SYSLEVEL);
#endif

#if MYNEWT_VAL(SSD1306_STATS)
    /* Initialise the stats entry */
    rc = stats_init(
             STATS_HDR(g_ssd1306stats),
             STATS_SIZE_INIT_PARMS(g_ssd1306stats, STATS_SIZE_32),
             STATS_NAME_INIT_PARMS(ssd1306_stat_section));
    SYSINIT_PANIC_ASSERT(rc == 0);
    /* Register the entry with the stats registry */
    rc = stats_register("ssd1306", STATS_HDR(g_ssd1306stats));
    SYSINIT_PANIC_ASSERT(rc == 0);
#endif

    return (0);
error:
    return rc;
}
