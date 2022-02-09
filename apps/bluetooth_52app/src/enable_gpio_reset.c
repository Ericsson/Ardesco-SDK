/*
 * Copyright (c) Ericsson AB 2020, all rights reserved
 */

#include <init.h>

#include <power/reboot.h>
#include <drivers/gpio.h>

// Used to locate the GPIO for the blue LED.
#define LED_GPIO_NUM         DT_GPIO_PIN(DT_ALIAS(led0), gpios)
#define LED_GPIO_CTRLR       DT_GPIO_LABEL(DT_ALIAS(led0), gpios)

/*
 * led_ctl - Simple routine to turn the LED attached to the 52840 on and off.
 */
int led_ctl (bool on)
{
	int rc;

	printk ("led_ctl++ %d\n", on);

	// Get GPIO driver
	const struct device *gpio_dev = device_get_binding(LED_GPIO_CTRLR);
	if (!gpio_dev) {
		printk("Cannot find %s!\n", LED_GPIO_CTRLR);
		return -ENODEV;
	}

	// Write gpio
	rc = gpio_pin_set_raw(gpio_dev, LED_GPIO_NUM, (on != 0) ? 1 : 0);

	printk ("led_ctl--\n");
    return rc;

}

/*
 * led_init - Initializes the GPIO connected to the blue LED.
 */
int led_init (void)
{
	int rc;

	// Get GPIO driver
	const struct device *gpio_dev = device_get_binding(LED_GPIO_CTRLR);
	if (!gpio_dev) {
		printk("Cannot find %s!\n", LED_GPIO_CTRLR);
		return -ENODEV;
	}

	// Configure input pin boot button.
	rc = gpio_pin_configure(gpio_dev, LED_GPIO_NUM, GPIO_OUTPUT | GPIO_PULL_UP);

    return rc;

}

#ifdef USE_MCU4
// Set shortcuts for gpio connected to MCU_4 on 52840
#define BOOT_PIN_GPIO_NUM         0
#define BOOT_PIN_GPIO_CTRLR       "GPIO_1"
#else
// Set shortcuts for gpio connected to MCU_2 (UART0_RTS) on 52840
#define BOOT_PIN_GPIO_NUM         20
#define BOOT_PIN_GPIO_CTRLR       "GPIO_0"
#endif

/*
 * reset_pin_wait_low - Spins on the state of the GPIO. Returns
 * when the line goes low.
 */
static void reset_pin_wait_low(const struct device *port, uint32_t pin)
{
	int val;

	/* Wait until the pin is pulled low */
	do {
		val = gpio_pin_get_raw(port, pin);
	} while (val > 0);
}

/*
 * chip_reset - callback function from gpio assert. Called
 * when 9160 raises MCU_4".
 */ 
void chip_reset(const struct device *gpiob, 
					struct gpio_callback *cb, uint32_t pins)
{
	sys_reboot(SYS_REBOOT_COLD);
}

/*
 * config_52840_reset_gpio - configure gpio for 52840 reset function.
 */ 
int config_52840_reset_gpio ()
{
	int rc;
	const struct device *gpio_dev;
	uint32_t pin = 0;
	static struct gpio_callback gpio_ctx;

	// Get GPIO driver
	gpio_dev = device_get_binding(BOOT_PIN_GPIO_CTRLR);
	if (!gpio_dev) {
		printk("Cannot find %s!\n", BOOT_PIN_GPIO_CTRLR);
		return -ENODEV;
	}

	pin = BOOT_PIN_GPIO_NUM;
	rc = gpio_pin_configure(gpio_dev, pin, GPIO_INPUT | GPIO_PULL_DOWN);
	if (rc) {
		return rc;
	}

	// Init and set callback routine when GPIO is asserted.
	gpio_init_callback(&gpio_ctx, chip_reset, BIT(pin));
	rc = gpio_add_callback(gpio_dev, &gpio_ctx);
	if (rc) {
		return rc;
	}

	// Enable GPIO interrupt
	rc = gpio_pin_interrupt_configure(gpio_dev, pin, GPIO_INT_EDGE_RISING);
	if (rc) {
		return rc;
	}

	/* Wait until the pin is pulled low before continuing.
	 * This lets the 9160 sync with our boot.
	 * Light the blue LED on Ardesco to indicate we are waiting.
	 * turn the LED off when we proceed. Useful for debugging.
	 */
	led_ctl (1);
	reset_pin_wait_low(gpio_dev, pin);
	led_ctl (0);

    return rc;
}
/*
 * gpio_reset_init - Called by OS after drivers initialized.
 */
static int gpio_reset_init(const struct device *dev)
{
	ARG_UNUSED(dev);

	led_init ();

	config_52840_reset_gpio ();

	return 0;
}
// Registers gpio_reset_init to be called during init.
SYS_INIT(gpio_reset_init, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE);

