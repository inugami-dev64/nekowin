/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE
/// file: napi.h - source file for platform independant api calls
/// author: Karl-Mihkel Ott


#define __NAPI_C
#include <napi.h>


void neko_ToggleVCMode(neko_Window win) {
    wslots[win].vc_data.is_enabled = !wslots[win].vc_data.is_enabled;
}


bool neko_IsVCMode(neko_Window win) {
    return wslots[win].vc_data.is_enabled;
}


void neko_GetWindowSize(neko_Window win, int32_t *x, int32_t *y) {
    *x = wslots[win].cwidth;
    *y = wslots[win].cheight;
}


void neko_GetWindowHints(neko_Window win, neko_Hint *hints) {
    *hints = wslots[win].hints;
}


void neko_GetPixelSize(neko_Window win, float *x, float *y) {
    *x = 2.0f / (float) wslots[win].cwidth;
    *y = 2.0f / (float) wslots[win].cheight;
}


void neko_GetMousePos(neko_Window win, uint64_t *x, uint64_t *y) {
    if(wslots[win].vc_data.is_enabled) {
        *x = wslots[win].vc_data.x;
        *y = wslots[win].vc_data.y;
    } else {
        *x = wslots[win].mx;
        *y = wslots[win].my;
    }
}


void neko_FindDeltaMovement(neko_Window win, uint64_t *x, uint64_t *y) {
    if(wslots[win].vc_data.is_enabled) {
        *x = wslots[win].vc_data.x - __prev_x;
        *y = wslots[win].vc_data.y - __prev_y;
    } else {
        *x = wslots[win].mx - __prev_x;
        *y = wslots[win].my - __prev_y;
    }
}


/// Limit the largest and smallest virtual cursor position that can be achieved using 
/// virtual mouse positioning
void neko_LimitVirtualPos (
    uint64_t max_x,       
    uint64_t min_x,
    uint64_t max_y,
    uint64_t min_y
) {
    __max_vc_x = max_x;
    __min_vc_x = min_x;
    __max_vc_y = max_y;
    __min_vc_y = min_y;
}


/// Set virtual mouse position overflow actions that specify what
/// should happen if virtual mouse position limit has been reached
void neko_SetOverflowAction (
    neko_VCPOverflowAction x_overflow_act,
    neko_VCPOverflowAction y_overflow_act
) {
    __x_overflow_act = x_overflow_act;
    __y_overflow_act = y_overflow_act;
}
