# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.10)

file(GLOB SOURCES "*.c" "*.cpp")

target_sources(reaktek_display_driver_library PRIVATE ${SOURCES})

target_include_directories(reaktek_display_driver_library PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB SUBDIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

if(SOURCES)
    foreach(subdir ${SUBDIRS})
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${subdir})
            if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/CMakeLists.txt)
                add_subdirectory(${subdir})
            else()
                message(STATUS "Skipping directory ${subdir} as it does not contain a CMakeLists.txt file.")
            endif()
        endif()
    endforeach()
else()
    message(STATUS "No source files found in ${CMAKE_CURRENT_SOURCE_DIR}, skipping library creation.")
endif()






