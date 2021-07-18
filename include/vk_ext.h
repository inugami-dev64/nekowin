/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: vk_ext.h - Vulkan extension finder header file
/// author: Karl-Mihkel Ott

#ifndef __VK_EXT_H
#define __VK_EXT_H

#ifdef __cplusplus
extern "C" {
#endif

#define NEKO_VK_WSI_EXT_NAME "VK_KHR_surface"
#define NEKO_VK_XLIB_SURFACE_EXT_NAME "VK_KHR_xlib_surface"
#define NEKO_VK_WIN32_SURFACE_EXT_NAME "VK_KHR_win32_surface"
#define NEKO_VK_DEBUG_UTILS_EXT_NAME "VK_EXT_debug_utils"

#ifdef __VK_EXT_C
    #include <string.h> // strlen(), strcpy()
    #include <stdlib.h> // malloc(), calloc()
    #include <stdbool.h>
    #include <stdint.h>

	#include <vulkan/vulkan.h>
    #ifdef __linux__
        #include <X11/XKBlib.h> // KeySym
        #include <vulkan/vulkan_xlib.h>
    #endif

    #include <key_definitions.h>
    #include <surface_window.h>

    #ifdef _WIN32
        #include <vulkan/vulkan_win32.h>
    #endif
#endif 

void neko_GetRequiredVKSurfaceExt (
    neko_Window *p_win, 
    char ***p_exts,
    uint32_t *p_ext_c, 
    bool add_layer_ext
);


VkResult neko_InitVKSurface (
    neko_Window *p_window, 
    VkInstance *p_instance, 
    VkSurfaceKHR *p_surface
);

#ifdef __cplusplus
}
#endif

#endif
