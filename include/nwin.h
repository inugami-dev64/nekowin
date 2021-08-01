/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: surface_window.h - abstracted surface window header file
/// author: Karl-Mihkel Ott

#ifndef __NWIN_H
#define __NWIN_H

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __NWIN_C
    #include <stdlib.h>
    #include <stdbool.h>
    #include <stdint.h>
    #include <string.h>
    #include <signal.h>
    #include <limits.h>
    #include <vulkan/vulkan.h>

    #ifdef __linux__
        #include <X11/keysym.h>
        #include <X11/XKBlib.h>
    #endif
#endif


/// Custom macros
#define neko_assert(val, msg) (!val ? fprintf(stderr, "Assertion failed in %s:%u\n%s\n", __FILE__, __LINE__, msg), abort() : val);
#define pcast(T, src) ((T) ((uint64_t) (src)))

/// OpenGL version definition macros
#define OPENGL_SUPPORTED_MAJOR  4
#define OPENGL_SUPPORTED_MINOR  6

#include <glad/glad.h>
#include <key_translation.h>
#include <key_ev.h>

#ifdef __NWIN_C

    // Global static varible declarations
    // Specify maximum and minimum virtual cursor positions
    static uint64_t __min_vc_x = INT64_MIN;
    static uint64_t __max_vc_x = INT64_MAX;
    static uint64_t __min_vc_y = INT64_MIN;
    static uint64_t __max_vc_y = INT64_MAX;

    static uint64_t __prev_x = 0;
    static uint64_t __prev_y = 0;

    // VCP overflow action specifiers
    static neko_VCPOverflowAction __x_overflow_act = NEKO_VCP_OVERFLOW_ACTION_BLOCK_POSITION;
    static neko_VCPOverflowAction __y_overflow_act = NEKO_VCP_OVERFLOW_ACTION_BLOCK_POSITION;

    // VCP cursor movement speed
    volatile sig_atomic_t __is_running;

    // Vulkan extension name macros
    #define NEKO_VK_WSI_EXT_NAME            "VK_KHR_surface"
    #define NEKO_VK_XLIB_SURFACE_EXT_NAME   "VK_KHR_xlib_surface"
    #define NEKO_VK_WIN32_SURFACE_EXT_NAME  "VK_KHR_win32_surface"
    #define NEKO_VK_DEBUG_UTILS_EXT_NAME    "VK_debug_utils"
#endif


/// Enumeral values for nekowin cursors
typedef enum neko_CursorMode {
    NEKO_CURSOR_MODE_STANDARD,
    NEKO_CURSOR_MODE_WAITING,
    NEKO_CURSOR_MODE_POINTER,
    NEKO_CURSOR_MODE_HIDDEN
} neko_CursorMode;


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


#if defined(__linux__)
    #include <x11_surface.h>
#elif defined(_WIN32)
    #include <win32_surface.h>
#endif


/* 
 * Nekowin uses so called window slots, which in reality is just a fixed array for containing information about multiple windows.
 * The API end user is given a handle to created window instance, which is just a index to the window in the window slot
 */
#ifdef __NWIN_C
    // This macro can be redefined if for some reason more windows are needed
    #define _MAX_WSLOT_C    16

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

    _neko_Window wslots[_MAX_WSLOT_C] = { 0 };
    uint32_t wslot_reserved = 0;

#endif

typedef uint32_t neko_Window;


/**************************/
/****** API bindings ******/
/**************************/

/// Initialise platform dependent backend api for nekowin library
void neko_InitAPI();


/// Deinitialise platform dependent backend api for nekowin library
void neko_DeinitAPI();


/// Create new platform independant neko_Window instance for Vulkan or OpenGL
neko_Window neko_NewWindow(int32_t width, int32_t height, neko_Hint hints, const char *title);


/// Initialise the given neko_Window instance for Vulkan surface 
VkResult neko_InitVKSurface(neko_Window win, VkInstance i, VkSurfaceKHR s);


/// Update window events and key arrays
/// This function is meant to be called in every frame
void neko_UpdateWindow(neko_Window win);


/// Set new resettable hints for neko window
void neko_UpdateSizeMode(neko_Window win, neko_Hint hints);


/// Destroy window instance and free all resources that were used
void neko_DestroyWindow(neko_Window win);


/// Check if window is still running and no close events have happened
bool neko_IsRunning();


/****************************************/
/****** Input device communication ******/
/****************************************/

/// Switch mouse cursor behaviour within the DENG window 
void neko_SetMouseCursorMode(neko_Window win, neko_CursorMode cur_mode);


/// Force mouse cursor to certain location on window
void neko_SetMouseCoords(neko_Window win, uint64_t x, uint64_t y);


/// Synchronise mouse position in neko_Window
void neko_UpdateMousePos(neko_Window win, bool init_vc);


/// Toggle virtual cursor mode that locks the real cursor movement within the window instance
void neko_ToggleVCMode(neko_Window win);


/// Find the current window size
void neko_GetWindowSize(neko_Window win, int32_t *x, int32_t *y);


void neko_GetWindowHints(neko_Window win, neko_Hint *hints);


/// Find the pixel size in vector units from -1.0 to 1.0
void neko_GetPixelSize(neko_Window win, float *x, float *y);


/// Find delta mouse movement between current and previous frames
void neko_FindDeltaMovement(neko_Window win, uint64_t *x, uint64_t *y);


/// Acquire all required Vulkan extension strings
void neko_FindRequiredVkExtensionsStrings(char ***p_exts, size_t *p_ext_s, bool use_validation_layers);


/// Limit the largest and smallest virtual cursor position that can be achieved using 
/// virtual cursor positioning (VCP)
void neko_LimitVirtualPos(uint64_t max_x, uint64_t min_x, uint64_t max_y, uint64_t min_y);


/// Set virtual mouse position overflow actions that specify what
/// should happen if virtual mouse position limit has been reached
void neko_SetOverflowAction(neko_VCPOverflowAction x_overflow_act, neko_VCPOverflowAction y_overflow_act);


#ifdef __cplusplus
}
#endif

#endif
