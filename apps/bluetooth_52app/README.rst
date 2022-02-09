.. bluetooth_52app:

Zephyr HCI bluetooth example ported to Ardesco
#############################

Ths sample is ported from the Nordic HCI low power UART sample which is based on the zephyr bluetooth-hci-uart-sample`. The nordic example uses the :ref:`uart_nrf_sw_lpuart` for HCI UART communication.

The Ardesco porting involves merging the serial ISR pointers using the serial_52lib to conform to the 52840 requirement that all serial drivers share the same ISR base address.

In addition, this sample merges the usb_uart library to relay COM0 to a virtual usb serial port to relay the 9160 debug and console streams.


Requirements
************

Overview
********

The sample implements the Bluetooth HCI controller using the :ref:`uart_nrf_sw_lpuart` for UART communication.

Building and running
********************

.. |sample path| replace:: :file:`samples/bluetooth/hci_lpuart`

.. include:: /includes/build_and_run_nrf9160.txt

Dependencies
************

This sample uses the following |NCS| driver:

* :ref:`uart_nrf_sw_lpuart`
