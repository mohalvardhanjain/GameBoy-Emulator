cmake_minimum_required(VERSION 3.16)

project(gbemu LANGUAGES C CXX)

# -----------------------------------------------------------------------------
# Build configuration
# -----------------------------------------------------------------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default build type (if not specified)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# -----------------------------------------------------------------------------
# Custom CMake modules
# -----------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# -----------------------------------------------------------------------------
# System checks
# -----------------------------------------------------------------------------
include(CheckIncludeFiles)
include(CheckTypeSize)

# Check headers
check_include_files("stdlib.h" HAVE_STDLIB_H)

# Check types
check_type_size(intmax_t INTMAX_T)
check_type_size(uintmax_t UINTMAX_T)
check_type_size(pid_t PID_T)

# Fallback for pid_t
if(NOT HAVE_PID_T)
    if(WIN32)
        set(pid_t "int")
    else()
        message(FATAL_ERROR "pid_t doesn't exist on this platform?")
    endif()
endif()

# -----------------------------------------------------------------------------
# SDL2 setup
# -----------------------------------------------------------------------------
if(WIN32)

    # Windows manual setup
    set(SDL2_DIR "${PROJECT_SOURCE_DIR}/../windows_deps/sdl2")

    set(SDL2_INCLUDE_DIRS
        "${SDL2_DIR}/include"
        "${SDL2_DIR}/include/SDL2"
    )

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)

        set(SDL2_LIBRARIES
            "${SDL2_DIR}/lib/x64/SDL2.lib"
            "${SDL2_DIR}/lib/x64/SDL2main.lib"
        )

    else()

        set(SDL2_LIBRARIES
            "${SDL2_DIR}/lib/x86/SDL2.lib"
            "${SDL2_DIR}/lib/x86/SDL2main.lib"
        )

    endif()


    # SDL2_ttf
    set(SDL2_TTF_DIR "${PROJECT_SOURCE_DIR}/../windows_deps/sdl2_ttf")

    set(SDL2_TTF_INCLUDE_DIRS
        "${SDL2_TTF_DIR}/include"
    )

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)

        set(SDL2_TTF_LIBRARIES
            "${SDL2_TTF_DIR}/lib/x64/SDL2_ttf.lib"
        )

    else()

        set(SDL2_TTF_LIBRARIES
            "${SDL2_TTF_DIR}/lib/x86/SDL2_ttf.lib"
        )

    endif()

else()

    # Linux / Arch
    list(APPEND CMAKE_MODULE_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2"
    )

    find_package(SDL2 REQUIRED)
    find_package(SDL2_ttf REQUIRED)

endif()

# -----------------------------------------------------------------------------
# Generate config.h
# -----------------------------------------------------------------------------
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

# Make config.h available
add_compile_definitions(HAVE_CONFIG_H)

include_directories(
    "${CMAKE_CURRENT_BINARY_DIR}"
)

# -----------------------------------------------------------------------------
# Subdirectories
# -----------------------------------------------------------------------------
add_subdirectory(lib)
add_subdirectory(gbemu)
add_subdirectory(tests)

# -----------------------------------------------------------------------------
# Testing
# -----------------------------------------------------------------------------
enable_testing()

add_test(
    NAME check_gbe
    COMMAND check_gbe
)
