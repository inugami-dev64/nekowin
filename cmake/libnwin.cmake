# nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
# licence: Apache, see LICENCE.md
# file: libnwin.cmake - libnwin cmake configuration file
# author: Karl-Mihkel Ott

set(LIBNWIN_STATIC_TARGET nwin-static)
set(LIBNWIN_SHARED_TARGET nwin-shared)

set(LIBNWIN_HEADERS
    include/key_ev.h
    include/key_translation.h
    include/napi.h
    include/nekodll.h
    include/nwin.h
    include/third_party/glad/glad.h
    include/window.h
)

set(LIBNWIN_SOURCES
    src/glad.c
    src/key_ev.c
    src/key_translation.c
    src/napi.c
)

# Add platform specific sources
if(WIN32)
    list(APPEND LIBNWIN_HEADERS include/win32_window.h)
    list(APPEND LIBNWIN_SOURCES src/win32_window.c)
elseif(UNIX AND NOT APPLE)
    list(APPEND LIBNWIN_HEADERS include/x11_window.h)
    list(APPEND LIBNWIN_SOURCES src/x11_window.c)
elseif(APPLE)
    message(FATAL_ERROR "Nekowin does not support MacOS")
endif()

# Check if debug mode is used
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(_DEBUG)
endif()

# Build static library
if(BUILD_STATIC_LIBNWIN)
    add_library(${LIBNWIN_STATIC_TARGET}
        ${LIBNWIN_HEADERS}
        ${LIBNWIN_SOURCES}
    )

    target_include_directories(${LIBNWIN_STATIC_TARGET}
        PRIVATE include
        PUBLIC include/third_party
    )

    target_compile_definitions(${LIBNWIN_STATIC_TARGET}
        PRIVATE LIBNWIN_EXPORT_LIBRARY
        PRIVATE LIBNWIN_STATIC
    )

    # OS dependent dependencies
    if(UNIX)
        target_link_libraries(${LIBNWIN_STATIC_TARGET}
            PUBLIC X11
            PUBLIC XCursor
            PUBLIC GL
            PUBLIC dl
            PUBLIC vulkan
        )
        
        if(NOT VULKAN_SDK_PATH STREQUAL "")
            target_include_directories(${LIBNWIN_STATIC_TARGET} PUBLIC ${VULKAN_SDK_PATH}/x86_64/include)
            target_link_directories(${LIBNWIN_STATIC_TARGET} PUBLIC ${VULKAN_SDK_PATH}/x86_64/lib)
        endif()

    elseif(WIN32)
        target_link_libraries(${LIBNWIN_STATIC_TARGET}
            PUBLIC vulkan-1
            PUBLIC gdi32
            PUBLIC kernel32
        )
    endif()
endif()


# Build shared library
if(BUILD_SHARED_LIBNWIN)
    add_library(${LIBNWIN_SHARED_TARGET}
        ${LIBNWIN_HEADERS}
        ${LIBNWIN_SOURCES}
    )

    target_include_directories(${LIBNWIN_SHARED_TARGET}
        PRIVATE include
        PUBLIC include/third_party
    )

    target_compile_definitions(${LIBNWIN_SHARED_TARGET}
        PRIVATE LIBNWIN_EXPORT_LIBRARY
        PRIVATE LIBNWIN_STATIC
    )

    # OS dependent dependencies
    if(UNIX)
        target_link_libraries(${LIBNWIN_SHARED_TARGET}
            PUBLIC X11
            PUBLIC Xcursor
            PUBLIC GL
            PUBLIC dl
            PUBLIC vulkan
        )
        
        if(NOT VULKAN_SDK_PATH STREQUAL "")
            target_include_directories(${LIBNWIN_SHARED_TARGET} PUBLIC ${VULKAN_SDK_PATH}/x86_64/include)
            target_link_directories(${LIBNWIN_SHARED_TARGET} PUBLIC ${VULKAN_SDK_PATH}/x86_64/lib)
        endif()

    elseif(WIN32)
        target_link_libraries(${LIBNWIN_SHARED_TARGET}
            PUBLIC vulkan-1
            PUBLIC gdi32
            PUBLIC kernel32
        )
    endif()
endif()
