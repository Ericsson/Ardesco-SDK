/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 * Copyright (c) Ericsson AB 2020, all rights reserved
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

/*
 * Modified by Ericsson AB for Ardesco:
 * - Based on nRF driver adp536x
 * - Changed to support Ardesco battery monitoring
 */

#ifndef ADP536X_H_
#define ADP536X_H_

/**@file adp536x.h
 *
 * @brief Driver for ADP536X.
 * @defgroup adp536x Driver for ADP536X
 * @{
 */

#include <zephyr.h>

/* Definition of VBUS current limit values. */
#define ADP536X_VBUS_ILIM_50mA		0x00
#define ADP536X_VBUS_ILIM_100mA		0x01
#define ADP536X_VBUS_ILIM_150mA		0x02
#define ADP536X_VBUS_ILIM_200mA		0x03
#define ADP536X_VBUS_ILIM_250mA		0x04
#define ADP536X_VBUS_ILIM_300mA		0x05
#define ADP536X_VBUS_ILIM_400mA		0x06
#define ADP536X_VBUS_ILIM_500mA		0x07

/* Definition of charging current values. */
#define ADP536X_CHG_CURRENT_10mA        0x00
#define ADP536X_CHG_CURRENT_50mA        0x04
#define ADP536X_CHG_CURRENT_100mA	0x09
#define ADP536X_CHG_CURRENT_150mA	0x0E
#define ADP536X_CHG_CURRENT_200mA	0x13
#define ADP536X_CHG_CURRENT_250mA	0x18
#define ADP536X_CHG_CURRENT_300mA	0x1D
#define ADP536X_CHG_CURRENT_320mA	0x1F

/* Definition of overcharge protection threshold values. */
#define ADP536X_OC_CHG_THRESHOLD_25mA	0x00
#define ADP536X_OC_CHG_THRESHOLD_50mA	0x01
#define ADP536X_OC_CHG_THRESHOLD_100mA	0x02
#define ADP536X_OC_CHG_THRESHOLD_150mA	0x03
#define ADP536X_OC_CHG_THRESHOLD_200mA	0x04
#define ADP536X_OC_CHG_THRESHOLD_250mA	0x05
#define ADP536X_OC_CHG_THRESHOLD_300mA	0x06
#define ADP536X_OC_CHG_THRESHOLD_400mA	0x07


/*  INTERRUPT_ENABLE1 register*/
#define EN_SOCLOW_INT						BIT(7)
#define EN_SOCACM_INT						BIT(6)
#define EN_ADPICHG_INT						BIT(5)
#define EN_BATPRO_INT						BIT(4)
#define EN_THR_INT							BIT(3)
#define EN_BAT_INT							BIT(2)
#define EN_CHG_INT							BIT(1)
#define EN_VBUS_INT							BIT(0)

/*  INTERRUPT_ENABLE2 register*/
#define EN_MR_INT							BIT(7)
#define EN_WD_INT							BIT(6)
#define EN_BUCKPG_INT						BIT(5)
#define EN_BUCKBSTPG_INT					BIT(4)

/*  INTERRUPT_FLAG1 register*/
#define SOCLOW_INT							BIT(7)
#define SOCACM_INT							BIT(6)
#define ADPICHG_INT							BIT(5)
#define BATPRO_INT							BIT(4)
#define THR_INT								BIT(3)
#define BAT_INT								BIT(2)
#define CHG_INT								BIT(1)
#define VBUS_INT							BIT(0)

/*  INTERRUPT_FLAG2 register*/
#define MR_INT								BIT(7)
#define WD_INT								BIT(6)
#define BUCKPG_INT							BIT(5)
#define BUCKBSTPG_INT						BIT(4)


/**
 * @brief Initialize ADP536X.
 *
 * @param[in] dev_name Pointer to the device name.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int adp536x_init(const char *dev_name);

/**
 * @brief Set the VBUS current limit.
 *
 * @param[in] value The upper current threshold in LSB.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int adp536x_vbus_current_set(uint8_t value);

/**
 * @brief Set the charger current.
 *
 * @param[in] value The charger current in LSB.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int adp536x_charger_current_set(uint8_t value);

/**
 * @brief Set the charger termination voltage.
 *
 * This function configures the maximum battery voltage where
 * the charger remains active.
 *
 * @param[in] value The charger termination voltage.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int adp536x_charger_termination_voltage_set(uint8_t value);

/**
 * @brief Enable charging.
 *
 * @param[in] enable The requested charger operation state.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int adp536x_charging_enable(bool enable);

/**
 * @brief Read the STATUS1 register.
 *
 * @param[out] buf The read value of the STATUS1 register.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int adp536x_charger_status_1_read(uint8_t *buf);

/**
 * @brief Read the STATUS2 register.
 *
 * @param[out] buf The read value of the STATUS2 register.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int adp536x_charger_status_2_read(uint8_t *buf);

/**
 * @brief Read the BAT_SOC register.
 *
 * @param[out] buf The read value of the BAT_SOC register.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int adp536x_bat_soc_read(uint8_t *buf);

/**
 * @brief Enable charge hiccup protection mode.
 *
 * @param[in] enable The requested hiccup protection state.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int adp536x_oc_chg_hiccup_set(bool enable);

/**
 * @brief Enable discharge hiccup protection mode.
 *
 * @param[in] enable The requested hiccup protection state.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int adp536x_oc_dis_hiccup_set(bool enable);

/**
 * @brief Enable the buck/boost regulator.
 *
 * @param[in] enable The requested regulator operation state.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int adp536x_buckbst_enable(bool enable);

/**
 * @brief Set the buck regulator to 1.8 V.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int adp536x_buck_1v8_set(void);

/**
 * @brief Set the buck/boost regulator to 3.3 V.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int adp536x_buckbst_3v3_set(void);

/**
 * @brief Reset the device to its default values.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int adp536x_factory_reset(void);

/**
 * @brief Set the charge over-current threshold.
 *
 * @param[in] value The over-current threshold.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int adp536x_oc_chg_current_set(uint8_t value);

/**
 * @brief Set the buck discharge resistor status.
 *
 * @param[in] enable Boolean value to enable or disable the discharge resistor.
 */
int adp536x_buck_discharge_set(bool enable);

/** @brief Set the battery capacity input.
 *
 * @param[in] value The battery capacity input.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int adp536x_bat_cap_set(uint8_t value);


/**
 * @brief Enable/disable the fuel gauge.
 *
 * @param[in] enable The requested fuel gauge operation mode.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int adp536x_fuel_gauge_set(bool enable, uint8_t *curve);

/**
 * @brief Enable the fuel gauge sleep mode.
 *
 * @param[in] enable The requested fuel gauge operation mode.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int adp536x_fuel_gauge_enable_sleep_mode(bool enable);


/**
 * @brief Set the fuel gauge update rate.
 *
 * @param[in] rate Set the fuel gauge update rate.
 *                 0: 1 min
 *                 1: 4 min
 *                 2: 8 min
 *                 3: 16 min
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int adp536x_fuel_gauge_update_rate_set(uint8_t rate);

/*
 * @brief Enable selected interrupts.
 */
int adp536x_irq_enable(uint8_t reg1, uint8_t reg2);

/*
 * @brief Query status of interrupts.
 */
int adp536x_irq_status(uint8_t *reg1, uint8_t *reg2);

/*
 * @brief Acknowlege interrupts, reg1
 */
int adp536x_irq_ack1(uint8_t reg1);
/*
 * @brief Acknowlege interrupts, reg2
 */
int adp536x_irq_ack2(uint8_t reg2);


#endif /* ADP536X_H_ */
