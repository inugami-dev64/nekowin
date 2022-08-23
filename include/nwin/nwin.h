/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: nwin.h - abstracted surface window header file
/// author: Karl-Mihkel Ott

#ifndef __NWIN_H
#define __NWIN_H

#ifdef __cplusplus
extern "C" {
#endif


#if defined(X11_WINDOW_C) || defined(WIN32_WINDOW_C)
    #include "nwin/neko_except.h"
#endif

// std includes
#include <stdbool.h>
#include <stdint.h>
#include "nwin/nekodll.h"
#include "nwin/input.h"
#include "nwin/icon.h"
#include "nwin/gamepad.h"

/// Platform specific includes, structures and definitions
#if defined(_WIN32)
    #include <windows.h>
    #include <windowsx.h>
    #include "nwin/glad/glad.h"
#elif defined(__linux__)
    #include <X11/X.h>
    #include <X11/Xatom.h>
    #include <X11/XKBlib.h>
    #include "nwin/glad/glad.h"
    #include <GL/glx.h>
#endif

#include "nwin/neko_window.h"
#include "nwin/nekogl.h"

#ifdef WIN32_WINDOW_C
    #include "nwin/win32_translation.h"

    static struct {
        bool is_init;
        ATOM main_class;
        HINSTANCE instance;
        neko_Cursors cursors;
    } _neko_API = { 0 };
#endif


#define __NEKO_CLASS_NAME           "NWIN"
#define __NEKO_WGL_PF_ATTRIB_C      40

#ifdef X11_WINDOW_C
    #include <string.h>
    #include <unistd.h>
    #include "nwin/x11_translation.h"
    #include "nwin/xkb_unicode.h"
    #include "x11_api.h"
    #include <X11/cursorfont.h>

    // NET_WM message type definitions
    #define _NET_WM_STATE_REMOVE    0
    #define _NET_WM_STATE_ADD       1

    /// Structure for containing all API specific information 
    static struct {
        bool is_init;
        Display *display;
        neko_AtomsX11 atoms;
        Window root;
        int32_t scr;
        neko_Cursors cursors;
        XVisualInfo visual;
    } _neko_API = { 0 };
#endif

#define UC_BUFFER_SIZE 32

#include "nwin/neko_window.h"


#if defined(X11_WINDOW_C)
    /// Inner function declarations for X11 backend
    static void _neko_RegisterRaw(neko_Window *_win, int type, XKeyEvent *_kev);
    static void _neko_HandleKeyEvents(neko_Window *_win, int _type, XKeyEvent *_kev); 
    static void _neko_HandleMouseEvents(neko_Window *_win, int _type, XButtonEvent *_bev); 
    static void _neko_HandleMouseMovement(neko_Window *_win, int64_t _x, int64_t _y);
    static void _neko_SendClientMessage(neko_Window *_win, Atom _msg_type, long *_data);
    static void _neko_UpdateWindowSize(neko_Window *_win);
    static void _neko_LoadCursors();
#elif defined(WIN32_WINDOW_C)
    // Win32 implementation specific functions
    static LRESULT CALLBACK _neko_Win32MessageHandler(HWND _hwnd, UINT _msg, WPARAM _param, LPARAM _lparam);
    static HICON _neko_CreateIcon(const neko_Icon* _icon);
    static DWORD _neko_HandleSizeHints(neko_Window *_win);
    static void _neko_HandleMouseMovement(neko_Window* _win, POINT _pt);
#endif


/**************************/
/****** API bindings ******/
/**************************/

/// Initialise platform dependent backend api for nekowin library
LIBNWIN_API void neko_InitAPI();


/// Get the initialisation status of the API
LIBNWIN_API bool neko_APIInitStatus();


/// Deinitialise platform dependent backend api for nekowin library
LIBNWIN_API void neko_DeinitAPI();


/** 
 * Create new platform independant neko_Window instance for Vulkan or OpenGL
 * @param _width specifies the initial window width to use
 * @param _height specifies the initial window height to use
 * @param _hints specify window hints to use
 * @param _parent specifies a valid pointer to neko_Window instance that is as a parent for window to be created
 * @param _spawn_x specifies the initial window spawn location (X coordinate)
 * @param _spawn_y specifies the initial window spawn location (Y coordinate)
 * @param _title specifies the title to use for new window
 */
LIBNWIN_API neko_Window neko_NewWindow(
    int32_t _width, 
    int32_t _height,
    neko_SizeHint _size_hints,
    int32_t _spawn_x,
    int32_t _spawn_y,
    const char *_title
);


/// Explicitly set window icons
LIBNWIN_API void neko_SetIcons(neko_Window *_win, uint32_t _count, neko_Icon *_icons);


/// Update window events and key arrays
/// This function is meant to be called in every frame
LIBNWIN_API void neko_UpdateWindow(neko_Window *_win);


/// Set new resettable hints for neko window
LIBNWIN_API void neko_UpdateSizeMode(neko_Window *_win, neko_SizeHint _hints);


/// Make specified neko window the current rendering context
// LIBNWIN_API void neko_glMakeCurrent(neko_Window *_win);


/// Destroy window instance and free all resources that were used
LIBNWIN_API void neko_DestroyWindow(neko_Window *_win);


/****************************************/
/****** Input device communication ******/
/****************************************/

/// Change mouse cursor mode within neko window
LIBNWIN_API void neko_SetMouseCursorMode(neko_Window *_win, neko_CursorMode _cur_mode);


/// Force mouse cursor to certain location on window
LIBNWIN_API void neko_SetMouseCoords(neko_Window *_win, uint64_t _x, uint64_t _y);


/// Acquire all required Vulkan extension strings
LIBNWIN_API char **neko_FindRequiredVkExtensionStrings(uint32_t *_p_ext_s);

// #include "../include/limit_vc.h"

#ifdef __cplusplus
}
#endif

#endif
