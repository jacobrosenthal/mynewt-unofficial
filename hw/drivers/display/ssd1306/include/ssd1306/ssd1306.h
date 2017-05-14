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

#ifndef __DISPLAY_SSD1306_H__
#define __DISPLAY_SSD1306_H__

#include "os/os.h"
#include "os/os_dev.h"

#ifdef __cplusplus
extern "C" {
#endif


#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D


// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A



enum ssd1306_pwr_mode {
    SSD1306_SWITCHCAPVCC                = 0x00,
    SSD1306_EXTERNALVCC                 = 0x01
};

// struct bm250_int_int_event {
//     enum bma250_int_map_0 event;
// };

struct ssd1306_cfg {
    uint8_t height;
    uint8_t width;
    enum ssd1306_pwr_mode pwr_mode;
};

struct ssd1306 {
    struct os_dev dev;
    struct ssd1306_cfg cfg;
};


// enum LIS2DH_commands {                                              /* TYPE */
//     SSD1306_REGISTER_FUNDAMENTAL_CONTRAST                   = 0x81, /* rw  */
//     SSD1306_REGISTER_FUNDAMENTAL_RESUME                     = 0xA4, /* rw  */
//     SSD1306_REGISTER_FUNDAMENTAL_ON                         = 0xA5, /* rw  */
//     SSD1306_REGISTER_FUNDAMENTAL_NORMAL                     = 0xA6, /* rw  */
//     SSD1306_REGISTER_FUNDAMENTAL_INVERSE                    = 0xA7, /* rw  */
//     SSD1306_REGISTER_FUNDAMENTAL_DISPLAY_OFF                = 0xAE, /* rw  */
//     SSD1306_REGISTER_FUNDAMENTAL_DISPLAY_ON                 = 0xAF, /* rw  */
//     SSD1306_REGISTER_SCROLLING_CONTINUOUS_RIGHT             = 0x26, /* rw  */
//     SSD1306_REGISTER_SCROLLING_CONTINUOUS_LEFT              = 0x27, /* rw  */
//     SSD1306_REGISTER_SCROLLING_CONTINUOUS_VERTICAL_RIGHT    = 0x29, /* rw  */
//     SSD1306_REGISTER_SCROLLING_CONTINUOUS_VERTICAL_LEFT     = 0x2A  /* rw  */
// };

int
ssd1306_config(struct ssd1306 *ssd, struct ssd1306_cfg *cfg);

int
ssd1306_init(struct os_dev *, void *);

void
ssd1306_reset();

void
ssd1306_enable_command();

void
ssd1306_enable_data();

int
ssd1306_write8(uint8_t value);

int
ssd1306_writelen(uint8_t* buffer, uint16_t len);

int
ssd1306_startscrollright(struct ssd1306 *ssd, uint8_t start, uint8_t stop);

int
ssd1306_startscrollleft(struct ssd1306 *ssd, uint8_t start, uint8_t stop);

int
ssd1306_startscrolldiagright(struct ssd1306 *ssd, uint8_t start, uint8_t stop);

int
ssd1306_startscrolldiagleft(struct ssd1306 *ssd, uint8_t start, uint8_t stop);

int
ssd1306_stopscroll(struct ssd1306 *ssd);

int
ssd1306_display(struct ssd1306 *ssd, uint8_t *buffer, uint16_t len);

int
ssd1306_invert(struct ssd1306 *ssd, bool invert);

#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_SSD1306_H__ */
