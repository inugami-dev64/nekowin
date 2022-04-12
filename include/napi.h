/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE
/// file: napi.h - header file for platform independant api calls
/// author: Karl-Mihkel Ott


#ifndef __NAPI_H
#define __NAPI_H


#ifdef __NAPI_C
    #include <stdint.h>
    #include <stdbool.h>
	#include <signal.h>
    #include <vulkan/vulkan.h>
    typedef uint32_t neko_Window;

    #include <nekodll.h>
    #include <key_translation.h>
    #include <window.h>


    extern int64_t __min_vc_x;
    extern int64_t __max_vc_x;
    extern int64_t __min_vc_y;
    extern int64_t __max_vc_y;

    extern int64_t __prev_x;
    extern int64_t __prev_y;

    extern neko_VCPOverflowAction __x_overflow_act;
    extern neko_VCPOverflowAction __y_overflow_act;

    extern _neko_Window wslots[__MAX_WSLOT_C];
    extern uint32_t wslot_reserved;
#endif


/**************************/
/****** API bindings ******/
/**************************/

/// Explicitly change virtual cursor mode
LIBNWIN_API void neko_ChangeVCMode(bool is_vcp, neko_Window win);


/// Toggle virtual cursor mode that locks the real cursor movement within the window instance
LIBNWIN_API void neko_ToggleVCMode(neko_Window win);


/// Check if virtual cursor mode is enabled or not
LIBNWIN_API bool neko_IsVCMode(neko_Window win);


/// Check if resizing event is occurring
LIBNWIN_API bool neko_ResizeNotify(neko_Window win);


/// Get window title
LIBNWIN_API const char *neko_GetTitle(neko_Window win);


/// Explicitly set mouse coordinates
LIBNWIN_API void neko_SetMouseCoords(neko_Window win, uint64_t x, uint64_t y);


/// Find the current window size
LIBNWIN_API void neko_GetWindowSize(neko_Window win, int32_t *x, int32_t *y);


/// Find hints for current window
LIBNWIN_API void neko_GetWindowHints(neko_Window win, neko_Hint *hints);


/// Find the pixel size in vector units from -1.0 to 1.0
LIBNWIN_API void neko_GetPixelSize(neko_Window win, float *x, float *y);


/// Get the current mouse position on the window
LIBNWIN_API void neko_GetMousePos(neko_Window win, int64_t *x, int64_t *y);


/// Find delta mouse movement between current and previous frames
LIBNWIN_API void neko_FindDeltaMovement(neko_Window win, int64_t *x, int64_t *y);


/// Limit the largest and smallest virtual cursor position that can be achieved using 
/// virtual cursor positioning (VCP)
LIBNWIN_API void neko_LimitVirtualPos(int64_t max_x, int64_t min_x, int64_t max_y, int64_t min_y);


/// Set virtual mouse position overflow actions that specify what
/// should happen if virtual mouse position limit has been reached
LIBNWIN_API void neko_SetOverflowAction(neko_VCPOverflowAction x_overflow_act, neko_VCPOverflowAction y_overflow_act);

#endif
