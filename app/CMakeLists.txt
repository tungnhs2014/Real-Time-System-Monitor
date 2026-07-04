# SPDX-License-Identifier: GPL-2.0-only
# Copyright (c) 2025-2026 TungNHS

cmake_minimum_required(VERSION 3.16)
project(system-monitor
    VERSION 1.0.0
    DESCRIPTION "System Monitor with ILI9341 Display"
    LANGUAGES CXX
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml)

if(NOT DEFINED LOG_LEVEL)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(LOG_LEVEL 5)
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        set(LOG_LEVEL 3)
    else()
        set(LOG_LEVEL 0)
    endif()
endif()

if(LOG_LEVEL LESS 0 OR LOG_LEVEL GREATER 5)
    message(FATAL_ERROR "LOG_LEVEL must be 0-5")
endif()

set(LOG_LEVEL_NAMES "OFF" "CRITICAL" "ERROR" "WARNING" "INFO" "DEBUG")
list(GET LOG_LEVEL_NAMES ${LOG_LEVEL} LOG_LEVEL_NAME)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(PLATFORM_RASPBERRY_PI TRUE)
    set(PLATFORM_NAME "Raspberry Pi")
else()
    set(PLATFORM_DESKTOP TRUE)
    set(PLATFORM_NAME "Desktop")
endif()

add_compile_definitions(
    $<$<BOOL:${PLATFORM_RASPBERRY_PI}>:PLATFORM_RASPBERRY_PI>
    $<$<BOOL:${PLATFORM_DESKTOP}>:PLATFORM_DESKTOP>
    LOG_LEVEL=${LOG_LEVEL}
    $<$<CONFIG:Release>:QT_NO_DEBUG>
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
    $<$<CONFIG:Release>:QT_NO_INFO_OUTPUT>
    $<$<CONFIG:Release>:QT_NO_WARNING_OUTPUT>
    APP_VERSION="${PROJECT_VERSION}"
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/controller
    ${CMAKE_CURRENT_SOURCE_DIR}/src/domain
    ${CMAKE_CURRENT_SOURCE_DIR}/src/model
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
)

set(COMMON_SOURCES
    src/common/FileReader.cpp
    src/common/Logger.cpp
)

set(COMMON_HEADERS
    src/common/FileReader.h
    src/common/Logger.h
    src/common/Constants.h
)

set(DOMAIN_HEADERS
    src/domain/SystemMetricsSnapshot.h
    src/domain/WarningThresholdPolicy.h
)

set(MODEL_SOURCES
    src/model/CpuMonitor.cpp
    src/model/GpuMonitor.cpp
    src/model/MemoryMonitor.cpp
    src/model/StorageMonitor.cpp
    src/model/NetworkMonitor.cpp
    src/model/SettingsManager.cpp
)

set(MODEL_HEADERS
    src/model/CpuMonitor.h
    src/model/GpuMonitor.h
    src/model/MemoryMonitor.h
    src/model/StorageMonitor.h
    src/model/NetworkMonitor.h
    src/model/SettingsManager.h
)

set(SERVICE_SOURCES
    src/services/SystemMetricsService.cpp
)

set(SERVICE_HEADERS
    src/services/SystemMetricsService.h
)

set(CONTROLLER_SOURCES
    src/controller/SystemController.cpp
)

set(CONTROLLER_HEADERS
    src/controller/SystemController.h
)

set(QML_RESOURCES
    resources/qml.qrc
)

add_executable(${PROJECT_NAME}
    src/main.cpp
    ${COMMON_SOURCES}
    ${COMMON_HEADERS}
    ${DOMAIN_HEADERS}
    ${MODEL_SOURCES}
    ${MODEL_HEADERS}
    ${SERVICE_SOURCES}
    ${SERVICE_HEADERS}
    ${CONTROLLER_SOURCES}
    ${CONTROLLER_HEADERS}
    ${QML_RESOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    $<$<CONFIG:Debug>:-O0>
    $<$<CONFIG:Debug>:-g3>
    $<$<CONFIG:Debug>:-DDEBUG>
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Release>:-DNDEBUG>
    $<$<CONFIG:Release>:-ffunction-sections>
    $<$<CONFIG:Release>:-fdata-sections>
    $<$<CONFIG:RelWithDebInfo>:-O2>
    $<$<CONFIG:RelWithDebInfo>:-g>
    $<$<CONFIG:MinSizeRel>:-Os>
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_options(${PROJECT_NAME} PRIVATE
        -Wl,--gc-sections
    )
endif()

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

message(STATUS "")
message(STATUS "System Monitor - Build Config")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Platform: ${PLATFORM_NAME}")
message(STATUS "Qt Version: ${Qt6_VERSION}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Log Level: ${LOG_LEVEL} (${LOG_LEVEL_NAME})")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Logging: disabled")
else()
    message(STATUS "Logging: level ${LOG_LEVEL}")
endif()
