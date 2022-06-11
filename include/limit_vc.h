/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE
/// file: napi.h - header file for platform independant api calls
/// author: Karl-Mihkel Ott


#ifndef __LIMIT_VC_H
#define __LIMIT_VC_H


#ifdef __LIMIT_VC_C
    #include <stdint.h>
    #include <stdbool.h>
    #include <signal.h>
    #include <nekodll.h>
    #include <key_translation.h>
    #include <nwin.h>


    extern int64_t __min_vc_x;
    extern int64_t __max_vc_x;
    extern int64_t __min_vc_y;
    extern int64_t __max_vc_y;

    extern int64_t __prev_x;
    extern int64_t __prev_y;

    extern neko_VCPOverflowAction __x_overflow_act;
    extern neko_VCPOverflowAction __y_overflow_act;
#endif


/**************************/
/****** API bindings ******/
/**************************/

/// Find delta movement of mouse
LIBNWIN_API void neko_FindDeltaMovement(neko_Window *_win, int64_t *_x, int64_t *_y);

/// Limit the largest and smallest virtual cursor position that can be achieved using 
/// virtual cursor positioning (VCP)
LIBNWIN_API void neko_LimitVirtualPos(int64_t max_x, int64_t min_x, int64_t max_y, int64_t min_y);


/// Set virtual mouse position overflow actions that specify what
/// should happen if virtual mouse position limit has been reached
LIBNWIN_API void neko_SetOverflowAction(neko_VCPOverflowAction x_overflow_act, neko_VCPOverflowAction y_overflow_act);

#endif
