# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0


# 输出用于调试的信息，例如项目路径和 CMake 版本
message(STATUS "Tets log Current Source 11111 Directory: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Tets log CMake Version: ${CMAKE_VERSION}")

message(STATUS "ZEPHYR_BASE is set to: ${ZEPHYR_BASE}")

message(STATUS "CONFIG_REALTEK_DISPLAY is set to: ${CONFIG_REALTEK_DISPLAY}")

if(ZEPHYR_BASE)
  add_subdirectory_ifdef(CONFIG_REALTEK_DISPLAY driver)
  #add_subdirectory_ifdef(CONFIG_REALTEK_DISPLAY zephyr-driver)
else()
  add_subdirectory_ifdef(CONFIG_REALTEK_DISPLAY driver)
endif()


