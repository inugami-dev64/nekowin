/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: window.h - abstaction header for platform specific headers and 
///                  generic window structure definition 
/// author: Karl-Mihkel Ott


#ifndef __WINDOW_H
#define __WINDOW_H


#if defined(__linux__)
    #include <x11_window.h>
#elif defined(_WIN32)
    #include <win32_window.h>
#endif

// This macro can be redefined if for some reason more windows are needed
#define _MAX_WSLOT_C    16


/*
 * Virtual cursor (VC) position in nekowin means that mouse cursor is stuck to certain position
 * and is only allowed to move within one frame cycle.
 * That also means that the recorded mouse position is not corresponding to the real
 * position of mouse cursor.
 */
typedef struct neko_VCData {
   bool is_enabled;
    #ifdef __linux__
        char * cursor;
        bool is_lib_cur;
    #endif
    // X and Y virtual position are in 64bit floating point integer since 
    // Arithmetic operation with these types are needed in camera classes
    uint64_t x;
    uint64_t y;
    uint64_t orig_x;
    uint64_t orig_y;
} neko_VCData;


/// Main structure for storing information about surface window and its parameters.
typedef struct _neko_Window {
    int32_t cwidth;
    int32_t cheight;
    int32_t cposx;
    int32_t cposy;
    int32_t swidth;
    int32_t sheight;
    const char * window_title;
    uint64_t mx;
    uint64_t my;
    neko_Hint hints;
    neko_VCData vc_data;
    neko_CursorMode cursor_mode;
    #if defined(_WIN32)
        neko_SurfaceWIN32 win32;
    #elif defined(__linux__)
        neko_SurfaceX11 x11;
    #endif
} _neko_Window;


#endif
