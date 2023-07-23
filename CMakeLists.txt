# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(button)

target_sources(app PRIVATE src/main.c src/drv_button.c src/drv_led.c src/drv_uart.c)
#target_sources_ifdef(CONFIG_ADDFUNCTION app PRIVATE src/addfunction.c)