/*
 * Copyright (c) Ericsson AB 2020, all rights reserved
 */
#ifndef BUTTON_LIB_H__
#define BUTTON_LIB_H__

// Include basic operating system dependencies
#include <ardesco.h>

#ifdef __cplusplus
extern "C" {
#endif

// Set shortcuts for gpio connected to user button
#define BTN_GPIO_NUM         DT_GPIO_PIN(DT_ALIAS(sw0), gpios)
#define BTN_GPIO_CTRLR       DT_GPIO_LABEL(DT_ALIAS(sw0), gpios)


typedef int (*ardbutton_event_cb_t)(uint32_t btn, bool btn_pressed, void *user_data);

struct button_callback {
    void *reserved;
    void *user_data;
    ardbutton_event_cb_t fn;
};

//
int ardbutton_init (struct button_callback *cb_struct);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif /* BUTTON_LIB_H__ */

