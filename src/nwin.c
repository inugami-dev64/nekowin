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
    __prev_vc_x = win->vc_data.x;
    __prev_vc_y = win->vc_data.y;

    if(win->hints & NEKO_HINT_API_OPENGL)
        glViewport(0, 0, win->width, win->height);

    neko_UpdateWindow(win);
}
