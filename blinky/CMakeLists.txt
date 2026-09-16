# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(blinky)

target_sources(app PRIVATE
	src/main.c
	src/bme280_direct.c
	src/bme280_temperature.c
)

target_sources_ifdef(CONFIG_SUM_PRINT app PRIVATE sum_printk/sum_printk.c)
target_sources_ifdef(CONFIG_SUM_LOG app PRIVATE sum_log/sum_log.c)

target_include_directories(app PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/sum_printk
	${CMAKE_CURRENT_SOURCE_DIR}/sum_log
)
