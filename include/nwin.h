/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: nwin.h - abstracted surface window header file
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

    #ifdef __linux__
        #include <X11/keysym.h>
        #include <X11/XKBlib.h>
    #endif
#endif


/// Custom macros
#define neko_assert(val, msg) if(!val) fprintf(stderr, "Assertion failed in %s:%u\n%s\n", __FILE__, __LINE__, msg), abort()
#define pcast(T, src) ((T) ((uint64_t) (src)))

/// OpenGL version definition macros
#define OPENGL_SUPPORTED_MAJOR  4
#define OPENGL_SUPPORTED_MINOR  6

#define neko_LoadGL gladLoadGL

#include <vulkan/vulkan.h>
#include "../include/nekodll.h"
#include <glad/glad.h>
#include "../include/key_translation.h"
#include "../include/key_ev.h"

#ifdef __NWIN_C

    // Global static varible declarations
    // Specify maximum and minimum virtual cursor positions
    int64_t __min_vc_x = INT64_MIN;
    int64_t __max_vc_x = INT64_MAX;
    int64_t __min_vc_y = INT64_MIN;
    int64_t __max_vc_y = INT64_MAX;

    int64_t __prev_x = 0;
    int64_t __prev_y = 0;

    // VCP overflow action specifiers
    neko_VCPOverflowAction __x_overflow_act = NEKO_VCP_OVERFLOW_ACTION_BLOCK_POSITION;
    neko_VCPOverflowAction __y_overflow_act = NEKO_VCP_OVERFLOW_ACTION_BLOCK_POSITION;

    // Vulkan extension name macros
    #define NEKO_VK_WSI_EXT_NAME            "VK_KHR_surface"
    #define NEKO_VK_XLIB_SURFACE_EXT_NAME   "VK_KHR_xlib_surface"
    #define NEKO_VK_WIN32_SURFACE_EXT_NAME  "VK_KHR_win32_surface"
    #define NEKO_VK_DEBUG_UTILS_EXT_NAME    "VK_EXT_debug_utils"

    bool __api_init = false;
#endif



typedef uint32_t neko_Window;


/* 
 * Nekowin uses so called window slots, which in reality is just a fixed array for containing information about multiple windows.
 * The API end user is given a handle to created window instance, which is just a index to the window in the window slot
 */
#ifdef __NWIN_C
    #include <window.h>   

    _neko_Window wslots[__MAX_WSLOT_C] = { 0 };
    uint32_t wslot_reserved = 0;
#endif


/**************************/
/****** API bindings ******/
/**************************/

/// Initialise platform dependent backend api for nekowin library
LIBNWIN_API void neko_InitAPI();


/// Get the initialisation status of the API
LIBNWIN_API bool neko_APIInitStatus();


/// Deinitialise platform dependent backend api for nekowin library
LIBNWIN_API void neko_DeinitAPI();


/// Create new platform independant neko_Window instance for Vulkan or OpenGL
LIBNWIN_API neko_Window neko_NewWindow(int32_t width, int32_t height, neko_Hint hints, const char *title);


/// Initialise the given neko_Window instance for Vulkan surface 
LIBNWIN_API VkResult neko_InitVKSurface(neko_Window win, VkInstance i, VkSurfaceKHR *s);


/// Update window events and key arrays
/// This function is meant to be called in every frame
LIBNWIN_API void neko_UpdateWindow(neko_Window win);


/// Set new resettable hints for neko window
LIBNWIN_API void neko_UpdateSizeMode(neko_Window win, neko_Hint hints);


/// Destroy window instance and free all resources that were used
LIBNWIN_API void neko_DestroyWindow(neko_Window win);


/// Check if window is still running and no close events have happened
LIBNWIN_API bool neko_IsRunning(neko_Window win);


/****************************************/
/****** Input device communication ******/
/****************************************/

/// Change mouse cursor mode within neko window
LIBNWIN_API void neko_SetMouseCursorMode(neko_Window win, neko_CursorMode cur_mode);


/// Force mouse cursor to certain location on window
LIBNWIN_API void neko_SetMouseCoords(neko_Window win, uint64_t x, uint64_t y);


/// Synchronise mouse position in neko_Window
LIBNWIN_API void neko_UpdateMousePos(neko_Window win);


/// Acquire all required Vulkan extension strings
LIBNWIN_API void neko_FindRequiredVkExtensionsStrings(char ***p_exts, size_t *p_ext_s);

#include "../include/napi.h"

#ifdef __cplusplus
}
#endif

#endif
