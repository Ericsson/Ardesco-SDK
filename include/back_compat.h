/*
 * Copyright (c) Ericsson AB 2020, all rights reserved
 */

#ifndef ARD_BACKCOMPAT_H__
#define ARD_BACKCOMPAT_H__

#ifdef __cplusplus
extern "C" {
#endif

//
// v1.4.x removed the shorthand var declarations
//
#if (NRF_VERSION_MAJOR == 1) && (NRF_VERSION_MINOR > 3)
#define u8_t      uint8_t
#define u16_t     uint16_t
#define uint32_t  uint32_t
#endif

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

//
// v1.6.x uses new Zephyr kernel that redefines the delayed work funcs
//
#if (NRF_VERSION_MAJOR == 1) && (NRF_VERSION_MINOR < 6)

#else
#undef  k_delayed_work
#define k_delayed_work k_work_delayable

#undef  k_delayed_work_init
#define k_delayed_work_init   k_work_init_delayable

#undef  k_delayed_work_submit
#define k_delayed_work_submit k_work_reschedule

#undef  k_delayed_work_cancel
#define k_delayed_work_cancel k_work_cancel_delayable

#undef  k_delayed_work_submit_to_queue
#define k_delayed_work_submit_to_queue k_work_schedule_for_queue

#undef  k_delayed_work_remaining_get
#define k_delayed_work_remaining_get   k_work_delayable_remaining_get



#endif

#endif /* ARD_BACKCOMPAT_H__ */
