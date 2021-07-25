/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: x11_surface.h - x11 window / surface creation handler header file
/// author: Karl-Mihkel Ott
//
#ifndef __X11_SURFACE_H
#define __X11_SURFACE_H

#define EVENT_MASK KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | LeaveWindowMask | \
                    SubstructureNotifyMask | SubstructureRedirectMask

#define VALUE_MASK CWBorderPixel | CWColormap | CWEventMask


// Message type definitions
#define _NET_WM_STATE_REMOVE    0
#define _NET_WM_STATE_ADD       1
#define _NET_WM_STATE_TOGGLE    2

//#define NEKO_CURSOR_HIDDEN (char*) "xcursor/invisible"
//#define NEKO_CURSOR_DEFAULT (char*) "default"
//#define NEKO_CURSOR_ROTATE (char*) "plus"
#define _NEKO_XLIB_DEFAULT_CURSOR      "dnd_none"
#define _NEKO_DEFAULT_WINDOW_BORDER    5

// X11 includes 
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <GL/glx.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/extensions/Xrandr.h>
#include <vulkan/vulkan_xlib.h>

typedef struct _neko_X11Atoms {
    Atom WM_DELETE_WINDOW;
    Atom _NET_WM_STATE;
    Atom _NET_WM_STATE_FULLSCREEN;
} _neko_X11Atoms;


typedef struct _neko_SurfaceX11 {
    Cursor default_cursor;
    Window window;
    GC gc;
    XVisualInfo *p_vi;
    XVisualInfo vi;
} neko_SurfaceX11;


// Forward declaration for function types
typedef struct neko_Window;

#ifdef __NEKO_SURFACE_C 
    // Structure for containing all API specific information 
    struct {
        Display *display;
        XEvent fr_ev;
        _neko_X11Atoms atoms;
        Window root;
        int32_t scr;
    } _neko_API;
#endif


/// Function declarations
//static void _neko_XInitCursors(neko_Window *p_win);
//static void _neko_XFreeCursors(neko_Window *p_win);
//static void _neko_XSetCursor(neko_Window *p_win, bool hide);

/// Generic window event handlers
static void _neko_XHandleKeyEvents(); 
static void _neko_XHandleMouseEvents(); 
static void _neko_XHandleResize(neko_Window *p_win);

/// Window configuration 
static void _neko_SendClientMessage(neko_Window *p_win, Atom msg_type, long *data);
static void _neko_UpdateWindowSize(neko_Window *p_win);

/// OpenGL context creation
static void _neko_CreateGLContext(neko_Window *p_win);

/// Visual finder 
static void _neko_GetVisualInfo(neko_Window *p_win);

#endif
