.. bluetooth_scanner:

Ardesco: BT Scanner Demo
###########################

.. contents::
   :local:
   :depth: 2

The Bluetooth Scanner sample demonstrates how to use the functionality of the 52840 bluetooth hardware to scan for BT LE beacons.

The sample uses UART1 configured with the Nordic low power UART driver to communicate with the 52840. 

Logging and console output is directed to UART0 which is typically relayed to the USB port via the 52840 application.


Overview
*********

The sample scans for BLE beacons and returns a list of the beacons found out the console every 10 seconds.

Requirements
************

* The sample requires the bluetooth_52app sample to be running on on 52840.

* It also requires bluetooth beacons to be in range. In a typical office, this is not a problem.


Dependencies
************

This sample uses the following libraries:

From |NCS|
  * :ref:`uart_nrf_sw_lpuart`

From Zephyr
  * :ref:`zephyr:bluetooth_api`

