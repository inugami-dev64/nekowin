/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: surface_window.h - abstracted surface window header file
/// author: Karl-Mihkel Ott

#ifndef __NEKO_SURFACE_H
#define __NEKO_SURFACE_H

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __NEKO_SURFACE_C
    #include <stdlib.h>
    #include <stdbool.h>
    #include <stdint.h>
    #include <signal.h>
    #include <limits.h>
    #include <vulkan/vulkan.h>

    #ifdef __linux__
        #include <X11/keysym.h>
        #include <X11/XKBlib.h>
    #endif
#endif

#include <glad/glad.h>
#include <GL/glx.h>
#include <key_definitions.h>
#include <key_ev.h>

#ifdef __NEKO_SURFACE_C

    // Global static varible declarations
    // Specify maximum and minimum virtual cursor positions
    static uint64_t __min_vc_x = INT64_MIN;
    static uint64_t __max_vc_x = INT64_MAX;
    static uint64_t __min_vc_y = INT64_MIN;
    static uint64_t __max_vc_y = INT64_MAX;

    // VCP overflow action specifiers
    static neko_VCPOverflowAction __x_overflow_act = NEKO_VCP_OVERFLOW_ACTION_BLOCK_POSITION;
    static neko_VCPOverflowAction __y_overflow_act = NEKO_VCP_OVERFLOW_ACTION_BLOCK_POSITION;

    // VCP cursor movement speed
    static neko_MouseMode __mouse_mode = NEKO_MOUSE_MODE_CURSOR_VISIBLE;
    volatile sig_atomic_t __is_running;
#endif


/*
 * Virtual cursor (VC) position in DENG means that mouse cursor is stuck to certain position
 * and is only allowed to move within one frame cycle.
 * That also means that the recorded mouse position is not corresponding to the real
 * position of mouse cursor.
 */
typedef struct neko_VCData {
   bool is_enabled;
    #ifdef __linux__
        char *cursor;
        bool is_lib_cur;
    #endif
    // X and Y virtual position are in 64bit floating point integer since 
    // Arithmetic operation with these types are needed in camera classes
    uint64_t x;
    uint64_t y;
    uint64_t orig_x;
    uint64_t orig_y;
} neko_VCData;


#if defined(__linux__)
    #include <x11_surface.h>
#elif defined(_WIN32)
    #include <win32_surface.h>
#endif



/// Create new platform independant neko_Window instance for vulkan
/// This functions uses either Xlib or WIN32 api to create window depending on the operating system
neko_Window *neko_InitSurfaceWindow(int32_t width, int32_t height, neko_Hint hints, const char *title);


/// Update window events and key arrays
/// This function is meant to be called with every loop iteration 
void neko_UpdateWindow(neko_Window *p_win);


/// Destroy window instance and free all resources that were used
void neko_DestroyWindow(neko_Window *p_win);


/// Check if window is still running and no close events have happened
bool neko_IsRunning();

/****************************************/
/****** Input device communication ******/
/****************************************/

/// Switch mouse cursor behaviour within the DENG window 
void neko_SetMouseCursorMode (
    neko_Window* p_window,
    neko_MouseMode mouse_mode
);


/// Force mouse cursor to certain location on window
void neko_SetMouseCoords (
    neko_Window* p_window,
    uint64_t x,
    uint64_t y
);


/// Synchronise mouse position in neko_Window
void neko_GetMousePos(
    neko_Window* p_window,
    bool init_vc
);


/// Limit the largest and smallest virtual cursor position that can be achieved using 
/// virtual mouse positioning
void neko_LimitVirtualPos (
    uint64_t max_x,
    uint64_t min_x,
    uint64_t max_y,
    uint64_t min_y
);


/// Set virtual mouse position overflow actions that specify what
/// should happen if virtual mouse position limit has been reached
void neko_SetOverflowAction (
    neko_VCPOverflowAction x_overflow_act,
    neko_VCPOverflowAction y_overflow_act
);


#ifdef __cplusplus
}
#endif

#endif
