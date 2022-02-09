/*
 * Copyright (c) Ericsson AB 2020, all rights reserved
 */

#ifndef ARD_BUZZER_H__
#define ARD_BUZZER_H__

// Include basic operating system dependencies
#include <ardesco.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BUZZER_MIN_FREQUENCY		100
#define BUZZER_MAX_FREQUENCY		5000
#define BUZZER_MIN_INTENSITY		0
#define BUZZER_MAX_INTENSITY		100
#define BUZZER_MIN_DUTY_CYCLE_DIV	100
#define BUZZER_MAX_DUTY_CYCLE_DIV	2


typedef void (*ardbuzz_event_cb_t)(void);

int buzzer_buzz(uint32_t frequency, uint8_t intensity);

int buzzer_init(void);

int buzzer_timed_buzz (uint32_t frequency, uint8_t intensity, uint32_t mseconds, ardbuzz_event_cb_t fn);

bool buzzer_timer_active(void);

// static void buzzer_disable(void);
// static int buzzer_enable(void);

#ifdef __cplusplus
}
#endif

#endif /* ARD_BUZZER_H__ */
