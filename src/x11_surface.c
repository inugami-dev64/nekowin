/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: x11_surface.c - x11 window / surface creation handler source file
/// author: Karl-Mihkel Ott

#define __NEKO_SURFACE_C
#include <surface_window.h>


/// Initialise platform dependent backend api for nekowin library
void neko_InitAPI() {
    _neko_API.display = XOpenDisplay(NULL);
    _neko_API.root = DefaultRootWindow(_neko_API.display);
    _neko_API.scr = DefaultScreen(_neko_API.display);

    // Get basic atom values
    _neko_API.atoms.WM_DELETE_WINDOW = XInternAtom(_neko_API.display, "WM_DELETE_WINDOW", False);
    _neko_API.atoms._NET_WM_STATE = XInternAtom(_neko_API.display, "_NET_WM_STATE", False);
    _neko_API.atoms._NET_WM_STATE_FULLSCREEN = XInternAtom(_neko_API.display, "_NET_WM_STATE_FULLSCREEN", False);

    // Check if detectable keyboard autorepeat is supported
    Bool supported;
    neko_assert(XkbSetDetectableAutoRepeat(_neko_API.display, True, &supported), 
                "DetectableAutoRepeat is not supported");
}


void _neko_GetVisualInfo(neko_Window *win) {
    if(win->hints & NEKO_HINT_API_OPENGL) {
        GLint attrs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
        XVisualInfo *vi = glXChooseVisual(_neko_API.display, 0, attrs);

        neko_assert(vi, "Failed to choose GLX visual");
        win->x11.p_vi = vi;
    }

    else {

        win->x11.vi.visual = DefaultVisual(_neko_API.display, _neko_API.scr);
        win->x11.vi.depth = DefaultDepth(_neko_API.display, _neko_API.scr);
    }
}


neko_Window *neko_NewWindow (
    int32_t width, 
    int32_t height, 
    neko_Hint hints,
    const char *title
) {
    static neko_Window win;

    // Fill the window structure
    win.swidth = win.cwidth = width;
    win.sheight = win.cwidth = height;
    win.window_title = title;

    win.vc_data.is_enabled = 0;
    win.hints = hints;
    win.vc_data.orig_x = (float) width / 2.0f;
    win.vc_data.orig_y = (float) height / 2.0f;
    win.vc_data.x = 0;
    win.vc_data.y = 0;
 
    // Retrieve visual info about the window
    _neko_GetVisualInfo(&win);

    XSetWindowAttributes swa = { 0 };
    swa.event_mask = EVENT_MASK;
    
    // Create a new window
    if(win.x11.p_vi) {
        swa.colormap = XCreateColormap(_neko_API.display, _neko_API.root, win.x11.p_vi->visual, AllocNone);
        win.x11.window = XCreateWindow(_neko_API.display, _neko_API.root, 
                                       0, 0, win.swidth, win.sheight,
                                       0, win.x11.p_vi->depth, 
                                       InputOutput, win.x11.p_vi->visual, 
                                       VALUE_MASK, 
                                       &swa);
    } else {
        swa.colormap = XCreateColormap(_neko_API.display, _neko_API.root, win.x11.vi.visual, AllocNone);
        win.x11.window = XCreateWindow(_neko_API.display, _neko_API.root, 
                                       0, 0, win.swidth, win.sheight,
                                       0, InputOutput, 
                                       win.x11.vi.depth, win.x11.vi.visual, 
                                       VALUE_MASK, 
                                       &swa);
    }

    // Check if the window was created successfully
    neko_assert(win.x11.window, "Failed to create x11 window!");
    XMapWindow(_neko_API.display, win.x11.window);
    XStoreName(_neko_API.display, win.x11.window, win.window_title);

    _neko_UpdateWindowSize(&win);
    
    // Set the base WM protocols
    XSetWMProtocols(_neko_API.display, win.x11.window, &_neko_API.atoms.WM_DELETE_WINDOW, True);

    if(hints & NEKO_HINT_API_OPENGL)
        _neko_CreateGLContext(&win);

    __is_running = true;
    return &win;
}


VkResult neko_InitVKSurface (
    neko_Window *win, 
    VkInstance instance,
    VkSurfaceKHR surface
) {
    VkXlibSurfaceCreateInfoKHR surface_info;
    surface_info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    surface_info.window = win->x11.window;
    surface_info.dpy = _neko_API.display;
    surface_info.flags = 0;
    surface_info.pNext = NULL;

    PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR;
    vkCreateXlibSurfaceKHR = (PFN_vkCreateXlibSurfaceKHR) vkGetInstanceProcAddr(instance, "vkCreateXlibSurfaceKHR");

    return vkCreateXlibSurfaceKHR(instance, &surface_info, NULL, &surface);
} 


/// Force mouse cursor to certain location on window
void neko_SetMouseCoords (
    neko_Window *win, 
    uint64_t x, 
    uint64_t y
) {
    XWarpPointer(_neko_API.display, None, win->x11.window, 0, 0, 0, 0, x, y);
}


/// Synchronise mouse position in neko_Window
void neko_GetMousePos (
    neko_Window *win, 
    bool init_vc
) {
    // dummy variables for x11
    Window return_window;
    int win_x, win_y, x, y;
    unsigned int mask;
    XQueryPointer(_neko_API.display, win->x11.window, &return_window, &return_window, 
        &win_x, &win_y, &x, &y, &mask);

    if(win->vc_data.is_enabled && !init_vc) {
        double movement_x = (double) ((uint64_t) x - win->vc_data.orig_x);
        double movement_y = (double) ((uint64_t) y - win->vc_data.orig_y);

        if(x != win->vc_data.orig_x || y != win->vc_data.orig_y)
            neko_SetMouseCoords(win, win->vc_data.orig_x, win->vc_data.orig_y);

        // Check for overflow on x position
        if(win->vc_data.x + movement_x >= (double) __max_vc_x) {
            if(__x_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION) {
                win->vc_data.x = __min_vc_x;
            }
        }
        
        else if(win->vc_data.x + movement_x <= (double) __min_vc_x) {
            if(__x_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                win->vc_data.x = __max_vc_x;
        }

        else win->vc_data.x += movement_x;


        // Check for overflow on y position
        if(win->vc_data.y + movement_y >= (double) __max_vc_y) {
            if(__y_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                win->vc_data.y = __min_vc_y;
        }
        
        else if(win->vc_data.y + movement_y <= (double) __min_vc_y) {
            if(__y_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                win->vc_data.y = __max_vc_y;
        }

        else win->vc_data.y += movement_y;
    }

    else {
        win->mx = (uint64_t) x;
        win->my = (uint64_t) y;
    }
}


/// Switch mouse cursor behaviour within the DENG window 
void neko_SetMouseCursorMode (
    neko_Window *win, 
    neko_CursorMode mouse_mode
) {
    switch(mouse_mode) {
    case NEKO_CURSOR_MODE_HIDDEN:
        win->vc_data.is_enabled = false;
        neko_GetMousePos(win, true);
        neko_SetMouseCoords(win, win->vc_data.orig_x, win->vc_data.orig_y);
        win->vc_data.is_enabled = true;
        break;

    case NEKO_CURSOR_MODE_WAITING:
        break;

    case NEKO_CURSOR_MODE_STANDARD:
        break;

    case NEKO_CURSOR_MODE_POINTER: 
        /*_neko_XSetCursor(win, true);*/
        win->vc_data.is_enabled = false;
        break;

    default:
        break;
    }
}


/// Update window events and key arrays
/// This function is meant to be called with every loop iteration 
void neko_UpdateWindow(neko_Window *win) {
    // Check for exit event
    if(XCheckTypedWindowEvent(_neko_API.display, win->x11.window, 
       ClientMessage, &_neko_API.fr_ev)) {
        __is_running = false;
        return;
    }

    _neko_UnreleaseKeys();
    
    if(XCheckWindowEvent(_neko_API.display, win->x11.window, 
       KeyPressMask | KeyReleaseMask, &_neko_API.fr_ev)) 
        _neko_XHandleKeyEvents(win);
    
    if(XCheckWindowEvent(_neko_API.display, win->x11.window, 
       ButtonPressMask | ButtonReleaseMask, &_neko_API.fr_ev)) 
        _neko_XHandleMouseEvents(win);

    XWindowAttributes attribs;
    XGetWindowAttributes(_neko_API.display, win->x11.window, &attribs);
    win->cwidth = attribs.width;
    win->cheight = attribs.height;

    // Check if the window is used as in OpenGL context
    if(win->hints & NEKO_HINT_API_OPENGL)
        glXSwapBuffers(_neko_API.display, win->x11.window);
}


/// Destroy window instance and free all resources that were used
void neko_DestroyWindow(neko_Window *win) {
    if(!(win->hints & NEKO_HINT_API_OPENGL)) {
        XFreeGC(_neko_API.display, win->x11.gc);
    }
    XDestroyWindow(_neko_API.display, win->x11.window);
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


/// Check if window is still running and no close events have happened
bool neko_IsRunning() {
    return __is_running;
}


void neko_UpdateSizeMode(neko_Window *win, neko_Hint hints) {
    if(win->hints & NEKO_HINT_API_OPENGL)
        win->hints = hints | NEKO_HINT_API_OPENGL;
    else if(win->hints & NEKO_HINT_API_VULKAN)
        win->hints = hints | NEKO_HINT_API_VULKAN;
    else win->hints = hints;

    _neko_UpdateWindowSize(win);
}


static void _neko_CreateGLContext(neko_Window *win) {
    GLXContext glc = glXCreateContext(_neko_API.display, win->x11.p_vi, NULL, GL_TRUE);
    glXMakeCurrent(_neko_API.display, win->x11.window, glc);
}


static void _neko_SendClientMessage(neko_Window *win, Atom msg_type, long *data) {
    XEvent ev = { ClientMessage };
    ev.xclient.window = win->x11.window;
    ev.xclient.format = 32;
    ev.xclient.message_type = msg_type;
    ev.xclient.data.l[0] = data[0];
    ev.xclient.data.l[1] = data[1];
    ev.xclient.data.l[2] = data[2];
    ev.xclient.data.l[3] = data[3];
    ev.xclient.data.l[4] = data[4];

    XSendEvent(_neko_API.display, _neko_API.root, False, 
        SubstructureNotifyMask | SubstructureRedirectMask, &ev);
}



static void _neko_UpdateWindowSize(neko_Window *win) {
    /// Set flags for creating a fixed window
    /// however it is up to windows manager to decide if the size hint flags 
    /// are respected or not
    if(!(win->hints & NEKO_HINT_RESIZEABLE)) {
        XSizeHints size_hints = { 0 };
        size_hints.flags |= (PMinSize | PMaxSize);
        size_hints.min_width = size_hints.max_width = win->swidth;
        size_hints.min_height = size_hints.max_height = win->sheight;   
        
        // Submit size hints
        XSetWMNormalHints(_neko_API.display, win->x11.window, &size_hints);
    }

    else if(!(win->hints & NEKO_HINT_RESIZEABLE)) {
        XSizeHints size_hints = { 0 };
        size_hints.flags |= (PMinSize | PMaxSize);
        size_hints.min_width = -1;
        size_hints.max_width = -1;
        size_hints.min_height = -1;
        size_hints.max_height = -1;

        // Submit size hints
        XSetWMNormalHints(_neko_API.display, win->x11.window, &size_hints);
    }

    if(win->hints & NEKO_HINT_FULL_SCREEN) {
        // Send _NET_WM_STATE_FULLSCREEN atom to the window manager and the window manager should make 
        // the window into fullscreen
        long ldata[5] = { _NET_WM_STATE_ADD, _neko_API.atoms._NET_WM_STATE_FULLSCREEN, 0, 1, 0 };
        _neko_SendClientMessage(win, _neko_API.atoms._NET_WM_STATE, ldata);
    }

    else {
        // Send _NET_WM_STATE_REMOVE event to the window manager
        long ldata[] = { _NET_WM_STATE_REMOVE, _neko_API.atoms._NET_WM_STATE_FULLSCREEN, 0, 1, 0 };
        _neko_SendClientMessage(win, _neko_API.atoms._NET_WM_STATE, ldata);
    }
}


/// Initialise all cursors to use with xlib
/*static void _neko_XInitCursors(neko_Window *win) {*/
    /*__hidden_cur = None;*/

    /*// Load default cursor*/
    /*__default_cur = XcursorLibraryLoadCursor(_neko_API.display, __NEKO_XLIB_DEFAULT_CURSOR);*/
/*}*/


/*/// Destroy all cursors used in DENG*/
/*static void _neko_XFreeCursors(neko_Window *win) {*/
    /*XFreeCursor(_neko_API.display, __default_cur);*/
    /*XFreeCursor(_neko_API.display, __hidden_cur);*/
/*}*/


/// Unlike WIN32 api X11 doesn't have a callback system on events, which
/// means that key events must be checked manually on every frame update 
static void _neko_XHandleKeyEvents(neko_Window *win) {
    neko_Key key;
    switch (_neko_API.fr_ev.type) {
    case KeyPress: {
        key = translateX11Key(XLookupKeysym(&_neko_API.fr_ev.xkey, 0));
        _neko_RegisterKeyEvent(key, NEKO_MOUSE_BTN_UNKNOWN, NEKO_INPUT_TYPE_KB, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        break;
    }
        
    case KeyRelease:
        key = translateX11Key(XLookupKeysym(&_neko_API.fr_ev.xkey, 0));
        _neko_RegisterKeyEvent(key, NEKO_MOUSE_BTN_UNKNOWN, NEKO_INPUT_TYPE_KB,
            NEKO_INPUT_EVENT_TYPE_RELEASED);
        break;

    default:
        break;
    }
} 


/// Check for any mouse button events
static void _neko_XHandleMouseEvents(neko_Window *win) {
    neko_MouseButton btn = NEKO_MOUSE_BTN_UNKNOWN;
    switch (_neko_API.fr_ev.type) {
    case ButtonPress:
        btn = translateX11Btn(_neko_API.fr_ev.xbutton.button);
        _neko_RegisterKeyEvent(NEKO_KEY_UNKNOWN, btn, NEKO_INPUT_TYPE_MOUSE, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        break;
    
    case ButtonRelease:
        btn = translateX11Btn(_neko_API.fr_ev.xbutton.button);
        _neko_RegisterKeyEvent(NEKO_KEY_UNKNOWN, btn, NEKO_INPUT_TYPE_MOUSE, NEKO_INPUT_EVENT_TYPE_RELEASED);
        break;

    default:
        break;
    }
}


/// Set new width and height from the recieved event
static void _neko_XHandleResize(neko_Window *win) {
    win->cwidth = _neko_API.fr_ev.xconfigure.width;
    win->cheight = _neko_API.fr_ev.xconfigure.height;
}
