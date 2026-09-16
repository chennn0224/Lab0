/*
 * SPDX-FileCopyrightText: Copyright The Zephyr Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/devicetree.h>
#include <zephyr/ztest.h>

#include "bme280_temperature.h"

#define BME280_TEST_NODE DT_NODELABEL(bme280_5180)

#if DT_NODE_EXISTS(BME280_TEST_NODE)
BUILD_ASSERT(DT_NODE_HAS_STATUS(BME280_TEST_NODE, okay),
	     "The BME280 devicetree node must be enabled");
BUILD_ASSERT(DT_REG_ADDR(BME280_TEST_NODE) == 0x77, "The BME280 devicetree address must be 0x77");

ZTEST(bme280_test_suite, test_devicetree_setup)
{
	zassert_equal(DT_REG_ADDR(BME280_TEST_NODE), 0x77, "BME280 should use address 0x77");
}
#endif

ZTEST(bme280_test_suite, test_datasheet_temperature_example)
{
	const struct bme280_temperature_calibration calibration = {
		.dig_t1 = 27504,
		.dig_t2 = 26435,
		.dig_t3 = -1000,
	};

	zassert_equal(bme280_compensate_temperature(&calibration, 519888), 2508,
		      "Datasheet example should equal 25.08 C");
}

ZTEST(bme280_test_suite, test_temperature_sanity_range)
{
	zassert_true(bme280_temperature_is_sane(-4000), "-40.00 C should be valid");
	zassert_true(bme280_temperature_is_sane(8500), "85.00 C should be valid");
}

ZTEST(bme280_test_suite, test_temperature_outside_range)
{
	zassert_false(bme280_temperature_is_sane(-4001), "Below -40.00 C should be invalid");
	zassert_false(bme280_temperature_is_sane(8501), "Above 85.00 C should be invalid");
}

ZTEST_SUITE(bme280_test_suite, NULL, NULL, NULL, NULL, NULL);
