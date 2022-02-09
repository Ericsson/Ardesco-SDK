.. LTE_Connect:

LTE_Connect application
###############

The LTE_Connect application demonstrates a simple connection to the cellular 
network. The application calls the Nordic LTE library to initiate a connection
with callbacks. As each callback is received, the status is printed console (UART0).
The LED will start White then change Yellow while waiting for a connection. If a
connection is made, the LED turns Green. If the connection attempt times out, the
LED turns Red.


Requirements
************

* One of the following development boards:

  * |Ardesco Protptype|
  * |Ardesco Combi|
  * |Ardesco Combi Dev|
  * |Ardesco Mini|
  * |Thingy91|


