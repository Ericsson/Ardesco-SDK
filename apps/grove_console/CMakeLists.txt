#
# Copyright (c) Ericsson AB 2020, all rights reserved
#

cmake_minimum_required(VERSION 3.8.2)

# Include Ardesco SDK 
set(AR $ENV{ARDESCO_ROOT})
if(DEFINED AR)
   include($ENV{ARDESCO_ROOT}/ardesco.cmake)
else()
   # Assume we are building from the apps folder in Ardesco tree
   include(${CMAKE_CURRENT_SOURCE_DIR}/../../ardesco.cmake)
endif()

project(grove_console)
zephyr_compile_definitions(PROJECT_NAME=${PROJECT_NAME})

if(APP_VERSION)
  zephyr_compile_definitions(APP_VERSION=${APP_VERSION})
endif()

# NORDIC SDK APP START
target_sources(app PRIVATE src/main.c)
# NORDIC SDK APP END

# Add lib LED
add_subdirectory(${ARDESCO_LIB_DIR}/led_lib ${CMAKE_BINARY_DIR}/lib/led_lib)

