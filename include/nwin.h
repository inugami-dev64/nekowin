/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: nwim.c - nwin library C bindings header file
/// author: Karl-Mihkel Ott


#ifndef __NWIN_H
#define __NWIN_H


#ifdef __NWIN_C
    #include <stdbool.h>
    #include <stdint.h>
    #include <stdlib.h>
    #include <string.h>
    #include <vulkan/vulkan.h>
    static bool __is_vc = false;
    static uint64_t __prev_x = 0;
    static uint64_t __prev_y = 0;

    // Vulkan extension name macros
    #define NEKO_VK_WSI_EXT_NAME            "VK_KHR_surface"
    #define NEKO_VK_XLIB_SURFACE_EXT_NAME   "VK_KHR_xlib_surface"
    #define NEKO_VK_WIN32_SURFACE_EXT_NAME  "VK_KHR_win32_surface"
    #define NEKO_VK_DEBUG_UTILS_EXT_NAME    "VK_debug_utils"
#endif

#include <key_definitions.h>
#include <key_ev.h>
#include <surface_window.h>

typedef struct Rectangle {
    uint64_t x;
    uint64_t y;
} Rectangle;


/// Toggle virtual cursor mode that locks real 
void neko_ToggleVCMode();


/// Update window and input devices data
void neko_Update();


/// Find the pixel size in vector units from -1.0 to 1.0
void neko_GetPixelSize(neko_Window *win, float *x, float *y);


Rectangle neko_FindDeltaMovement(neko_Window *win);


void neko_DefaultVulkanExtensions(char ***p_exts, size_t *ext_s, bool is_validation_layer);
#endif
