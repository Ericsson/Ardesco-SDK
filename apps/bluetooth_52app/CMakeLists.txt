# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

#(db) commented out this line used to build in nordic SDK
#(port to Ardesco)find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

#(db) Include Ardesco SDK 
set(AR $ENV{ARDESCO_ROOT})
if(DEFINED AR)
   include($ENV{ARDESCO_ROOT}/ardesco.cmake)
else()
   # Assume we are building from the apps folder in Ardesco tree
   include(${CMAKE_CURRENT_SOURCE_DIR}/../../ardesco.cmake)
endif()

project(bluetooth_52app)

# NORDIC SDK APP START
target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/enable_gpio_reset.c)

# Add lib for serial on 52840
add_subdirectory(${ARDESCO_LIB_DIR}/serial_52lib ${CMAKE_BINARY_DIR}/lib/serial_52lib)
# Add lib for usb support on 52840
add_subdirectory(${ARDESCO_LIB_DIR}/usb_uart_52lib ${CMAKE_BINARY_DIR}/lib/usb_uart_52lib)

# NORDIC SDK APP END
