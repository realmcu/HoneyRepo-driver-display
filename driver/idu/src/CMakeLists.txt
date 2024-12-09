# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.10)

file(GLOB SOURCES "*.c" "*.cpp")

target_sources(reaktek_display_driver_library PRIVATE ${SOURCES})

target_include_directories(reaktek_display_driver_library PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})






