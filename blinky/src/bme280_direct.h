/*
 * SPDX-FileCopyrightText: Copyright The Zephyr Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BME280_DIRECT_H_
#define BME280_DIRECT_H_

#include <stdint.h>

int bme280_direct_init(void);
int bme280_direct_read_temperature(int32_t *temperature_centi_c);

#endif
