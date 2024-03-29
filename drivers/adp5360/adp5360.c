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

#include <zephyr.h>
#include <device.h>
#include <drivers/i2c.h>
#include <sys/util.h>

#define ADP536X_I2C_ADDR				0x46

/* Register addresses */
#define ADP536X_MANUF_MODEL				0x00
#define ADP536X_SILICON_REV				0x01
#define ADP536X_CHG_VBUS_ILIM				0x02
#define ADP536X_CHG_TERM_SET				0x03
#define ADP536X_CHG_CURRENT_SET				0x04
#define ADP536X_CHG_V_THRESHOLD				0x05
#define ADP536X_CHG_TIMER_SET				0x06
#define ADP536X_CHG_FUNC				0x07
#define ADP536X_CHG_STATUS_1				0x08
#define ADP536X_CHG_STATUS_2				0x09
#define ADP536X_BAT_PROTECT_CTRL			0x11
#define ADP536X_BAT_OC_CHG				0x15
#define ADP536X_V_SOC_0					0x16
#define ADP536X_V_SOC_5					0x17
#define ADP536X_V_SOC_11				0x18
#define ADP536X_V_SOC_19				0x19
#define ADP536X_V_SOC_28				0x1A
#define ADP536X_V_SOC_41				0x1B
#define ADP536X_V_SOC_55				0x1C
#define ADP536X_V_SOC_69				0x1D
#define ADP536X_V_SOC_84				0x1E
#define ADP536X_V_SOC_100				0x1F
#define ADP536X_BAT_CAP					0x20
#define ADP536X_BAT_SOC					0x21
#define ADP536X_BAT_SOCACM_CTL				0x22
#define ADP536X_FUEL_GAUGE_MODE				0x27
#define ADP536X_BUCK_CFG				0x29
#define ADP536X_BUCK_OUTPUT				0x2A
#define ADP536X_BUCKBST_CFG				0x2B
#define ADP536X_BUCKBST_OUTPUT				0x2C
//(db) Add IRQ regs
#define ADP536X_INTERRUPT_ENABLE1		0x32
#define ADP536X_INTERRUPT_ENABLE2		0x33
#define ADP536X_INTERRUPT_FLAG1			0x34
#define ADP536X_INTERRUPT_FLAG2			0x35
//(db) End if add IRQ regs
#define ADP536X_DEFAULT_SET_REG				0x37

/* Manufacturer and model ID register. */
#define ADP536X_MANUF_MODEL_MANUF_MSK			GENMASK(7, 4)
#define ADP536X_MANUF_MODEL_MANUF(x)			(((x) & 0x0F) << 4)
#define ADP536X_MANUF_MODEL_MODEL_MSK			GENMASK(3, 0)
#define ADP536X_MANUF_MODEL_MODEL(x)			((x) & 0x0F)

/* Silicon revision register. */
#define ADP536X_SILICON_REV_REV_MSK			GENMASK(4, 0)
#define ADP536X_SILICON_REV_REV(x)			((x) & 0x0F)

/* Charger VBUS ILIM register. */
#define ADP536X_CHG_VBUS_ILIM_VADPICHG_MSK		GENMASK(7, 5)
#define ADP536X_CHG_VBUS_ILIM_VADPICHG(x)		(((x) & 0x07) << 5)
/* Bit 4 not used, according to datasheet. */
#define ADP536X_CHG_VBUS_ILIM_VSYSTEM_MSK		BIT(3)
#define ADP536X_CHG_VBUS_ILIM_VSYSTEM(x)		(((x) & 0x01) << 3)
#define ADP536X_CHG_VBUS_ILIM_ILIM_MSK			GENMASK(2, 0)
#define ADP536X_CHG_VBUS_ILIM_ILIM(x)			((x) & 0x07)

/* Charger termination settings register. */
#define ADP536X_CHG_TERM_SET_VTRM_MSK			GENMASK(7, 2)
#define ADP536X_CHG_TERM_SET_VTRM(x)			(((x) & 0x3F) << 2)
#define ADP536X_CHG_TERM_SET_ITRK_DEAD_MSK		GENMASK(1, 0)
#define ADP536X_CHG_TERM_SET_ITRK_DEAD(x)		((x) & 0x03)

/* Charger current setting register. */
#define ADP536X_CHG_CURRENT_SET_IEND_MSK		GENMASK(7, 5)
#define ADP536X_CHG_CURRENT_SET_IEND(x)			(((x) & 0x07) << 5)
#define ADP536X_CHG_CURRENT_SET_ICHG_MSK		GENMASK(4, 0)
#define ADP536X_CHG_CURRENT_SET_ICHG(x)			((x) & 0x1F)

/* Charger voltage threshold. */
#define ADP536X_CHG_V_THRESHOLD_DIS_RCH_MSK		BIT(7)
#define ADP536X_CHG_V_THRESHOLD_DIS_RCH(x)		(((x) & 0x01) << 7)
#define ADP536X_CHG_V_THRESHOLD_VRCH_MSK		GENMASK(6, 5)
#define ADP536X_CHG_V_THRESHOLD_VRCH(x)			(((x) & 0x03) << 5)
#define ADP536X_CHG_V_THRESHOLD_VTRK_DEAS_MSK		GENMASK(4, 3)
#define ADP536X_CHG_V_THRESHOLD_VTRK_DEAS(x)		(((x) & 0x03) << 3)
#define ADP536X_CHG_V_THRESHOLD_VWEAK_MSK		GENMASK(2, 0)
#define ADP536X_CHG_V_THRESHOLD_VWEAK(x)		((x) & 0x07)

/* Charger timer setting register. */
#define ADP536X_CHG_TIMER_SET_EN_TEND_MSK		BIT(3)
#define ADP536X_CHG_TIMER_SET_EN_TEND(x)		(((x) & 0x01) << 3)
#define ADP536X_CHG_TIMER_SET_EN_CHG_TIMER_MSK		BIT(2)
#define ADP536X_CHG_TIMER_SET_EN_CHG_TIMER(x)		(((x) & 0x01) << 2)
#define ADP536X_CHG_TIMER_SET_CHG_TMR_PERIOD_MSK	GENMASK(1, 0)
#define ADP536X_CHG_TIMER_SET_CHG_TMR_PERIOD(x)		((x) & 0x03)

/* Charger functional settings register. */
#define ADP536X_CHG_FUNC_EN_JEITA_MSK			BIT(7)
#define ADP536X_CHG_FUNC_EN_JEITA(x)			(((x) & 0x01) << 7)
#define ADP536X_CHG_FUNC_ILIM_JEITA_COOL_MSK		BIT(6)
#define ADP536X_CHG_FUNC_ILIM_JEAITA_COOL(x)		(((x) & 0x01) << 6)
/* Bit number 5 is unused according to datasheet.  */
#define ADP536X_CHG_FUNC_OFF_ISOFET_MSK			BIT(4)
#define ADP536X_CHG_FUNC_OFF_ISOFET(x)			(((x) & 0x01) << 4)
#define ADP536X_CHG_FUNC_EN_LDO_MSK			BIT(3)
#define ADP536X_CHG_FUNC_EN_LDO(x)			(((x) & 0x01) << 3)
#define ADP536X_CHG_FUNC_EN_EOC_MSK			BIT(2)
#define ADP536X_CHG_FUNC_EN_EOC(x)			(((x) & 0x01) << 2)
#define ADP536X_CHG_FUNC_EN_ADPICHG_MSK			BIT(1)
#define ADP536X_CHG_FUNC_EN_ADPICHG(x)			(((x) & 0x01) << 1)
#define ADP536X_CHG_FUNC_EN_CHG_MSK			BIT(0)
#define ADP536X_CHG_FUNC_EN_CHG(x)			((x) & 0x01)

/* Battery protection control register. */
#define ADP536X_BAT_PROTECT_CTRL_ISOFET_OVCHG_MSK	BIT(4)
#define ADP536X_BAT_PROTECT_CTRL_ISOFET_OVCHG(x)	(((x) & 0x01) << 4)
#define ADP536X_BAT_PROTECT_CTRL_OC_DIS_HICCUP_MSK	BIT(3)
#define ADP536X_BAT_PROTECT_CTRL_OC_DIS_HICCUP(x)	(((x) & 0x01) << 3)
#define ADP536X_BAT_PROTECT_CTRL_OC_CHG_HICCUP_MSK	BIT(2)
#define ADP536X_BAT_PROTECT_CTRL_OC_CHG_HICCUP(x)	(((x) & 0x01) << 2)
#define ADP536X_BAT_PROTECT_CTRL_EN_CHGLB_MSK		BIT(1)
#define ADP536X_BAT_PROTECT_CTRL_EN_CHGLB(x)		(((x) & 0x01) << 1)
#define ADP536X_BAT_PROTECT_CTRL_EN_BATPRO_MSK		BIT(0)
#define ADP536X_BAT_PROTECT_CTRL_EN_BATPRO(x)		((x) & 0x01)

#define ADP536X_BAT_OC_CHG_OC_CHG_MSK			GENMASK(7, 5)
#define ADP536X_BAT_OC_CHG_OC_CHG(x)			(((x) & 0x07) << 5)
#define ADP536X_BAT_OC_CHG_DGT_OC_CHG_MSK		GENMASK(4, 3)
#define ADP536X_BAT_OC_CHG_DGT_OC_CHG(x)		(((x) & 0x03) << 3)

/* Battery fuel gauge mode register. */
#define ADP536X_FUEL_GAUGE_MODE_SOC_LOW_TH_MSK		GENMASK(7, 6)
#define ADP536X_FUEL_GAUGE_MODE_SOC_LOW_TH(x)		(((x) & 0xC0) << 6)
#define ADP536X_FUEL_GAUGE_MODE_SLP_CURR_MSK		GENMASK(5, 4)
#define ADP536X_FUEL_GAUGE_MODE_SLP_CURR(x)		(((x) & 0x30) << 4)
#define ADP536X_FUEL_GAUGE_MODE_SLP_TIME_MSK		GENMASK(3, 2)
#define ADP536X_FUEL_GAUGE_MODE_SLP_TIME(x)		(((x) & 0x0C) << 2)
#define ADP536X_FUEL_GAUGE_MODE_FG_MSK			BIT(1)
#define ADP536X_FUEL_GAUGE_MODE_FG(x)			(((x) & 0x01) << 1)
#define ADP536X_FUEL_GAUGE_MODE_EN_FG_MSK		BIT(0)
#define ADP536X_FUEL_GAUGE_MODE_EN_FG(x)		((x) & 0x01)

/* Battery capacity register. */
#define ADP536X_V_SOC_MSK				GENMASK(7, 0)
#define ADP536X_V_SOC_SET(x)				(((x) & 0xFF) << 0)

/* Battery capacity register. */
#define ADP536X_BAT_CAP_MSK				GENMASK(7, 0)
#define ADP536X_BAT_CAP_SET(x)				(((x) & 0xFF) << 0)

/* Battery state of charge register */
#define ADP536X_BAT_SOC_MSK				GENMASK(6, 0)
#define ADP536X_BAT_SOC_SET(x)				(((x) & 0xFF) << 0)

/* Buck configure register. */
#define ADP536X_BUCK_CFG_DISCHG_BUCK_MSK		BIT(1)
#define ADP536X_BUCK_CFG_DISCHG_BUCK(x)			(((x) & 0x01) << 1)

/* Buck output voltage setting register. */
#define ADP536X_BUCK_OUTPUT_VOUT_BUCK_MSK		GENMASK(5, 0)
#define ADP536X_BUCK_OUTPUT_VOUT_BUCK(x)		(((x) & 0x3F) << 0)
#define ADP536X_BUCK_OUTPUT_BUCK_DLY_MSK		GENMASK(7, 6)
#define ADP536X_BUCK_OUTPUT_BUCK_DLY(x)			(((x) & 0x03) << 6)

/* Buck/boost output voltage setting register. */
#define ADP536X_BUCKBST_OUTPUT_VOUT_BUCKBST_MSK		GENMASK(5, 0)
#define ADP536X_BUCKBST_OUTPUT_VOUT_BUCKBST(x)		(((x) & 0x3F) << 0)
#define ADP536X_BUCKBST_OUT_BUCK_DLY_MSK		GENMASK(7, 6)
#define ADP536X_BUCKBST_OUT_BUCK_DLY(x)			(((x) & 0x03) << 6)

/* Buck/boost configure register. */
#define ADP536X_BUCKBST_CFG_EN_BUCKBST_MSK		BIT(0)
#define ADP536X_BUCKBST_CFG_EN_BUCKBST(x)		(((x) & 0x01) << 0)

/* DEFAULT_SET register. */
#define ADP536X_DEFAULT_SET_MSK				GENMASK(7, 0)
#define ADP536X_DEFAULT_SET(x)				(((x) & 0xFF) << 0)

/* Interrupt reg bits are defined in .h file so that callers can select the proper interrupts. */


static const struct device *i2c_dev;

static int adp536x_reg_read(uint8_t reg, uint8_t *buff)
{
	return i2c_reg_read_byte(i2c_dev, ADP536X_I2C_ADDR, reg, buff);
}

static int adp536x_reg_write(uint8_t reg, uint8_t val)
{
	return i2c_reg_write_byte(i2c_dev, ADP536X_I2C_ADDR, reg, val);
}

static int adp536x_reg_write_mask(uint8_t reg_addr,
			       uint32_t mask,
			       uint8_t data)
{
	int err;
	uint8_t tmp;

	err = adp536x_reg_read(reg_addr, &tmp);
	if (err) {
		return err;
	}

	tmp &= ~mask;
	tmp |= data;

	return adp536x_reg_write(reg_addr, tmp);
}

int adp536x_charger_current_set(uint8_t value)
{
	return adp536x_reg_write_mask(ADP536X_CHG_CURRENT_SET,
					ADP536X_CHG_CURRENT_SET_ICHG_MSK,
					ADP536X_CHG_CURRENT_SET_ICHG(value));
}

int adp536x_vbus_current_set(uint8_t value)
{
	return adp536x_reg_write_mask(ADP536X_CHG_VBUS_ILIM,
					ADP536X_CHG_VBUS_ILIM_ILIM_MSK,
					ADP536X_CHG_VBUS_ILIM_ILIM(value));
}

int adp536x_charger_termination_voltage_set(uint8_t value)
{
	return adp536x_reg_write_mask(ADP536X_CHG_TERM_SET,
					ADP536X_CHG_TERM_SET_VTRM_MSK,
					ADP536X_CHG_TERM_SET_VTRM(value));
}

int adp536x_charger_ldo_enable(bool enable)
{
	return adp536x_reg_write_mask(ADP536X_CHG_FUNC,
					ADP536X_CHG_FUNC_EN_LDO_MSK,
					ADP536X_CHG_FUNC_EN_LDO(enable));
}

int adp536x_charging_enable(bool enable)
{
	return adp536x_reg_write_mask(ADP536X_CHG_FUNC,
					ADP536X_CHG_FUNC_EN_CHG_MSK,
					ADP536X_CHG_FUNC_EN_CHG(enable));
}

int adp536x_charger_status_1_read(uint8_t *buf)
{
	return adp536x_reg_read(ADP536X_CHG_STATUS_1, buf);
}

int adp536x_charger_status_2_read(uint8_t *buf)
{
	return adp536x_reg_read(ADP536X_CHG_STATUS_2, buf);
}

int adp536x_bat_soc_read(uint8_t *buf)
{
	return adp536x_reg_read(ADP536X_BAT_SOC, buf);
}

int adp536x_oc_dis_hiccup_set(bool enable)
{
	return adp536x_reg_write_mask(ADP536X_BAT_PROTECT_CTRL,
				ADP536X_BAT_PROTECT_CTRL_OC_DIS_HICCUP_MSK,
				ADP536X_BAT_PROTECT_CTRL_OC_DIS_HICCUP(enable));
}

int adp536x_oc_chg_hiccup_set(bool enable)
{
	return adp536x_reg_write_mask(ADP536X_BAT_PROTECT_CTRL,
				ADP536X_BAT_PROTECT_CTRL_OC_CHG_HICCUP_MSK,
				ADP536X_BAT_PROTECT_CTRL_OC_CHG_HICCUP(enable));
}

int adp536x_oc_chg_current_set(uint8_t value)
{
	return adp536x_reg_write_mask(ADP536X_BAT_OC_CHG,
					ADP536X_BAT_OC_CHG_OC_CHG_MSK,
					ADP536X_BAT_OC_CHG_OC_CHG(value));
}

int adp536x_bat_cap_set(uint8_t value)
{
	return adp536x_reg_write_mask(ADP536X_BAT_CAP,
					ADP536X_BAT_CAP_MSK,
					ADP536X_BAT_CAP_SET(value));
}

int adp536x_fuel_gauge_set(bool enable, uint8_t *curve)
{
	uint8_t reg;
	int err;
	for (reg = ADP536X_V_SOC_0; reg <= ADP536X_V_SOC_100; reg++) {
		err = adp536x_reg_write_mask(reg,
					     ADP536X_V_SOC_MSK,
					     ADP536X_V_SOC_SET(*curve++));
		if (err) {
			return err;
		}
	}
	return adp536x_reg_write_mask(ADP536X_FUEL_GAUGE_MODE,
				      ADP536X_FUEL_GAUGE_MODE_EN_FG_MSK,
				      ADP536X_FUEL_GAUGE_MODE_EN_FG(enable));
}

int adp536x_fuel_gauge_enable_sleep_mode(bool enable)
{
	return adp536x_reg_write_mask(ADP536X_FUEL_GAUGE_MODE,
				      ADP536X_FUEL_GAUGE_MODE_FG_MSK,
				      ADP536X_FUEL_GAUGE_MODE_FG(enable));
}

int adp536x_fuel_gauge_update_rate_set(uint8_t rate)
{
	return adp536x_reg_write_mask(ADP536X_FUEL_GAUGE_MODE,
				      ADP536X_FUEL_GAUGE_MODE_SLP_TIME_MSK,
				      ADP536X_FUEL_GAUGE_MODE_SLP_TIME(rate));
}

int adp536x_buck_1v8_set(void)
{
	/* 1.8V equals to 0b11000 = 0x18 according to ADP536X datasheet. */
	uint8_t value = 0x18;

	return adp536x_reg_write_mask(ADP536X_BUCK_OUTPUT,
					ADP536X_BUCK_OUTPUT_VOUT_BUCK_MSK,
					ADP536X_BUCK_OUTPUT_VOUT_BUCK(value));
}

int adp536x_buck_discharge_set(bool enable)
{
	return adp536x_reg_write_mask(ADP536X_BUCK_CFG,
				ADP536X_BUCK_CFG_DISCHG_BUCK_MSK,
				ADP536X_BUCK_CFG_DISCHG_BUCK(enable));
}

int adp536x_buckbst_3v3_set(void)
{
	/* 3.3V equals to 0b10011 = 0x13, according to ADP536X datasheet. */
	uint8_t value = 0x13;

	return adp536x_reg_write_mask(ADP536X_BUCKBST_OUTPUT,
				ADP536X_BUCKBST_OUTPUT_VOUT_BUCKBST_MSK,
				ADP536X_BUCKBST_OUTPUT_VOUT_BUCKBST(value));
}

int adp536x_buckbst_enable(bool enable)
{
	return adp536x_reg_write_mask(ADP536X_BUCKBST_CFG,
					ADP536X_BUCKBST_CFG_EN_BUCKBST_MSK,
					ADP536X_BUCKBST_CFG_EN_BUCKBST(enable));
}

int adp536x_irq_enable(uint8_t reg1, uint8_t reg2)
{
	int rc;
	rc = adp536x_reg_write (ADP536X_INTERRUPT_ENABLE1, reg1);
	if (rc == 0)
	{
		rc = adp536x_reg_write (ADP536X_INTERRUPT_ENABLE2, reg2);
	}
	return rc;
}

int adp536x_irq_enable_read(uint8_t *reg1, uint8_t *reg2)
{
	int rc;
	rc = adp536x_reg_read (ADP536X_INTERRUPT_ENABLE1, reg1);
	if (rc == 0)
	{
		rc = adp536x_reg_read (ADP536X_INTERRUPT_ENABLE2, reg2);
	}
	return rc;
}

int adp536x_irq_status(uint8_t *reg1, uint8_t *reg2)
{
	int rc;
	rc = adp536x_reg_read (ADP536X_INTERRUPT_FLAG1, reg1);
	if (rc == 0)
	{
		rc = adp536x_reg_read (ADP536X_INTERRUPT_FLAG2, reg2);
	}
	return rc;
}

int adp536x_irq_ack1(uint8_t reg1)
{
	return adp536x_reg_write (ADP536X_INTERRUPT_FLAG1, reg1);
}

int adp536x_irq_ack2(uint8_t reg2)
{
	return adp536x_reg_write (ADP536X_INTERRUPT_FLAG2, reg2);
}

static int adp536x_default_set(void)
{
	/* The value 0x7F has to be written to this register to accomplish
	 * factory reset of the I2C registers, according to ADP536X datasheet.
	 */
	return adp536x_reg_write_mask(ADP536X_DEFAULT_SET_REG,
					ADP536X_DEFAULT_SET_MSK,
					ADP536X_DEFAULT_SET(0x7F));
}

int adp536x_factory_reset(void)
{
	int err;

	err = adp536x_default_set();
	if (err) {
		printk("adp536x_default_set failed: %d\n", err);
		return err;
	}

	return 0;
}

int adp536x_init(const char *dev_name)
{
	int err = 0;

	i2c_dev = device_get_binding(dev_name);
	if (i2c_dev == 0) {
		err = -ENODEV;
	}

	return err;
}
