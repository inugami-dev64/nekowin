# nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
# licence: Apache, see LICENCE.md
# file: CMakeLists.txt - main cmake configuration file
# author: Karl-Mihkel Ott

cmake_minimum_required(VERSION 3.10)
project(nwin LANGUAGES C CXX)

# Default values and options
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(VULKAN_SDK_PATH CACHE STRING "")
option(NEKOWIN_BUILD_GLAPP "Build a sample application using nekowin and OpenGL" ON)
option(NEKOWIN_BUILD_GAMEPAD_INFO "Build a sample console application for testing controller inputs" ON)
option(NEKOWIN_BUILD_STATIC_LIB "Build nekowin as a static library" ON)
option(NEKOWIN_BUILD_SHARED_LIB "Build nekowin as a shared library" ON)


# Set appropriate compiler flags
if(MSVC)
    add_compile_options(/W3)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(/Od)
    else()
        add_compile_options(/O2)
    endif()
else()
    add_compile_options(-Wall -Wextra $<$<COMPILE_LANGUAGE:CXX>:-std=c++14> $<$<COMPILE_LANGUAGE:C>:-std=c99>)

    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(-O0)
    else()
        add_compile_options(-O3)
    endif()
endif()

# Debug definition macro
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_definitions(-D__DEBUG)
endif()

include(cmake/libnwin.cmake)

if(NEKOWIN_BUILD_GAMEPAD_INFO)
    include(cmake/gpdinfo.cmake)
endif()

if(NEKOWIN_BUILD_GLAPP)
    include(cmake/glapp.cmake)
endif()
