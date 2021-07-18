/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: surface_window.h - abstracted surface window header file
/// author: Karl-Mihkel Ott

#ifndef __WIN32_SURFACE_H
#define __WIN32_SURFACE_H

#define NOMINMAX
#include <windows.h>
#include <synchapi.h>    
#include <stdio.h>

typedef struct _neko_SurfaceWIN32 {
    WNDCLASS win;
    HWND hwnd;
    MSG message;
    RAWINPUTDEVICE rids[2];
    RAWINPUT raw_input;
    UINT raw_input_size;
} neko_SurfaceWIN32;

#define NEKO_WIN32_CLASS_NAME L"NEKO_WINDOW"

/// WIN32 message callback function 
static LRESULT CALLBACK __deng_Win32MessageHandler(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam);

#endif
