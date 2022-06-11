/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE
/// file: napi.h - source file for platform independant api calls
/// author: Karl-Mihkel Ott


#define __LIMIT_VC_C
#include <limit_vc.h>


/*void neko_ChangeVCMode(bool is_vcp, neko_Window win) {*/
    /*wslots[win].vc_data.is_enabled = is_vcp;*/
    /*if(wslots[win].vc_data.is_enabled)*/
		/*neko_SetMouseCoords(win, wslots[win].vc_data.orig_x, wslots[win].vc_data.orig_y);*/
/*}*/


/*void neko_ToggleVCMode(neko_Window win) {*/
    /*wslots[win].vc_data.is_enabled = !wslots[win].vc_data.is_enabled;*/
    /*if(wslots[win].vc_data.is_enabled)*/
		/*neko_SetMouseCoords(win, wslots[win].vc_data.orig_x, wslots[win].vc_data.orig_y);*/
/*}*/


/*bool neko_IsVCMode(neko_Window win) {*/
    /*return wslots[win].vc_data.is_enabled;*/
/*}*/


/*bool neko_ResizeNotify(neko_Window win) {*/
    /*return wslots[win].resize_notify;*/
/*}*/


/*const char *neko_GetTitle(neko_Window win) {*/
    /*return wslots[win].window_title;*/
/*}*/


/*void neko_GetWindowSize(neko_Window win, int32_t *x, int32_t *y) {*/
    /**x = wslots[win].cwidth;*/
    /**y = wslots[win].cheight;*/
/*}*/


/*void neko_GetWindowHints(neko_Window win, neko_Hint *hints) {*/
    /**hints = wslots[win].hints;*/
/*}*/


/*void neko_GetPixelSize(neko_Window win, float *x, float *y) {*/
    /**x = 2.0f / (float) wslots[win].cwidth;*/
    /**y = 2.0f / (float) wslots[win].cheight;*/
/*}*/


/*void neko_GetMousePos(neko_Window win, int64_t *x, int64_t *y) {*/
    /*if(wslots[win].vc_data.is_enabled) {*/
        /**x = wslots[win].vc_data.x;*/
        /**y = wslots[win].vc_data.y;*/
    /*} else {*/
        /**x = wslots[win].mx;*/
        /**y = wslots[win].my;*/
    /*}*/
/*}*/


void neko_FindDeltaMovement(neko_Window *_win, int64_t *_x, int64_t *_y) {
    if(_win->vc_data.is_enabled) {
        *_x = _win->vc_data.x;
        *_y = _win->vc_data.y;
        _win->vc_data.x = 0;
        _win->vc_data.y = 0;
    } else {
        *_x = _win->mx - __prev_x;
        *_y = _win->my - __prev_y;
        __prev_x = _win->mx;
        __prev_y = _win->my;
    }
}


void neko_LimitVirtualPos (
    int64_t max_x,       
    int64_t min_x,
    int64_t max_y,
    int64_t min_y
) {
    __max_vc_x = max_x;
    __min_vc_x = min_x;
    __max_vc_y = max_y;
    __min_vc_y = min_y;
}


void neko_SetOverflowAction (
    neko_VCPOverflowAction x_overflow_act,
    neko_VCPOverflowAction y_overflow_act
) {
    __x_overflow_act = x_overflow_act;
    __y_overflow_act = y_overflow_act;
}
