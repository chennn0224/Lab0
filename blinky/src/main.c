/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include "bme280_direct.h"

#if defined(CONFIG_SUM_PRINT)
#include "sum_printk.h"
#elif defined(CONFIG_SUM_LOG)
#include "sum_log.h"
#endif

#define POLL_TIME_MS              20
#define TEMPERATURE_PRINT_TIME_MS 2000

LOG_MODULE_REGISTER(blinky, LOG_LEVEL_INF);

#define LED5180_NODE    DT_ALIAS(led5180)
#define BUTTON5180_NODE DT_ALIAS(button5180)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED5180_NODE, gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON5180_NODE, gpios);

static void run_sum_demo(void)
{
	int result;

#if defined(CONFIG_SUM_PRINT)
	result = sum_printk(7, 5);
#elif defined(CONFIG_SUM_LOG)
	result = sum_log(7, 5);
#endif

	(void)result;
}

int main(void)
{
	int ret;
	int button_state;
	int32_t temperature_centi_c;
	int32_t temperature_fraction;
	int64_t next_temperature_time = 0;
	bool button_was_pressed = false;
	bool led_state = false;
	bool bme280_ready;

	if ((gpio_is_ready_dt(&led) == false) || (gpio_is_ready_dt(&button) == false)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret < 0) {
		return 0;
	}

	run_sum_demo();
	printf("Press Button 1 to toggle LED 2\n");
	bme280_ready = bme280_direct_init() == 0;

	while (1) {
		button_state = gpio_pin_get_dt(&button);
		if (button_state < 0) {
			return 0;
		}

		if ((button_state > 0) && (button_was_pressed == false)) {
			ret = gpio_pin_toggle_dt(&led);
			if (ret < 0) {
				return 0;
			}

			led_state = !led_state;
			printf("LED 2: %s\n", led_state ? "ON" : "OFF");
		}

		button_was_pressed = button_state > 0;

		if (bme280_ready && (k_uptime_get() >= next_temperature_time)) {
			ret = bme280_direct_read_temperature(&temperature_centi_c);
			if (ret < 0) {
				LOG_ERR("Could not read temperature: %d", ret);
			} else {
				temperature_fraction = temperature_centi_c % 100;
				if (temperature_fraction < 0) {
					temperature_fraction = -temperature_fraction;
				}

				if (temperature_centi_c < 0) {
					LOG_INF("Temperature: -%d.%02d C",
						-temperature_centi_c / 100, temperature_fraction);
				} else {
					LOG_INF("Temperature: %d.%02d C", temperature_centi_c / 100,
						temperature_fraction);
				}
			}

			next_temperature_time = k_uptime_get() + TEMPERATURE_PRINT_TIME_MS;
		}

		k_msleep(POLL_TIME_MS);
	}
	return 0;
}
