/*
 * Copyright (c) 2019 Nordic Semiconductor ASA.
 * Copyright (c) Ericsson AB 2021, all rights reserved
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// Including Ardesco.h to get the MCU pin assignments.
#include <ardesco.h>

#include <drivers/gpio.h>
#include <drivers/uart.h>
#include <device.h>

//(db) #define RESET_PIN CONFIG_BOARD_NRF52840_GPIO_RESET_PIN
//(db) Hard coding the reset pin to the 52840 to MCU_2 (pin p0.19)
#define RESET_PIN 19

/*
 * bt_hci_transport_setup - This routine is called
 * from the bluetooth driver during driver init.
 * It's used to toggle the GPIO that resets the
 * 52840. The reset syncs the uart comms between
 * the two chips.
 */
int bt_hci_transport_setup(const struct device *h4)
{
	int err;
	char c;
	const struct device *port;

	// Get the GPIO driver.
	port = device_get_binding(DT_LABEL(DT_NODELABEL(gpio0)));
	if (!port) {
		return -EIO;
	}

	/* Configure pin as output and initialize it to low. */
	err = gpio_pin_configure(port, RESET_PIN, GPIO_OUTPUT_LOW);
	if (err) {
		return err;
	}

	/* Reset the nRF52840 and let it wait until the pin is
	 * pulled low again before running to main to ensure
	 * that it won't send any data until the H4 device
	 * is setup and ready to receive.
	 */
	err = gpio_pin_set(port, RESET_PIN, 1);
	if (err) {
		return err;
	}

	/* Wait for the nRF52840 peripheral to stop sending data.
	 *
	 * It is critical (!) to wait here, so that all bytes
	 * on the lines are received and drained correctly.
	 */
	k_sleep(K_MSEC(25));

	/* Drain bytes */
	while (uart_fifo_read(h4, &c, 1)) {
		continue;
	}

	/* We are ready, let the nRF52840 run to main */
	err = gpio_pin_set(port, RESET_PIN, 0);
	if (err) {
		return err;
	}

	return 0;
}
