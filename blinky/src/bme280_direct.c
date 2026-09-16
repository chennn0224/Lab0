/*
 * SPDX-FileCopyrightText: Copyright The Zephyr Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdint.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>

#include "bme280_direct.h"
#include "bme280_temperature.h"

LOG_MODULE_REGISTER(bme280_direct, LOG_LEVEL_INF);

#define BME280_NODE            DT_NODELABEL(bme280_5180)
#define BME280_CHIP_ID_REG     0xD0
#define BME280_CHIP_ID         0x60
#define BME280_CALIBRATION_REG 0x88
#define BME280_CTRL_MEAS_REG   0xF4
#define BME280_TEMP_MSB_REG    0xFA
#define BME280_CTRL_MEAS_VALUE 0x23

BUILD_ASSERT(DT_NODE_EXISTS(BME280_NODE), "The bme280_5180 devicetree node is missing");
BUILD_ASSERT(DT_REG_ADDR(BME280_NODE) == 0x77, "The BME280 I2C address must be 0x77");

static const struct i2c_dt_spec bme280_i2c = I2C_DT_SPEC_GET(BME280_NODE);
static struct bme280_temperature_calibration temperature_calibration;

static int bme280_read_register(uint8_t register_address, uint8_t *value)
{
	return i2c_write_read_dt(&bme280_i2c, &register_address, sizeof(register_address), value,
				 1);
}

int bme280_direct_init(void)
{
	uint8_t chip_id;
	uint8_t calibration_data[6];
	uint8_t ctrl_meas_data[2] = {
		BME280_CTRL_MEAS_REG,
		BME280_CTRL_MEAS_VALUE,
	};
	int ret;

	if (device_is_ready(bme280_i2c.bus) == false) {
		LOG_ERR("I2C bus is not ready");
		return -ENODEV;
	}

	ret = bme280_read_register(BME280_CHIP_ID_REG, &chip_id);
	if (ret < 0) {
		LOG_ERR("Could not read BME280 chip ID: %d", ret);
		return ret;
	}

	if (chip_id != BME280_CHIP_ID) {
		LOG_ERR("Unexpected BME280 chip ID: 0x%02x", chip_id);
		return -ENODEV;
	}

	ret = i2c_burst_read_dt(&bme280_i2c, BME280_CALIBRATION_REG, calibration_data,
				sizeof(calibration_data));
	if (ret < 0) {
		LOG_ERR("Could not read temperature calibration: %d", ret);
		return ret;
	}

	temperature_calibration.dig_t1 = sys_get_le16(&calibration_data[0]);
	temperature_calibration.dig_t2 = (int16_t)sys_get_le16(&calibration_data[2]);
	temperature_calibration.dig_t3 = (int16_t)sys_get_le16(&calibration_data[4]);

	ret = i2c_write_dt(&bme280_i2c, ctrl_meas_data, sizeof(ctrl_meas_data));
	if (ret < 0) {
		LOG_ERR("Could not configure BME280: %d", ret);
		return ret;
	}

	k_msleep(10);
	LOG_INF("BME280 found at I2C address 0x%02x", bme280_i2c.addr);
	return 0;
}

int bme280_direct_read_temperature(int32_t *temperature_centi_c)
{
	uint8_t raw_data[3];
	int32_t raw_temperature;
	int ret;

	if (temperature_centi_c == NULL) {
		return -EINVAL;
	}

	ret = i2c_burst_read_dt(&bme280_i2c, BME280_TEMP_MSB_REG, raw_data, sizeof(raw_data));
	if (ret < 0) {
		return ret;
	}

	raw_temperature = ((int32_t)raw_data[0] << 12) | ((int32_t)raw_data[1] << 4) |
			  ((int32_t)raw_data[2] >> 4);
	*temperature_centi_c =
		bme280_compensate_temperature(&temperature_calibration, raw_temperature);

	if (bme280_temperature_is_sane(*temperature_centi_c) == false) {
		return -ERANGE;
	}

	return 0;
}
