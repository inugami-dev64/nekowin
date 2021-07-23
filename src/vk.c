/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: vk_ext.c - Vulkan extension finder source file
/// author: Karl-Mihkel Ott

#define __VK_C
#include <vk.h>

void neko_GetRequiredVKSurfaceExt(
    neko_Window *p_win,
    char ***p_exts,
    uint32_t *p_ext_c, 
    bool add_layer_ext
) {
    if(add_layer_ext) {
        *p_ext_c = 3;
        *p_exts = (char**) malloc((*p_ext_c) * sizeof(char*));
        (*p_exts)[2] = calloc(32, sizeof(char));
        strcpy((*p_exts)[2], NEKO_VK_DEBUG_UTILS_EXT_NAME);
    }

    else {
        *p_ext_c = 2;
        (*p_exts) = (char**) malloc((*p_ext_c) * sizeof(char*));
    }

    **p_exts = (char*) calloc(32, sizeof(char));
    strcpy(**p_exts, NEKO_VK_WSI_EXT_NAME);

    #if defined(__linux__)
        *(*p_exts + 1) = (char*) calloc(32, sizeof(char));
        strcpy(*(*p_exts + 1), NEKO_VK_XLIB_SURFACE_EXT_NAME);

    #elif defined(_WIN32)
        *(*p_exts + 1) = (char*) calloc(32, sizeof(char));
        strcpy(*(*p_exts + 1), NEKO_VK_WIN32_SURFACE_EXT_NAME);
    #endif
}


VkResult neko_InitVKSurface (
    neko_Window *p_win, 
    VkInstance instance,
    _neko_API,
    VkSurfaceKHR surface
) {
    #ifdef __linux__
        VkXlibSurfaceCreateInfoKHR surface_info;
        surface_info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
        surface_info.window = p_win->x11.window;
        surface_info.dpy = ;
        surface_info.flags = 0;
        surface_info.pNext = NULL;

        PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR;
        vkCreateXlibSurfaceKHR = (PFN_vkCreateXlibSurfaceKHR) vkGetInstanceProcAddr(instance, "vkCreateXlibSurfaceKHR");

        return vkCreateXlibSurfaceKHR(instance, &surface_info, NULL, &surface);
    #endif

    #ifdef _WIN32
        VkWin32SurfaceCreateInfoKHR surface_info;
        surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surface_info.hinstance = p_win->win32_handler.win.hInstance;
        surface_info.hwnd = p_win->win32_handler.hwnd;
        surface_info.pNext = NULL;
        
        PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
        vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR) vkGetInstanceProcAddr(*p_instance, "vkCreateWin32SurfaceKHR");

        return vkCreateWin32SurfaceKHR(*p_instance, &surface_info, NULL, p_surface);
    #endif

    return VK_ERROR_UNKNOWN;
} 
