/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: nwim.c - nwin library C bindings source file
/// author: Karl-Mihkel Ott

#define __NWIN_C
#include <nwin.h>

void neko_ToggleVCMode(neko_Window *win) {
    win->vc_data.is_enabled = !win->vc_data.is_enabled;
}


void neko_Update(neko_Window *win) {
    __prev_x = win->vc_data.x;
    __prev_y = win->vc_data.y;

    if(win->hints & NEKO_HINT_API_OPENGL)
        glViewport(0, 0, win->cwidth, win->cheight);

    neko_UpdateWindow(win);
}


void neko_GetPixelSize(neko_Window *win, float *x, float *y) {
    *x = 2.0f / (float) win->cwidth;
    *y = 2.0f / (float) win->cheight;
}


XY neko_FindDeltaMovement(neko_Window *win) {
    XY rect = { 0 };
    if(win->vc_data.is_enabled) {
        rect.x = win->vc_data.x - __prev_x;
        rect.y = win->vc_data.y - __prev_y;
    } else {
        rect.x = win->mx - __prev_x;
        rect.y = win->my - __prev_y;
    }

    return rect;
}


void neko_DefaultVulkanExtensions(char ***p_exts, size_t *ext_s, bool is_validation_layer) {
    if(is_validation_layer) {
        *ext_s = 3;
        *p_exts = (char**) malloc((*ext_s) * sizeof(char*));
        (*p_exts)[2] = calloc(32, sizeof(char));
        strcpy((*p_exts)[2], NEKO_VK_DEBUG_UTILS_EXT_NAME);
    }

    else {
        *ext_s = 2;
        (*p_exts) = (char**) malloc((*ext_s) * sizeof(char*));
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
