/*
 * Copyright (c) Ericsson AB 2020, all rights reserved
 */

#ifndef ARD_ARDESCO_H__
#define ARD_ARDESCO_H__

// Include basic operating system dependencies
#include <zephyr.h>
#include <sys/types.h>
#include <zephyr/types.h>

#include <back_compat.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _COUNTOF(a) (sizeof(a)/sizeof(a[0]))


#ifdef CONFIG_BOARD_NRF9160_ARD0011ANS
#endif //CONFIG_BOARD_NRF9160_ARD0011ANS

// Ardesco Combi and Combi Dev
#if defined CONFIG_BOARD_NRF9160_ARD0021BNS || defined CONFIG_BOARD_NRF9160_ARD0022BNS
#endif //CONFIG_BOARD_NRF9160_ARD0021BNS || CONFIG_BOARD_NRF9160_ARD0022BNS
#if defined CONFIG_BOARD_NRF52840_ARD0021BNS || defined CONFIG_BOARD_NRF52840_ARD0022BNS
#endif //CONFIG_BOARD_NRF52840_ARD0021BNS || CONFIG_BOARD_NRF52840_ARD0022BNS

#ifdef CONFIG_BOARD_NRF9160_ARD0031ANS
#endif //CONFIG_BOARD_NRF9160_ARD0031ANS


#ifdef CONFIG_SOC_NRF9160
#define MCU_0_PIN        17  // UART_0 TX
#define MCU_1_PIN        18  // UART_0 RX
#define MCU_2_PIN        19  // UART_0 RTS
#define MCU_3_PIN        20  // UART_0 CTS

#define MCU_4_PIN        22
#define MCU_5_PIN        23
#define MCU_6_PIN        24
#define MCU_7_PIN        25
#endif // CONFIG_SOC_NRF9160

// Note RX/TX and CTS/RTS are swapped from 9160 perspective.
#ifdef CONFIG_SOC_NRF52840
#define MCU_0_PIN        11  // UART_0 TX 
#define MCU_1_PIN        15  // UART_0 RX
#define MCU_2_PIN        20  // UART_0 RTS
#define MCU_3_PIN        21  // UART_0 RTS

#define MCU_4_PIN        32
#define MCU_5_PIN        25
#define MCU_6_PIN        19
#define MCU_7_PIN        22
#endif // CONFIG_SOC_NRF52840

/* 
 * Used by 9160 to select digital vs analog on Grove data pins
 * on the Combi Dev.
 * This is defined for the Combi as well to maintain sw compat.
 */ 
#if defined CONFIG_BOARD_NRF9160_ARD0021BNS || defined CONFIG_BOARD_NRF9160_ARD0022BNS
#define BOARD_NS_GROVE_SELECT_PIN 10
#define BOARD_NS_SEL_GROVEDIGITAL 1
#endif //CONFIG_BOARD_NRF9160_ARD0021BNS || CONFIG_BOARD_NRF9160_ARD0022BNS


//
// The memory allcators/frees are dereferened here.
// 1. Simple replacement if needed.
// 2. Avoid use of stdlib if only for simple memory ops
// 3. To centralize the CONFIG_HEAP_MEM_POOL_SIZE error.
//

// If the error "undefined reference to `k_malloc'" is shown, 
// add CONFIG_HEAP_MEM_POOL_SIZE=xxx to prj.conf where xxx is a number 
// such as 2028.
static inline void * ard_malloc (size_t size) {
	return k_malloc (size);
}

// Free dereference
static inline void ard_free(void *mem) {
	return k_free (mem);
}


//
// Library related values
//

// Standard config functions are listed below
#define ARDCONFIG_GETVERSION		0
#define ARDCONFIG_SETCALLBACK		1
#define ARDCONFIG_SETMSRTIMER       2
#define ARDCONFIG_GETUNITS          3
#define ARDCONFIG_GETACCURACY       4
#define ARDCONFIG_GETFREQUENCY      5
#define ARDCONFIG_SETLIMIT          6
#define ARDCONFIG_GETLIMIT          7

// Library specific config functions start at the value below
#define ARDCONFIG_LIBSPECIFIC  0080

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif /* ARD_ARDESCO_H__ */
