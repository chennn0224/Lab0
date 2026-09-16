/*
 * SPDX-FileCopyrightText: Copyright The Zephyr Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>

#include "bme280_temperature.h"

int32_t bme280_compensate_temperature(const struct bme280_temperature_calibration *calibration,
				      int32_t raw_temperature)
{
	int32_t var1;
	int32_t var2;
	int32_t t_fine;

	var1 = (((raw_temperature >> 3) - ((int32_t)calibration->dig_t1 << 1)) *
		(int32_t)calibration->dig_t2) >>
	       11;
	var2 = (((((raw_temperature >> 4) - (int32_t)calibration->dig_t1) *
		  ((raw_temperature >> 4) - (int32_t)calibration->dig_t1)) >>
		 12) *
		(int32_t)calibration->dig_t3) >>
	       14;

	t_fine = var1 + var2;
	return (t_fine * 5 + 128) >> 8;
}

bool bme280_temperature_is_sane(int32_t temperature_centi_c)
{
	return (temperature_centi_c >= -4000) && (temperature_centi_c <= 8500);
}
