/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Controlling LEDs through UART. Press 1-3 on your keyboard to toggle LEDS 1-3 on your development kit */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
/* STEP 3 - Include the header file of the UART driver in main.c */
#include <zephyr/drivers/uart.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 5000

/* STEP 10.1.1 - Define the size of the receive buffer */
#define RECEIVE_BUFF_SIZE 10

/* STEP 10.2 - Define the receiving timeout period */
#define RECEIVE_TIMEOUT 100

/* STEP 5.1 - Get the device pointers of the LEDs through gpio_dt_spec */
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios);

/* STEP 4.1 - Get the device pointer of the UART hardware */
const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(uart0));
/* STEP 9.1 - Define the transmission buffer, which is a buffer to hold the data to be sent over UART */
static uint8_t tx_buf[] = {"nRF Connect SDK Fundamentals Course\n\r"
                           "Press 1-4 on your keyboard to toggle LEDS 1-4 on your development kit\n\r"};

/* STEP 10.1.2 - Define the receive buffer */
static uint8_t rx_buf[RECEIVE_BUFF_SIZE] = {0};

LOG_MODULE_REGISTER(Less5_Exer1, LOG_LEVEL_DBG);

/* STEP 7 - Define the callback function for UART */
static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
    switch (evt->type)
    {
    case UART_RX_RDY:
        LOG_INF("Received '%c'", evt->data.rx.buf[evt->data.rx.offset]);
        /* Sent the value without line ending */
        /* I recommend to use Serial Monitor Extension (Microsoft) */
        if ((evt->data.rx.len) == 1)
        {
            if (evt->data.rx.buf[evt->data.rx.offset] == '1')
            {
                gpio_pin_toggle_dt(&led0);
                LOG_INF("Toggled Led 1");
            }
            else if (evt->data.rx.buf[evt->data.rx.offset] == '2')
            {
                gpio_pin_toggle_dt(&led1);
                LOG_INF("Toggled Led 2");
            }
            else if (evt->data.rx.buf[evt->data.rx.offset] == '3')
            {
                gpio_pin_toggle_dt(&led2);
                LOG_INF("Toggled Led 3");
            }
            else if (evt->data.rx.buf[evt->data.rx.offset] == '4')
            {
                gpio_pin_toggle_dt(&led3);
                LOG_INF("Toggled Led 4");
            }
        }
        else
        {
            LOG_ERR("Data received is too long. Len = %d", evt->data.rx.len);
            char buffer[] = "           ";
            for (int i = 0; i < evt->data.rx.len + 1; i++)
            {
                buffer[i] = evt->data.rx.buf[evt->data.rx.offset + i];
            }
            LOG_INF("Received '%.10s'", buffer);
            LOG_WRN("Send Data without CR nor LF");
        }

        break;
    case UART_RX_DISABLED:
        uart_rx_enable(dev, rx_buf, sizeof rx_buf, RECEIVE_TIMEOUT);
        break;
    default:
        break;
    }
}

int main(void)
{
    int ret;

    /* STEP 4.2 - Verify that the UART device is ready */
    if (!device_is_ready(uart))
    {
        LOG_INF("UART device not ready\r\n");
        return 1;
    }

    /* STEP 5.2 - Verify that the LED devices are ready */
    if (!device_is_ready(led0.port))
    { /* all the LEDs are associated with the same port, &gpio0, so we only need to check one*/
        LOG_INF("GPIO device is not ready\r\n");
        return 1;
    }

    /* STEP 6 - Configure the GPIOs of the LEDs */
    ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_HIGH);
    if (ret < 0)
    {
        return 1;
    }
    ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT_HIGH);
    if (ret < 0)
    {
        return 1;
    }
    ret = gpio_pin_configure_dt(&led2, GPIO_OUTPUT_HIGH);
    if (ret < 0)
    {
        return 1;
    }
    ret = gpio_pin_configure_dt(&led3, GPIO_OUTPUT_HIGH);
    if (ret < 0)
    {
        return 1;
    }
    /* STEP 8 - Register the UART callback function */
    ret = uart_callback_set(uart, uart_cb, NULL);
    if (ret)
    {
        return 1;
    }

    /* STEP 9.2 - Send the data over UART by calling uart_tx() */
    ret = uart_tx(uart, tx_buf, sizeof(tx_buf), SYS_FOREVER_US);
    if (ret)
    {
        return 1;
    }

    /* STEP 10.3  - Start receiving by calling uart_rx_enable() and pass it the address of the receive  buffer */

    ret = uart_rx_enable(uart, rx_buf, sizeof rx_buf, RECEIVE_TIMEOUT);
    if (ret)
    {
        return 1;
    }

    while (1)
    {
        k_msleep(SLEEP_TIME_MS);
        LOG_INF("Device running...");
    }
}