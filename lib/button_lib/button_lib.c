/*
 * Copyright (c) Ericsson AB 2020, all rights reserved
 */

#include <ardesco.h>
#include <drivers/gpio.h>

#include <button_lib.h>

#define BUTTON_SCAN_INTERVAL  10

#if (NRF_VERSION_MAJOR == 1) && (NRF_VERSION_MINOR < 4)
	struct device *btn_gpio_dev;
#else
	const struct device *btn_gpio_dev;
#endif

// Work structure used for button state scan
static struct k_delayed_work	button_scan_work;

// gpio callback structure used by button callback.
static struct gpio_callback button_cb;

// 
static bool gpio_configured = false;

// Pointer to button callback list
static struct button_callback *cb_list = 0;

static bool first_scan = true;

/*
 * invoke_callbacks - Called to invoke each of
 * the callback functions in the list.
 */ 
static void invoke_callbacks(uint32_t btn, bool btn_pressed)
{
    struct button_callback *next = cb_list;
    while (next != 0)
    {
        // Call app. 
        (next->fn)(btn, btn_pressed, next->user_data);
        next = (struct button_callback *)next->reserved;
    }
    return;
}

/*
 * button_scan_work_fn - Called when the button is pressed
 * to detect when the button is no longer pressed.
 */ 
static void button_scan_work_fn(struct k_work *unused)
{
    int rc;
    gpio_port_value_t value;

    // Read the current value of the gpio array
	rc = gpio_port_get_raw(btn_gpio_dev, &value);

    // Look only at our gpio.
    value &= BIT(BTN_GPIO_NUM);

    // If this is the first time we've scanned, tell everyone.
    if (first_scan)
    {
        // Tell everyone the button is down.
        invoke_callbacks (0, true);
        first_scan = false;
    }
    // If button down, schedule another scan. If up, reenable gpio irq.
    if (value == 0)
    {
        k_delayed_work_submit(&button_scan_work, K_MSEC(BUTTON_SCAN_INTERVAL));
    }
    else
    {
        // Tell everyone the button is up.
        invoke_callbacks (0, false);
		rc = gpio_pin_interrupt_configure (btn_gpio_dev, BTN_GPIO_NUM, GPIO_INT_LEVEL_LOW);
    }
    return;
}

/*
 * button_cb_fn - callback function from button press. Called
 * when the user presses the Ardesco user button.
 */ 
#if (NRF_VERSION_MAJOR == 1) && (NRF_VERSION_MINOR < 4)
static void button_cb_fn(struct device *gpiob, 
					struct gpio_callback *cb, uint32_t pins)
#else
static void button_cb_fn(const struct device *gpiob, 
		    			struct gpio_callback *cb, uint32_t pins)
#endif
{
	gpio_pin_interrupt_configure(gpiob, BTN_GPIO_NUM, GPIO_INT_DISABLE);

    first_scan = true;
    k_delayed_work_submit(&button_scan_work, K_MSEC(BUTTON_SCAN_INTERVAL));
}

/* 
 * ardbutton_init - Initialize button handling
 */
int ardbutton_init (struct button_callback *cb_struct)
{
	int rc = 0;

    if (!gpio_configured)
    {
        // Get GPIO driver
        btn_gpio_dev = device_get_binding(BTN_GPIO_CTRLR);
        if (!btn_gpio_dev) {
            printk("Cannot find %s!\n", BTN_GPIO_CTRLR);
            return -ENODEV;
        }

        // Configure input gpio for button.
        rc = gpio_pin_configure(btn_gpio_dev, BTN_GPIO_NUM, GPIO_INPUT | GPIO_PULL_UP);
        if (rc == 0) 
        {
            // disable irq
            gpio_pin_interrupt_configure(btn_gpio_dev, BTN_GPIO_NUM, GPIO_INT_DISABLE);
        }
        if (rc == 0) 
        {
            // Initialize callback
            gpio_init_callback(&button_cb, button_cb_fn, BIT(BTN_GPIO_NUM));
            rc = gpio_add_callback(btn_gpio_dev, &button_cb);
        }
        if (rc == 0) 
        {
            // Enable callback.
            rc = gpio_pin_interrupt_configure (btn_gpio_dev, BTN_GPIO_NUM, GPIO_INT_LEVEL_LOW);
        }
        if (rc != 0) 
        {
            printk("Unable to configure user button!\n");
        }
    	k_delayed_work_init(&button_scan_work, button_scan_work_fn);

        gpio_configured = true;
    }
    // If callback func specified, add it to the callback list.
    if (cb_struct != 0)
    {
        bool found = false;
        // Check to see if this callback structure already in list
        struct button_callback *next = cb_list;
        while (next != 0)
        {
            // Call app. 
            if (next == cb_struct)
            {
                found = true;
                break;
            }
            next = (struct button_callback *)next->reserved;
        }
        // Only add struct to list if not already there.
        // If the structure is in list, it's ok as we'll call it.
        if (!found)
        {
            if (cb_struct->reserved == 0)
            {
                cb_struct->reserved = cb_list;
                cb_list = cb_struct;
            }
            else
            {
                printk ("callback reserved value not zero.\n");
                rc = -1;
            }
        }
    }
    return rc;
}

