# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.10)

# Define a variable for the library name
set(LIBRARY_NAME reaktek_for_zephyr_display_driver_library)

file(GLOB SOURCES "*.c" "*.cpp")
file(GLOB HEADERS "*.h" "*.hpp")

# Determine if we should create a library
if(SOURCES OR (HEADERS AND NOT SOURCES))
    if(TARGET ${LIBRARY_NAME})
        message(STATUS "Adding sources/headers to existing ${LIBRARY_NAME}.")
        # Only add sources if they exist
        if(SOURCES)
            target_sources(${LIBRARY_NAME} PRIVATE ${SOURCES})
        endif()
    else()
        if(SOURCES)
            message(STATUS "Creating ${LIBRARY_NAME} with found sources.")
            add_library(${LIBRARY_NAME} ${SOURCES})
        else()
            message(STATUS "Creating an interface library for ${LIBRARY_NAME} with found headers.")
            add_library(${LIBRARY_NAME} INTERFACE)
        endif()
    endif()

    target_include_directories(${LIBRARY_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
else()
    message(STATUS "No source or header files found in ${CMAKE_CURRENT_SOURCE_DIR}, skipping library creation.")
endif()

# Process subdirectories regardless
file(GLOB SUBDIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

foreach(subdir ${SUBDIRS})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${subdir})
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/CMakeLists.txt)
            add_subdirectory(${subdir})
        else()
            message(STATUS "Skipping directory ${subdir} as it does not contain a CMakeLists.txt file.")
        endif()
    endif()
endforeach()
