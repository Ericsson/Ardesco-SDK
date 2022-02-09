/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) Ericsson AB 2020, all rights reserved
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Modified by Ericsson AB for Ardesco:
 */
#include <ardesco.h>
#include <gpio_compat.h>
#include <drivers\pwm.h>
#include <ard_buzzer.h>

// #include <logging/log.h>
// LOG_MODULE_REGISTER(buzzer, CONFIG_UI_LOG_LEVEL);

/*
 * Buzzer definitions and Structures
 */

#if (NRF_VERSION_MAJOR == 1) && (NRF_VERSION_MINOR < 4)
struct device *buzzer_out_dev = 0;
#else
const struct device *buzzer_out_dev = 0;
#endif

#define BUZZER_PWM_NODE			    DT_ALIAS(buzzer_pwm)
#define BUZZER_PWM_PIN			    DT_PROP(BUZZER_PWM_NODE, ch0_pin)

static atomic_t buzzer_enabled;
static atomic_t buzzer_timer_busy;

static uint32_t intensity_to_duty_cycle_divisor(uint8_t intensity)
{
	return MIN(
		MAX(((intensity - BUZZER_MIN_INTENSITY) *
		    (BUZZER_MAX_DUTY_CYCLE_DIV - BUZZER_MIN_DUTY_CYCLE_DIV) /
		    (BUZZER_MAX_INTENSITY - BUZZER_MIN_INTENSITY) +
		    BUZZER_MIN_DUTY_CYCLE_DIV),
		    BUZZER_MAX_DUTY_CYCLE_DIV),
		BUZZER_MIN_DUTY_CYCLE_DIV);
}

static int pwm_out(uint32_t frequency, uint8_t intensity)
{
	static uint32_t prev_period;
	uint32_t period = (frequency > 0) ? USEC_PER_SEC / frequency : 0;
	uint32_t duty_cycle = (intensity == 0) ? 0 :
		period / intensity_to_duty_cycle_divisor(intensity);

    if (buzzer_out_dev == 0)
    {
        printk ("ERROR Buzzer lib not initialized\n");
    }
	/* Applying workaround due to limitations in PWM driver that doesn't
	 * allow changing period while PWM is running. Setting pulse to 0
	 * disables the PWM, but not before the current period is finished.
	 */
	if (prev_period) {
		pwm_pin_set_usec(buzzer_out_dev, BUZZER_PWM_PIN,
				 prev_period, 0, 0);
		k_sleep(K_MSEC(MAX((prev_period / USEC_PER_MSEC), 1)));
	}

	prev_period = period;

	return pwm_pin_set_usec(buzzer_out_dev, BUZZER_PWM_PIN,
				period, duty_cycle, 0);
}

static void buzzer_disable(void)
{
	atomic_set(&buzzer_enabled, 0);

	pwm_out(0, 0);

#ifdef CONFIG_PM_DEVICE
	int err = pm_device_state_set(buzzer_out_dev,
					 PM_DEVICE_STATE_SUSPEND,
					 NULL, NULL);
	if (err) {
		printk("PWM disable failed\n");
	}
#endif
}

static int buzzer_enable(void)
{
	int err = 0;

	atomic_set(&buzzer_enabled, 1);

#ifdef CONFIG_PM_DEVICE
	err = pm_device_state_set(buzzer_out_dev,
					 PM_DEVICE_STATE_ACTIVE,
					 NULL, NULL);
	if (err) {
		printk("PWM enable failed\n");
		return err;
	}
#endif

	return err;
}

static void stop_buzzer (void)
{
    buzzer_disable();
}
// delayed work structures for server updates.
static struct k_delayed_work timed_buzz_work;

static ardbuzz_event_cb_t buzz_done = 0;
/*
 * timed_buzz_work_fn
 */
static void timed_buzz_work_fn(struct k_work *work)
{
	printk ("timed_buzz_work_fn++\n");
	stop_buzzer();
    // Copy fn ptr in case early clearing of 
    // active bit causes a race.
    ardbuzz_event_cb_t buzz_done_now = buzz_done;

    atomic_set(&buzzer_timer_busy, 0);
    if (buzz_done_now)
    {
        (buzz_done_now)();
        buzz_done = 0;
    }
}

/*
 * buzzer_timed_buzz - Enables the buzzer for a specific amount of time.
 */
bool buzzer_timer_active(void)
{
    return atomic_get(&buzzer_timer_busy);
}
/*
 * buzzer_timed_buzz - Enables the buzzer for a specific amount of time.
 */
int buzzer_timed_buzz (uint32_t frequency, uint8_t intensity, uint32_t mseconds, ardbuzz_event_cb_t fn)
{
    int rc;

    if (mseconds == 0)
    {
        rc = -EINVAL;
    }
    else if (!atomic_get(&buzzer_timer_busy)) 
    {
        atomic_set(&buzzer_timer_busy, 1);
        buzz_done = fn;
        rc = buzzer_buzz(frequency, intensity);
        if (rc == 0)
        {
            k_delayed_work_submit(&timed_buzz_work, K_MSEC(mseconds));
        }
        else
        {
            atomic_set(&buzzer_timer_busy, 0);
            buzz_done = 0;
        }
    }
    else
    {
        rc = -EBUSY;
    }
	return rc;
}
/*
 * buzzer_buzz - Invokes the buzzer.
 * pass 0,0 to turn off buzzer.
 */
int buzzer_buzz(uint32_t frequency, uint8_t intensity)
{
	if (frequency == 0 || intensity == 0) 
    {
		printk("Frequency set to 0, disabling PWM\n");
        // If we're on a timed buzz, trigger stop immediately.
        if (atomic_get(&buzzer_timer_busy)) 
        {
            k_delayed_work_submit(&timed_buzz_work, K_NO_WAIT);
        }
        else
        {
            stop_buzzer();
        }            
		return 0;
	}

	if ((frequency < BUZZER_MIN_FREQUENCY) ||
	    (frequency > BUZZER_MAX_FREQUENCY)) {
		return -EINVAL;
	}

	if ((intensity < BUZZER_MIN_INTENSITY) ||
	    (intensity > BUZZER_MAX_INTENSITY)) {
		return -EINVAL;
	}

	if (!atomic_get(&buzzer_enabled)) {
		buzzer_enable();
	}

	return pwm_out(frequency, intensity);
}


/*
 * Function: buzzer_init
 * 
 * Description:                
 * Initialize the buzzer library
 */
int buzzer_init(void)
{
    char *buz_driver_name = DT_LABEL(DT_ALIAS(buzzer_pwm));
    buzzer_out_dev = device_get_binding(buz_driver_name);

    if (!buzzer_out_dev) 
    {
		printk ("Error opening buzzer PWM driver %s\n", buz_driver_name);
        return -1;
    }
	k_delayed_work_init(&timed_buzz_work, timed_buzz_work_fn);

    return 0;
}
