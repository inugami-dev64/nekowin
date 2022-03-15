# nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
# licence: Apache, see LICENCE.md
# file: CMakeLists.txt - main cmake configuration file
# author: Karl-Mihkel Ott

cmake_minimum_required(VERSION 3.10)
project(nwin LANGUAGES C CXX)

# Default values and options
set(CXX_STANDARD C++14)
set(C_STANDARD C99)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(VULKAN_SDK_PATH CACHE STRING "")
option(BUILD_GLAPP "Build a sample application using nekowin and OpenGL" ON)
option(BUILD_STATIC_LIBNWIN "Build nekowin as a static library" ON)
option(BUILD_SHARED_LIBNWIN "Build nekowin as a shared library" ON)

include(cmake/libnwin.cmake)

# Set appropriate compiler flags
if(MSVC)
    add_compile_options(/W4 -D_CRT_SECURE_NO_WARNINGS /std:c++14)

    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(/0d)
    else()
        add_compile_options(/02)
    endif()
else()
    add_compile_options(-Wall -Wextra)

    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(-O0)
    else()
        add_compile_options(-O3)
    endif()
endif()

if(BUILD_GLAPP)
    include(cmake/glapp.cmake)
endif()
