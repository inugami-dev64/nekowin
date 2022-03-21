/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: x11_surface.h - x11 window / surface creation handler header file
/// author: Karl-Mihkel Ott

#ifndef __X11_SURFACE_H
#define __X11_SURFACE_H

#define EVENT_MASK KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | LeaveWindowMask | \
                   StructureNotifyMask | ClientMessage
                   

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
#include <X11/cursorfont.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/extensions/Xrandr.h>
#include <vulkan/vulkan_xlib.h>

/// Commonly used atoms used in API instance structure
typedef struct _neko_X11Atoms {
    Atom WM_DELETE_WINDOW;
    Atom _NET_WM_STATE;
    Atom _NET_WM_STATE_FULLSCREEN;
} _neko_X11Atoms;


typedef struct {
    Window window;
    GC gc;
    GLXContext glc;
    GLXDrawable drawable;
    XVisualInfo *p_vi;
    XVisualInfo vi;
} neko_NativeWindowX11;


#ifdef __NWIN_C 
    // GLX extension name macros
    #define GLX_SWAP_CONTROL_EXT_NAME       "GLX_EXT_swap_control"
    #define GLX_SWAP_CONTROL_SGI_NAME       "GLX_SGI_swap_control"
    #define GLX_SWAP_CONTROL_MESA_NAME      "GLX_MESA_swap_control"

    /// Structure for storing all cursor data
    typedef struct _neko_XCursors {
        Cursor standard;
        Cursor waiting;
        Cursor pointer;
        Cursor hidden;
    } _neko_XCursors;

    typedef void(*PFN_glXSwapIntervalEXT)(Display*, GLXDrawable, int); 
    typedef int(*PFN_glXSwapIntervalSGI)(int);
    typedef int(*PFN_glXSwapIntervalMESA)(unsigned int);

    /// Structure for containing all API specific information 
    struct {
        bool is_init;
        Display *display;
        _neko_X11Atoms atoms;
        Window root;
        int32_t scr;
        _neko_XCursors cursors;
        PFN_glXSwapIntervalEXT glXSwapIntervalEXT;
        PFN_glXSwapIntervalSGI glXSwapIntervalSGI;
        PFN_glXSwapIntervalMESA glXSwapIntervalMESA;
    } _neko_API = { 0 };

#endif


#ifdef __X11_WINDOW_C
    /// Inner function declarations 
    static void _neko_HandleKeyEvents(int type, XKeyEvent *kev); 
    static void _neko_HandleMouseEvents(int type, XButtonEvent *bev); 
    static void _neko_HandleMouseMovement(neko_Window win, int x, int y);
    static void _neko_GetVisualInfo(neko_Window win);
    static void _neko_SendClientMessage(neko_Window win, Atom msg_type, long *data);
    static void _neko_UpdateWindowSize(neko_Window win);
    static void _neko_CreateGLContext(neko_Window win);
    static void _neko_LoadCursors();
#endif

#endif
