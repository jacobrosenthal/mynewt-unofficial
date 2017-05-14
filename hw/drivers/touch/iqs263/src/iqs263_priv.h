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

#ifndef __IQS263_PRIV_H__
#define __IQS263_PRIV_H__

#define IQS263_ADDR                     0x44
#define IQS263_PROD_NUM                 0x3C
#define IQS263_VERSION_NUM              0x00

#ifdef __cplusplus
extern "C" {
#endif

#define IQS263_REGISTER_DEVICE_INFO               (0x00) /* r   */
#define IQS263_REGISTER_SYS_FLAGS                 (0x01) /* rw  */
#define IQS263_REGISTER_COORDINATES               (0x02) /* r  */
#define IQS263_REGISTER_TOUCH_BYTES               (0x03) /* r  */
#define IQS263_REGISTER_COUNTS                    (0x04) /* r  */
#define IQS263_REGISTER_LTA                       (0x05) /* r  */
#define IQS263_REGISTER_DELTAS                    (0x06) /* r  */
#define IQS263_REGISTER_MULTIPLIERS               (0x07) /* rw  */
#define IQS263_REGISTER_COMPENSATION              (0x08) /* rw  */
#define IQS263_REGISTER_PROX_SETTINGS             (0x09) /* rw  */
#define IQS263_REGISTER_THRESHOLDS                (0x0A) /* rw  */
#define IQS263_REGISTER_TIMINGS_AND_TARGETS       (0x0B) /* rw  */
#define IQS263_REGISTER_GESTURE_TIMERS            (0x0C) /* rw  */
#define IQS263_REGISTER_ACTIVE_CHANNELS           (0x0D)  /* rw */

#define IQS263_ACTIVE_CHANNELS_CH0		          (0x01 << 0)
#define IQS263_ACTIVE_CHANNELS_CH1                (0x01 << 1)
#define IQS263_ACTIVE_CHANNELS_CH2                (0x01 << 2)
#define IQS263_ACTIVE_CHANNELS_CH3                (0x01 << 3)

#define IQS263_EVENT_PROX                         (0x01 << 0)
#define IQS263_EVENT_TOUCH                        (0x01 << 1)
#define IQS263_EVENT_SLIDE                        (0x01 << 2)
#define IQS263_EVENT_ATI                          (0x01 << 3)
#define IQS263_EVENT_MOVEMENT                     (0x01 << 4)
#define IQS263_EVENT_TAP                          (0x01 << 5)
#define IQS263_EVENT_FLICK_LEFT                   (0x01 << 6)
#define IQS263_EVENT_FLICK_RIGHT                  (0x01 << 7)

#define IQS263_SYS_FLAGS_LP_ACTIVE                (0x01 << 0)
#define IQS263_SYS_FLAGS_IND_HALT                 (0x01 << 1)
#define IQS263_SYS_FLAGS_ATI_BUSY                 (0x01 << 2)
#define IQS263_SYS_FLAGS_FILTER_HALT              (0x01 << 3)
#define IQS263_SYS_FLAGS_PROJ_MODE                (0x01 << 4)
#define IQS263_SYS_FLAGS_ATI_ERROR                (0x01 << 5)
#define IQS263_SYS_FLAGS_MOVEMENT                 (0x01 << 6)
#define IQS263_SYS_FLAGS_SHOW_RESET               (0x01 << 7)

#define IQS263_PROX_SETTINGS_0_FORCE_HALT         (0x01 << 0)
#define IQS263_PROX_SETTINGS_0_4MHZ               (0x01 << 1)
#define IQS263_PROX_SETTINGS_0_STREAM_ATI         (0x01 << 2)
#define IQS263_PROX_SETTINGS_0_RESEED             (0x01 << 3)
#define IQS263_PROX_SETTINGS_0_REDO_ATI           (0x01 << 4)
#define IQS263_PROX_SETTINGS_0_ATI_BAND           (0x01 << 5)
#define IQS263_PROX_SETTINGS_0_ATI_PARTIAL        (0x01 << 6)
#define IQS263_PROX_SETTINGS_0_ATI_OFF            (0x01 << 7)

#define IQS263_PROX_SETTINGS_1_CF                 (0x01 << 0)
#define IQS263_PROX_SETTINGS_1_SLIDER             (0x01 << 2)
#define IQS263_PROX_SETTINGS_1_LTA_BETA           (0x01 << 4)
#define IQS263_PROX_SETTINGS_1_EVENT_MODE         (0x01 << 6)
#define IQS263_PROX_SETTINGS_1_WDT_OFF            (0x01 << 7)

#define IQS263_PROX_SETTINGS_1_CF_OFF             (0x00 << 0)
#define IQS263_PROX_SETTINGS_1_CF_BETA_1          (0x01 << 0)
#define IQS263_PROX_SETTINGS_1_CF_BETA_2          (0x02 << 0)
#define IQS263_PROX_SETTINGS_1_CF_BETA_3          (0x03 << 0)

#define IQS263_PROX_SETTINGS_1_SLIDER_DISABLED    (0x00 << 2)
#define IQS263_PROX_SETTINGS_1_SLIDER_2CH         (0x01 << 2)
#define IQS263_PROX_SETTINGS_1_SLIDER_WHEEL       (0x02 << 2)
#define IQS263_PROX_SETTINGS_1_SLIDER_3CH         (0x03 << 2)

#define IQS263_PROX_SETTINGS_1_LTA_BETA_2_9       (0x00 << 4)
#define IQS263_PROX_SETTINGS_1_LTA_BETA_2_8       (0x01 << 4)
#define IQS263_PROX_SETTINGS_1_LTA_BETA_2_7       (0x02 << 4)
#define IQS263_PROX_SETTINGS_1_LTA_BETA_2_6       (0x03 << 4)

#define IQS263_PROX_SETTINGS_2_OUT                (0x01 << 0)
#define IQS263_PROX_SETTINGS_2_MOVEMENT           (0x01 << 2)
#define IQS263_PROX_SETTINGS_2_WHEEL_FILTER       (0x01 << 4)
#define IQS263_PROX_SETTINGS_2_WAKE_RELEASE       (0x01 << 5)
#define IQS263_PROX_SETTINGS_2_FORCE_SLEEP        (0x01 << 6)
#define IQS263_PROX_SETTINGS_2_SLEEP_HALT         (0x01 << 7)

#define IQS263_PROX_SETTINGS_3_XFER               (0x01 << 0)
#define IQS263_PROX_SETTINGS_3_FILTERED_TOUCHES   (0x01 << 1)
#define IQS263_PROX_SETTINGS_3_TURBO_MODE         (0x01 << 2)
#define IQS263_PROX_SETTINGS_3_ATA                (0x01 << 3)
#define IQS263_PROX_SETTINGS_3_FLOAT_CX           (0x01 << 4)
#define IQS263_PROX_SETTINGS_3_PROJ_BIAS          (0x01 << 5)
#define IQS263_PROX_SETTINGS_3_CS_CAP             (0x01 << 6)
#define IQS263_PROX_SETTINGS_3_TOUCH_DEBOUNCE     (0x01 << 7)

#define IQS263_EVENT_MASK_PROX                    (0x01 << 0)
#define IQS263_EVENT_MASK_TOUCH                   (0x01 << 1)
#define IQS263_EVENT_MASK_SLIDE                   (0x01 << 2)
#define IQS263_EVENT_MASK_ATI                     (0x01 << 3)
#define IQS263_EVENT_MASK_MOVEMENT                (0x01 << 4)
#define IQS263_EVENT_MASK_TAP                     (0x01 << 5)
#define IQS263_EVENT_MASK_FLICK_RIGHT             (0x01 << 6)
#define IQS263_EVENT_MASK_FLICK_LEFT              (0x01 << 7)

#ifdef __cplusplus
}
#endif

#endif /* __IQS263_PRIV_H__ */
