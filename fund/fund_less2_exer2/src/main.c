/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

/* STEP 9 - Increase the sleep time from 100ms to 10 minutes  */
#define SLEEP_TIME_MS 10 * 6 * 1000 // 10 min

/* SW0_NODE is the devicetree node identifier for the "sw0" alias */
#define SW0_NODE DT_ALIAS(sw0)
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

/* LED0_NODE is the devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* Init LEDs (to avoid leak current) */
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios);
bool init_leds_low()
{
    if (!device_is_ready(led1.port) ||
        !device_is_ready(led2.port) ||
        !device_is_ready(led3.port))
    {
        return false;
    }
    if (gpio_pin_configure_dt(&led1, GPIO_OUTPUT_HIGH) < 0 ||
        gpio_pin_configure_dt(&led2, GPIO_OUTPUT_HIGH) < 0 ||
        gpio_pin_configure_dt(&led3, GPIO_OUTPUT_HIGH) < 0)
    {
        return false;
    }
    return true;
}

/* STEP 4 - Define the callback function */
void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    gpio_pin_toggle_dt(&led0);
}

/* STEP 5 - Define a variable of type static struct gpio_callback */
static struct gpio_callback button_cb;

int main(void)
{
    int ret;

    if (!init_leds_low())
    {
        return 0;
    }
    if (!device_is_ready(led0.port))
    {
        return 0;
    }

    if (!device_is_ready(button.port))
    {
        return 0;
    }

    ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_HIGH);
    if (ret < 0)
    {
        return 0;
    }

    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret < 0)
    {
        return 0;
    }

    /* STEP 3 - Configure the interrupt on the button's pin */
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);

    /* STEP 6 - Initialize the static struct gpio_callback variable   */
    gpio_init_callback(&button_cb, button_pressed, BIT(button.pin));

    /* STEP 7 - Add the callback function by calling gpio_add_callback()   */
    gpio_add_callback(button.port, &button_cb);

    while (1)
    {
        /* STEP 8 - Remove the polling code */
        k_msleep(SLEEP_TIME_MS);
    }
}