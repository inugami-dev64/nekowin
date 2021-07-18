/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: nwim.c - nwin library C bindings header file
/// author: Karl-Mihkel Ott


#ifndef __NWIN_H
#define __NWIN_H


#ifdef __NWIN_C
    #include <stdbool.h>
    #include <stdint.h>
    #include <vulkan/vulkan.h>
    static bool __is_vc = false;
    static uint64_t __prev_vc_x = 0;
    static uint64_t __prev_vc_y = 0;
#endif

#include <key_definitions.h>
#include <key_ev.h>
#include <surface_window.h>


/// Toggle virtual cursor mode that locks real 
void neko_ToggleVCMode();


/// Update window and input devices data
void neko_Update();


/// Find the pixel size in vector units from -1.0 to 1.0
void neko_GetPixelSize(float *x, float *y);


void neko_FindDeltaMovement(neko_Window *win);


#endif
