/*
 * Copyright (c) Ericsson AB 2021, all rights reserved
 */

#include <ardesco.h>
#include <bluetooth/bluetooth.h>
#include "ble.h"

#include <sys/byteorder.h>

struct bt_mfg_data_struct {
  unsigned char *pDat;
  int max;
  int len;
 };

// List of tags found.
int nTagCnt;
struct tagdata Tags[MAX_DEVS];

/*
 * prt_bt_addr - Trivial helper routine.
 */
void prt_bt_addr (bt_addr_le_t addr)
{
    int i;
    for (i = 0; i < sizeof (addr.a.val); i++)
    {
        printk ("%02x", addr.a.val[i]);
    }
}

/*
 * dump_hex - Trivial helper routine.
 */
void dump_hex (const uint8_t *dat, int len)
{
  int i;
  unsigned char c;

  for (i = 0; i < len; i++)
  {
    c = (unsigned char)dat[i];
    printk ("%02x ", c);
  }
  printk("\n");
}

 /*
  * adv_data_found - Callback from bt_data_parse function.
  * We're only looking for the mfg data callback, but others
  * left in for visibility.
  */
static bool adv_data_found(struct bt_data *data, void *user_data)
{
	struct bt_mfg_data_struct *mfgdat =
			(struct bt_mfg_data_struct *)user_data;
        int len;

	switch (data->type) {
	case BT_DATA_NAME_COMPLETE:
           //printk ("BT_DATA_NAME_COMPLETE\n");
		break;

	case BT_DATA_NAME_SHORTENED:
           //printk ("BT_DATA_NAME_SHORTENED\n");
		break;

	case BT_DATA_GAP_APPEARANCE:
           //printk ("BT_DATA_GAP_APPEARANCE\n");
		break;

	case BT_DATA_UUID16_SOME:
           //printk ("BT_DATA_UUID16_SOME\n");
		break;
	case BT_DATA_UUID16_ALL:
           //printk ("BT_DATA_UUID16_ALL\n");
		break;

	case BT_DATA_UUID32_SOME:
           //printk ("BT_DATA_UUID32_SOME\n");
		break;
	case BT_DATA_UUID32_ALL:
           //printk ("BT_DATA_UUID32_ALL\n");
		break;

	case BT_DATA_UUID128_SOME:
           //printk ("BT_DATA_UUID128_SOME\n");
		break;
	case BT_DATA_UUID128_ALL:
           //printk ("BT_DATA_UUID128_ALL\n");
		break;

	case BT_DATA_MANUFACTURER_DATA:
            //printk ("BT_DATA_MANUFACTURER_DATA: ");

            len = data->data_len;
            if (len > mfgdat->max)
              len = mfgdat->max;
            memcpy (mfgdat->pDat, ((struct bt_data *)data)->data, len);
            mfgdat->len = len;
		break;

	default:
		break;
	}

	return true;
}

/*
 * scan_cb. Called when the BT scanner recvs a beacon.
 */
static void scan_cb(const bt_addr_le_t *addr, int8_t rssi, 
                    uint8_t adv_type, struct net_buf_simple *buf)
{
    int i;
    bool fnd = false;
    unsigned char dat[128];
    struct bt_mfg_data_struct mfgdat = {&dat[0], sizeof (dat), 0};

    // Parse the payload data. Get Mfg bytes
    bt_data_parse(buf, adv_data_found, &mfgdat);

    // If no mfg data, forget it.
    if (mfgdat.len < 3)
        return;

    for (i = 0; i < nTagCnt; i++)
    {
        if (memcmp (&Tags[i].addr, (char *)addr, sizeof (bt_addr_le_t)) == 0)
        {
            fnd = true;
            break;
        }
    }
    // If found, update. If not, add. i == found index or first free index
    if (i < MAX_DEVS)
    {
        Tags[i].dbSig = rssi;
        //time (&Tags[i].LastRcv);
        Tags[i].LastRcv = k_uptime_get();
        memcpy (Tags[i].data, mfgdat.pDat, mfgdat.len);
        Tags[i].data_len = mfgdat.len;
        if (!fnd)
        {
            // Copy the bt addr. 
            memcpy (&Tags[i].addr, addr, sizeof (bt_addr_le_t));
            nTagCnt++;
        }
    }
}

/*
 * GetTagList - Retuns a copy of the current list of tags.
 */
int GetTagList (struct tagdata *tl, int *tag_cnt, int max)
{
    int i;
    *tag_cnt = 0;
    for (i = 0; (i < nTagCnt) && (i < max); i++)
    {
        tl[i] = Tags[i];
        (*tag_cnt)++;
    }
    return 0;
}

/*
 * ble_ready - Callback after bluetooth lib initialized.
 */
static void ble_ready (int err)
{
	printk("Bluetooth ready\n");

	//err = bt_le_scan_start(scan_param, scan_cb);
	err = bt_le_scan_start(BT_LE_SCAN_PASSIVE, scan_cb);
	if (err) {
		printk("Starting scanning failed (err %d)\n", err);
		return;
	}
}

/*
 * ble_init - Initialize the bluetooth library. When initialized,
 * the ble_ready callback starts the scan.
 */
void ble_init(void)
{
	int err;

	printk("Initializing Bluetooth..\n");
	err = bt_enable(ble_ready);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}
}
