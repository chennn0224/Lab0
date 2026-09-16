/*
 * SPDX-FileCopyrightText: Copyright The Zephyr Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BME280_TEMPERATURE_H_
#define BME280_TEMPERATURE_H_

#include <stdbool.h>
#include <stdint.h>

struct bme280_temperature_calibration {
	uint16_t dig_t1;
	int16_t dig_t2;
	int16_t dig_t3;
};

int32_t bme280_compensate_temperature(const struct bme280_temperature_calibration *calibration,
				      int32_t raw_temperature);

bool bme280_temperature_is_sane(int32_t temperature_centi_c);

#endif
