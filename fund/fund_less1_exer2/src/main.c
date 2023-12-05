/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   500

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led_0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec led_1 = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);

int main(void)
{
	int ret;

	if (!gpio_is_ready_dt(&led_0) || !gpio_is_ready_dt(&led_1) ) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led_0, GPIO_OUTPUT_ACTIVE);
	ret = ret *  gpio_pin_configure_dt(&led_1, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	while (1) {
		ret = gpio_pin_toggle_dt(&led_0) * gpio_pin_toggle_dt(&led_1);
		if (ret < 0) {
			return 0;
		}
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}
