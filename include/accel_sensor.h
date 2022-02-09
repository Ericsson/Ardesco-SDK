/*
 * Copyright (c) Ericsson AB 2020, all rights reserved
 */

#ifndef ACCEL_H_
#define ACCEL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/types.h>
#include "sensor_common.h"

// Names of the two devices.
#define ACCEL_8G_DEV         "ADXL362"
#define ACCEL_200G_DEV       "ADXL372"

// Structure to pass accel data
typedef struct {
	double x;			/**< X-axis acceleration [m/s^2]. */
	double y;			/**< y-axis acceleration [m/s^2]. */
	double z;			/**< z-axis acceleration [m/s^2]. */
} accel_data_t;


// Setcallback structure
struct accelsetcbstruct {
    uint32_t reasonflags;
    SenLib_trigger_fn fn;
    uint32_t userdata;
};


typedef int (*ardaccel_event_cb_t)(uint32_t btn, bool btn_pressed, void *user_data);

struct accel_callback {
    void *reserved;
    void *user_data;
    ardaccel_event_cb_t fn;
};


void *ardaccel_init(int *prc, char *driver_name);
//void *ardaccel_init(int *prc);
int ardaccel_deinit(void *h);
int ardaccel_read (void *h, void *pData, int nSize);
int ardaccel_configure(void *h, unsigned int Func, void *pData, uint32_t *pnSize);

int ardaccel_set_trig_callback(void *h, unsigned int Func, void *pData, uint32_t *pnSize);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ACCEL_H_ */
