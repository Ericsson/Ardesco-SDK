/*
 * Copyright (c) Ericsson AB 2021, all rights reserved
 */

#include <ardesco.h>
#include <stdio.h>
#include <string.h>
#include <console/console.h>

#include "ble.h"

static struct tagdata tags[MAX_DEVS];
static int tag_cnt = 0;

struct bt_mfgrs {
	uint16_t id;
	const char *name;
};

// Very brief list of Bluetooth manufacturers.
struct bt_mfgrs mfg_list[] = {
	{0,     "Ericsson "},
	{6,     "Microsoft"},
	{76,    "Apple    "},
	{89,    "Nordic   "},
	{1177,  "Ruuvi    "},
};

/*
 * dump_bt_tag - Prints out the tag info.
 */
void dump_bt_tag (struct tagdata tag)
{
	int i;
	bool found = false;
	// The first 2 bytes of manufacturing data contain the manufacturer's ID
	uint16_t mfgr = *(uint16_t *)&tag.data[0];
	printk ("tag: ");
	prt_bt_addr (tag.addr);
	printk ("  sig: %d  mfgr ", tag.dbSig);
	
	// Look up the name of the manufacturer in the table.
	for (i = 0; i < _COUNTOF(mfg_list); i++)
	{
		if (mfg_list[i].id == mfgr)
		{
			printk ("%s ", mfg_list[i].name);
			found = true;
			break;
		}
	}
	if (!found)
		printk ("ID %5d  ", mfgr);

	printk ("data: ");
	dump_hex (tag.data, tag.data_len);
}

/*
 * main - Program entry point.
 */
void main(void)
{
	int i;
	printk("Ardesco Bluetooth Scan sample started\n");

	// Initialize the bluetooth library.
	ble_init();

	// Wait a bit for first scans to return.
	k_sleep(K_SECONDS(2));

	// Print out becon info every 10 seconds.
	while (true) {
		// Get the list of tags.
		GetTagList (tags, &tag_cnt, MAX_DEVS);

		// Print out the info.
		for (i = 0; i < tag_cnt; i++)
		{
			dump_bt_tag(tags[i]);
		}

		// Sleep for 10 seconds.
		printk ("Sleeping...\n");
		k_sleep(K_SECONDS(10));
	}
}
