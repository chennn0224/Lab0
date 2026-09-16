/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define POLL_TIME_MS 20

#define LED5180_NODE DT_ALIAS(led5180)
#define BUTTON_NODE DT_ALIAS(sw0)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED5180_NODE, gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);

int main(void)
{
	int ret;
	int button_state;
	bool button_was_pressed = false;
	bool led_state = false;

	if ((gpio_is_ready_dt(&led) == false) ||
	    (gpio_is_ready_dt(&button) == false)) {
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

	printf("Press Button 1 to toggle LED 2\n");

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
		k_msleep(POLL_TIME_MS);
	}
	return 0;
}
