/*
 * Copyright (c) Ericsson AB 2020, all rights reserved
 */

#include <ardesco.h>
#include <stdio.h>
#include <ard_led.h>

#if (NRF_VERSION_MAJOR == 1) && (NRF_VERSION_MINOR < 5)
#include <bsd.h>
#else
#include <modem/nrf_modem_lib.h>
#endif
#include <modem/lte_lc.h>

static struct k_sem sem_connect;
bool fconnected = false;
// We wait 2 minutes for the connection.
#define NETWORK_TIMEOUT 120

/* 
 * handles events from the LTE lib
 */ 
void lte_evt_handler(const struct lte_lc_evt *const evt)
{
	char sz[128];
	switch (evt->type) 
	{
		case LTE_LC_EVT_NW_REG_STATUS:
			printk ("LTE_LC_EVT_NW_REG_STATUS %d  ", evt->nw_reg_status);
			switch (evt->nw_reg_status)
			{
				case LTE_LC_NW_REG_NOT_REGISTERED:
					printk ("Not Registered.\n");
					break;
				case LTE_LC_NW_REG_REGISTERED_HOME:
					printk ("Registered Home.\n");
					k_sem_give(&sem_connect);
					fconnected = true;
					break;
				case LTE_LC_NW_REG_SEARCHING:
					printk ("Searching...\n");
					break;
				case LTE_LC_NW_REG_REGISTRATION_DENIED:
					printk ("Registration denied.\n");
					break;
				case LTE_LC_NW_REG_UNKNOWN:
					printk ("Registration unknown.\n");
					break;
				case LTE_LC_NW_REG_REGISTERED_ROAMING:
					printk ("Registered Roaming.\n");
					k_sem_give(&sem_connect);
					fconnected = true;
					break;
				case LTE_LC_NW_REG_REGISTERED_EMERGENCY:
					printk ("Registered Emergency.\n");
					break;
				case LTE_LC_NW_REG_UICC_FAIL:
					printk ("UICC Fail.\n");
					break;
				default:
					printk ("unknown network status\n");
					break;
			}
			break;
		case LTE_LC_EVT_PSM_UPDATE:
			printk ("LTE_LC_EVT_PSM_UPDATE act time %d tau %d\n", evt->psm_cfg.active_time, evt->psm_cfg.tau);
			break;
		case LTE_LC_EVT_EDRX_UPDATE:
			sprintf (sz, "LTE_LC_EVT_EDRX_UPDATE act time %f tau %f\n", evt->edrx_cfg.edrx, evt->edrx_cfg.ptw);
			printk ("%s", sz);
			break;
		case LTE_LC_EVT_RRC_UPDATE:
			printk ("LTE_LC_EVT_RRC_UPDATE %d  ", evt->rrc_mode);
			switch (evt->rrc_mode)
			{
				case LTE_LC_RRC_MODE_IDLE:
					printk ("Idle mode.\n");
					break;
				case LTE_LC_RRC_MODE_CONNECTED:
					printk ("Connected mode.\n");
					break;
				default:
					printk ("unknown network mode\n");
					break;
			}
			break;
		case LTE_LC_EVT_CELL_UPDATE:
			printk ("LTE_LC_EVT_CELL_UPDATE id:%d  tac:%d\n", evt->cell.id, evt->cell.tac);
			break;
		default:
			printk ("lte_evt_handler: unknown event %d\n", evt->type);
			break;
	}
}
//========================================================
// Program Entry Point
//========================================================
void main(void)
{
	int rc;
	printk("\nArdesco greets \"LTE Connect!\"\n");
	rc = led_init ();
	if (rc != 0)
	{
		printk("Failure calling led_init, rc = %d\n", rc);
	}

	// Start with white color
	led_set_color(LED_WHITE);

	printk("Calling lte_lc_init\n");
	lte_lc_init ();

	// Init a sem so we'll know when we connect. We don't want to poll.
	// Polling uses power.
	k_sem_init(&sem_connect, 0, 1);

	printk("Calling lte_lc_connect_async\n");
	lte_lc_connect_async (lte_evt_handler);
	printk("lte_lc_connect_async returned. waiting...\n");

	// Start with white color
	led_set_color(LED_YELLOW);

	// Wait for the connection
	rc = k_sem_take(&sem_connect, K_SECONDS(NETWORK_TIMEOUT));
	if (rc == -EAGAIN) 
	{
		printk ("Network connection timeout.\n");
		led_set_color(LED_RED);
	}
	else
	{
		printk ("Network connected.\n");
		led_set_color(LED_GREEN);
	}

	k_sleep(K_FOREVER);

}
