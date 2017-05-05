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
#include "sysinit/sysinit.h"
#include "console/console.h"
#include "shell/shell.h"
#include "sensor/sensor.h"
#include "sensor/accel.h"
#include "lis2dh/lis2dh.h"
#include "lis2dh_priv.h"

#if MYNEWT_VAL(LIS2DH_CLI)

static int lis2dh_shell_cmd(int argc, char **argv);

static struct shell_cmd lis2dh_shell_cmd_struct = {
    .sc_cmd = "lis2dh",
    .sc_cmd_func = lis2dh_shell_cmd
};

static int
lis2dh_shell_err_too_many_args(char *cmd_name)
{
    console_printf("Error: too many arguments for command \"%s\"\n",
                   cmd_name);
    return EINVAL;
}

static int
lis2dh_shell_err_unknown_arg(char *cmd_name)
{
    console_printf("Error: unknown argument \"%s\"\n",
                   cmd_name);
    return EINVAL;
}

static int
lis2dh_shell_err_invalid_arg(char *cmd_name)
{
    console_printf("Error: invalid argument \"%s\"\n",
                   cmd_name);
    return EINVAL;
}

static int
lis2dh_shell_help(void)
{
    console_printf("%s cmd [flags...]\n", lis2dh_shell_cmd_struct.sc_cmd);
    console_printf("cmd:\n");
    console_printf("\tr\t[n_samples]\n\n");
    console_printf("\tchip_id\n");
    console_printf("\tdumpreg [addr]\n");

    return 0;
}

static int
lis2dh_shell_cmd_get_chip_id(int argc, char **argv)
{
    uint8_t id;
    int rc;

    if (argc > 3) {
        return lis2dh_shell_err_too_many_args(argv[1]);
    }

    /* Display the chip id */
    if (argc == 2) {
        rc = lis2dh_get_chip_id(&id);
        if (rc) {
            console_printf("Read failed %d", rc);
        }
        console_printf("0x%02X\n", id);
    }

    return 0;
}

static int
lis2dh_shell_cmd_read(int argc, char **argv)
{
    uint16_t samples = 1;
    long val;
    int rc;
    void *databuf;
    struct sensor_accel_data *sad;
    char tmpstr[13];
    struct lis2dh lis;

    if (argc > 4) {
        return lis2dh_shell_err_too_many_args(argv[1]);
    }

    /* Since this is the biggest struct, malloc space for it */
    databuf = malloc(sizeof(struct sensor_accel_data));
    assert(databuf != NULL);

    /* Check if more than one sample requested */
    if (argc == 4) {
        if (sensor_shell_stol(argv[2], 1, UINT16_MAX, &val)) {
            return lis2dh_shell_err_invalid_arg(argv[2]);
        }
        samples = (uint16_t)val;
    }

    while (samples--) {
        rc = lis2dh_get_vector_data(databuf, &lis);
        if (rc) {
            console_printf("Read failed: %d\n", rc);
            goto err;
        }
        sad = databuf;

        console_printf("x:%s ", sensor_ftostr(sad->sad_x, tmpstr, 13));
        console_printf("y:%s ", sensor_ftostr(sad->sad_y, tmpstr, 13));
        console_printf("z:%s\n", sensor_ftostr(sad->sad_z, tmpstr, 13));
    }

    free(databuf);

    return 0;
err:
    return rc;
}

// static int
// lis2dh_shell_cmd_pwr_mode(int argc, char **argv)
// {
//     long val;
//     int rc;

//     if (argc > 3) {
//         return lis2dh_shell_err_too_many_args(argv[1]);
//     }

//     /* Display the mode */
//     if (argc == 2) {
//         rc = lis2dh_get_pwr_mode((uint8_t *)&val);
//         if (rc) {
//             goto err;
//         }
//         console_printf("%u\n", (unsigned int)val);
//     }

//     /* Update the mode */
//     if (argc == 3) {
//         if (sensor_shell_stol(argv[2], 0, LIS2DH_PWR_MODE_SUSPEND, &val)) {
//             return lis2dh_shell_err_invalid_arg(argv[2]);
//         }
//         /* Make sure mode is valid */
//         if (val > LIS2DH_PWR_MODE_SUSPEND) {
//             return lis2dh_shell_err_invalid_arg(argv[2]);
//         }

//         rc = lis2dh_set_pwr_mode(val);
//         if (rc) {
//             goto err;
//         }
//     }

//     return 0;
// err:
//     return rc;
// }

static int
lis2dh_shell_cmd_dumpreg(int argc, char **argv)
{
    long addr;
    uint8_t val;
    int rc;

    if (sensor_shell_stol(argv[2], 0, UINT8_MAX, &addr)) {
        return lis2dh_shell_err_invalid_arg(argv[2]);
    }
    rc = lis2dh_read8((uint8_t)addr, &val);
    if (rc) {
        goto err;
    }
    console_printf("0x%02X (ADDR): 0x%02X", (uint8_t)addr, val);

    return 0;
err:
    return rc;
}

//hpm1 and hpm0?
// static int
// lis2dh_shell_cmd_reset(int argc, char **argv)
// {
//     int rc;
//     /* Reset sensor */
//     rc = lis2dh_write8(lis2dh_SYS_TRIGGER_ADDR, lis2dh_SYS_TRIGGER_RST_SYS);
//     if (rc) {
//         goto err;
//     }

//     return 0;
// err:
//     return rc;
// }

static int
lis2dh_shell_cmd(int argc, char **argv)
{
    if (argc == 1) {
        return lis2dh_shell_help();
    }

    /* Read command (get a new data sample) */
    if (argc > 1 && strcmp(argv[1], "r") == 0) {
        return lis2dh_shell_cmd_read(argc, argv);
    }

    /* Chip ID command */
    if (argc > 1 && strcmp(argv[1], "chip_id") == 0) {
        return lis2dh_shell_cmd_get_chip_id(argc, argv);
    }

    /* Dump Registers command */
    if (argc > 1 && strcmp(argv[1], "dumpreg") == 0) {
        return lis2dh_shell_cmd_dumpreg(argc, argv);
    }

    return lis2dh_shell_err_unknown_arg(argv[1]);
}

int
lis2dh_shell_init(void)
{
    int rc;

    rc = shell_cmd_register(&lis2dh_shell_cmd_struct);
    SYSINIT_PANIC_ASSERT(rc == 0);

    return rc;
}

#endif
