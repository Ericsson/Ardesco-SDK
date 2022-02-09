Release Notes

Usage
	Unzip the ardesco_sdk .zip file into a directory on your hard drive.
	
	Download the Nordic Connect for Desktop tool from the Nordic website

	Use the Toolchain manager to download the Nordic SDK into the Ardesco
	directory.

	Modify the NCSDIR file in the Ardesco directory to select the proper
	NCS SDK verison. (Default is v1.8.0.)

	Open a command line build prompt using the ardcmdprompt.cmd command
	file in the ardesco directory.

	For detailed instructions, refer to "Getting Started with the Ardesco SDK"
	available on the Ardesco Teams page.

	Note: while the Ardesco libraries are written to be backward compatible,
	the examples in the 'apps' folder are written to the latest supported 
	Nordic Connect SDK.

Changelog

Release 1.9
	Added support for Nordic SDK 1.6.1, 1.7.0 and 1.8.0

	Added button_lib and buzzer_lib for additional UI support

	Updated usb_uart_52lib and sensor_lib related to v1.7.0 and v1.8.0 compatibilty.

	Modified Hello_52840 example to be compatible with v1.8.0

	Modified LTE_connect example given rename of BSD_LIB to NRF_MODEM_LIB

	Updated Bluetooth applications to be compatible with v1.8.0

	Set target NCS version to v1.8.0


Release 1.8
	Added support for Nordic SDK 1.5 and 1.5.1

	Added example LTE_connect to demonstrate connecting with LTE

	Added example bluetooth_beacon_scan for the 9160 and bluetooth_52app on the 52840 
	to demonstrate using 52840 bluetooth functionality from the 9160.

	Fixed bugs in serial_52 lib and usb_uart_52lib related to v1.4.0 compatibilty.


Release 1.7
	Fixed bug in board directories that would cause compile to fail if CONFIG_SPI=n

	Fixed bug in board directories that prevented antenna from being configured correctly.

	Fixed bug in board directories that prevented SPI and Grove select from being configured correctly.

	Fixed bug in LED library that caused build failures.

	Updated ADP536 driver for v1.4.0 compatibility.


Release 1.6
	Added board directories compatible to Nordic SDK 1.4. Set the target
	NCS version to v1.4.0, while remaining backward compatible with v1.3.

	Added sensor_lib library to provide simple API to read environment and
	accelerometer sensors.

	Added support on 52840 for LED and "boot" button in board directories 
	for Combi and Combi Dev.

	Added using_sensors example to demonstrate using the sensor library.

	Added grove_i2c example to demonstrate using I2C across Grove connector.

	Added grove_console example to demonstrate redirecting the console out the Grove connector

	Removed support for Ardesco Mini for v1.4.0

	Fixed bugs in Combi board directory that prevented successful compile.

Release 1.5
	Added missing file ard_led.h.

	Added precompiled .hex files for at_client and usb_uart_bridge samples.

	Added this readme file.

Release 1.4
	Added support for Nordic Connect SDK 1.3.x. 

	Updated build system to support multiple versions of the Nordic
	Connect SDK.

	Added Ard_LED library to simplify access to the 3-color LED.

	Added serial_52lib library to coordinate using the 52840 UARTs
	across mulitple libraries.

	Rewrote usb_uart_52lib (formerly usb_uart_lib) to support 
	serial_52lib.

	Added interprocessor communication library that uses a UART
	to communicate between the 9160 and 52840.

	Added 91ipc_simple and 52ipc_simple applications to demonstrate
	using the IPC library.

	Updated the Ardeso board directories to eliminate the predefined
	UART1 for both the 9160 and 52840 CPUs.

	Updated SDK to support both NCS 1.2.x and 1.3.x.

	Added ardcmdprompot.cmd file to open a console build window.

	Added Ardesco\Samples directory that contain the very useful 
	examples at_client (for 9160) and usb_uart_bridge (for 52840).