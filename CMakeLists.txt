cmake_minimum_required(VERSION 3.10)

project(OAS_Gateway CXX)

# =====================================
# C++ Standard
# =====================================

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =====================================
# Compiler Flags
# =====================================

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")

# =====================================
# Find Packages
# =====================================

find_package(PkgConfig REQUIRED)

pkg_check_modules(MOSQUITTO REQUIRED libmosquitto)

# =====================================
# Include Directories
# =====================================

include_directories(

    ${MOSQUITTO_INCLUDE_DIRS}

    ${CMAKE_SOURCE_DIR}

    ${CMAKE_SOURCE_DIR}/manager

    ${CMAKE_SOURCE_DIR}/tcp

    ${CMAKE_SOURCE_DIR}/mqtt
)

# =====================================
# Source Files
# =====================================

set(SOURCES

    main.cpp

    Manager/Manager.cpp
    Manager/OASManager.cpp

    TCP/TCPClient.cpp

    mqtt/MQTTClient.cpp
)

# =====================================
# Executable
# =====================================

add_executable(oas_gateway
    ${SOURCES}
)

# =====================================
# Link Libraries
# =====================================

target_link_libraries(oas_gateway

    ${MOSQUITTO_LIBRARIES}

    pthread
)

# =====================================
# Copy Config Folder To Build
# =====================================

file(COPY
    ${CMAKE_SOURCE_DIR}/Config
    DESTINATION
    ${CMAKE_BINARY_DIR}
)

# =====================================
# Install Binary
# =====================================

install(TARGETS oas_gateway
    RUNTIME DESTINATION /app
)

# =====================================
# Install Config
# =====================================

install(DIRECTORY
    ${CMAKE_SOURCE_DIR}/Config
    DESTINATION /app
)