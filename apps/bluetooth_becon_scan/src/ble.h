/*
 * Copyright (c) Ericsson AB 2021, all rights reserved
 */

#ifndef _BLE_H_
#define _BLE_H_

#include <bluetooth/gatt.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt_dm.h>
#include <bluetooth/scan.h>

#define MAX_DEVS  100
#define MAX_DATA  48

struct tagdata {
  bt_addr_le_t addr;
  int dbSig;
  unsigned char data[MAX_DATA];
  int data_len;
  time_t LastRcv;
};

void ble_init(void);

void BtAddrToString (bt_addr_le_t *addr, char *pstr, int nstrlen);

void dump_hex (const uint8_t *dat, int len);
void prt_bt_addr (bt_addr_le_t addr);

int GetTagList (struct tagdata *tl, int *tag_cnt, int max);
#endif /* _BLE_H_ */
