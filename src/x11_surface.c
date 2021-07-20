/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: x11_surface.c - x11 window / surface creation handler source file
/// author: Karl-Mihkel Ott

#include <X11/Xlib.h>
#define __NEKO_SURFACE_C
#include <surface_window.h>

// Static X11 exclusive functions


neko_Window *neko_InitSurfaceWindow (
    int32_t width, 
    int32_t height, 
    neko_Hint hints,
    const char *title
) {
    static neko_Window win;

    // Fill the window structure
    win.width = width;
    win.height = height;
    win.window_title = title;

    win.vc_data.is_enabled = 0;
    win.hints = hints;
    win.vc_data.orig_x = (float) width / 2;
    win.vc_data.orig_y = (float) height / 2;
    win.vc_data.x = 0;
    win.vc_data.y = 0;
 
    win.x11.display = XOpenDisplay(NULL);
    win.x11.screen = DefaultScreen(win.x11.display);

    // Check the requested API and create a window instance accordingly
    if(hints & NEKO_HINT_API_VULKAN) {
        win.is_opengl = false;
        _neko_CreateVkWindow(&win);
    }
    
    else if(hints & NEKO_HINT_API_OPENGL) {
        win.is_opengl = true;
        _neko_CreateGlWindow(&win);
    }

    return &win;
}


/// Create a simple X window to act as a Vulkan surface
static void _neko_CreateVkWindow(neko_Window *p_win) {
    p_win->x11.window = XCreateSimpleWindow(p_win->x11.display, DefaultRootWindow(p_win->x11.display), 
        0, 0, p_win->width, p_win->height, __NEKO_DEFAULT_WINDOW_BORDER, WhitePixel(p_win->x11.display, p_win->x11.screen), 
        BlackPixel(p_win->x11.display, p_win->x11.screen));

    // Set standard properties needed for x11 window creation
    XSetStandardProperties(p_win->x11.display, p_win->x11.window, p_win->window_title, 
        p_win->window_title, None, NULL, 0, NULL);

    XSelectInput(p_win->x11.display, p_win->x11.window, EVENT_MASK);
    p_win->x11.gc = XCreateGC(p_win->x11.display, 
                                      p_win->x11.window, 0, 0);

    _neko_ApplySizeHints(p_win);

    XSetBackground(p_win->x11.display, p_win->x11.gc, 
        BlackPixel(p_win->x11.display, p_win->x11.screen));

    XSetForeground(p_win->x11.display, p_win->x11.gc, 
        WhitePixel(p_win->x11.display, p_win->x11.screen));

    // Check if keyboard autorepeat is enabled
    Bool supported;
    if(!XkbSetDetectableAutoRepeat(p_win->x11.display, True, &supported)) {
        fprintf(stderr, "neko_InitVKSurfaceWindow(), %u: %s\n", __LINE__, "Detectable auto repeat is not supported on this system");
        exit(-1);
    }

    XClearWindow(p_win->x11.display, p_win->x11.window);
    XMapRaised(p_win->x11.display, p_win->x11.window);

    __is_running = true;

    Atom *WM_DELETE_WINDOW = &p_win->x11.atoms.WM_DELETE_WINDOW;
    *WM_DELETE_WINDOW = XInternAtom(p_win->x11.display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(p_win->x11.display, p_win->x11.window, WM_DELETE_WINDOW, True);
    /*_neko_XInitCursors(p_win);*/
}


static void _neko_CreateGlWindow(neko_Window *p_win) {
    // Array of OpenGL attributes
    GLint attributes[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    p_win->x11.vi = glXChooseVisual(p_win->x11.display, 0, attributes);

    Window root = DefaultRootWindow(p_win->x11.display);
    p_win->x11.swa.colormap = XCreateColormap(p_win->x11.display, root, p_win->x11.vi->visual, AllocNone);
    if(p_win->hints & NEKO_HINT_FULL_SCREEN) {
        XRRScreenResources *sc_res = XRRGetScreenResources(p_win->x11.display, root);
        XRRCrtcInfo *info = XRRGetCrtcInfo(p_win->x11.display, sc_res, *sc_res->crtcs);
        p_win->width = info->width;
        p_win->height = info->height;
        p_win->x11.swa.override_redirect = True;
    }

    // Create a new window instance for OpenGL renderer targets
    p_win->x11.window = XCreateWindow(p_win->x11.display, root, 0, 0, p_win->width, p_win->height,
        0, p_win->x11.vi->depth, InputOutput, p_win->x11.vi->visual, VALUE_MASK, &p_win->x11.swa);

    XMapWindow(p_win->x11.display, p_win->x11.window);
    XStoreName(p_win->x11.display, p_win->x11.window, p_win->window_title);

    GLXContext glc = glXCreateContext(p_win->x11.display, p_win->x11.vi, NULL, GL_TRUE);
    glXMakeCurrent(p_win->x11.display, p_win->x11.window, glc);


    // Retrieve all possible net atoms and apply size hints
    _neko_GetAtoms(p_win);
    _neko_ApplySizeHints(p_win);

    // glEnable(GL_DEPTH_TEST);

    __is_running = true;

    p_win->x11.atoms.WM_DELETE_WINDOW = XInternAtom(p_win->x11.display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(p_win->x11.display, p_win->x11.window, &p_win->x11.atoms.WM_DELETE_WINDOW, 1);
    /*_neko_XInitCursors(p_win);*/
}


static void _neko_GetAtoms(neko_Window *p_win) {
    p_win->x11.atoms._NET_WM_STATE = XInternAtom(p_win->x11.display, "_NET_WM_STATE", False);
    p_win->x11.atoms._NET_WM_STATE_FULLSCREEN = XInternAtom(p_win->x11.display, "_NET_WM_STATE_FULLSCREEN", False);
}


static void _neko_ApplySizeHints(neko_Window *p_win) {
    /// Set flags for creating a fixed window
    /// however it is up to windows manager to decide if the size hint flags 
    /// are respected or not
    if(p_win->hints & NEKO_HINT_FIXED_SIZE) {
        XSizeHints size_hints = { 0 };
        size_hints.flags |= (PMinSize | PMaxSize);
        size_hints.min_width = size_hints.max_width = p_win->width;
        size_hints.min_height = size_hints.max_height = p_win->height;   
        
        // Submit size hints
        XSetWMNormalHints(p_win->x11.display, p_win->x11.window, &size_hints);
    }

    else if(p_win->hints & NEKO_HINT_FULL_SCREEN) {
        // I assume that your window manager honors _NET_WM_STATE atoms. 
        // I do not intend to make some weird workaround for some obscure wm-s to make fullscreen functionality work, fuck off!
        Atom *_NET_WM_STATE = &p_win->x11.atoms._NET_WM_STATE; 
        Atom *_NET_WM_STATE_FULLSCREEN = &p_win->x11.atoms._NET_WM_STATE_FULLSCREEN;
        
        XChangeProperty(p_win->x11.display, p_win->x11.window, *_NET_WM_STATE, XA_ATOM, 32, 
            PropModeReplace, (unsigned char*) _NET_WM_STATE_FULLSCREEN, 1);

        p_win->x11.swa.override_redirect = True;
        XChangeWindowAttributes(p_win->x11.display, p_win->x11.window, VALUE_MASK, &p_win->x11.swa);
    }
}


/// Initialise all cursors to use with xlib
/*static void _neko_XInitCursors(neko_Window *p_win) {*/
    /*__hidden_cur = None;*/

    /*// Load default cursor*/
    /*__default_cur = XcursorLibraryLoadCursor(p_win->x11.display, __NEKO_XLIB_DEFAULT_CURSOR);*/
/*}*/


/*/// Destroy all cursors used in DENG*/
/*static void _neko_XFreeCursors(neko_Window *p_win) {*/
    /*XFreeCursor(p_win->x11.display, __default_cur);*/
    /*XFreeCursor(p_win->x11.display, __hidden_cur);*/
/*}*/


/// Unlike WIN32 api X11 doesn't have a callback system on events, which
/// means that key events must be checked manually on every frame update 
static void _neko_XHandleKeyEvents(neko_Window *p_win) {
    neko_Key key;
    switch (p_win->x11.event.type) {
    case KeyPress: {
        key = translateX11Key(XLookupKeysym(&p_win->x11.event.xkey, 0));
        _neko_RegisterKeyEvent(key, NEKO_MOUSE_BTN_UNKNOWN, NEKO_INPUT_TYPE_KB, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        break;
    }
        
    case KeyRelease:
        if(XEventsQueued(p_win->x11.display, QueuedAfterReading)) {
            key = translateX11Key(XLookupKeysym(&p_win->x11.event.xkey, 0));
            _neko_RegisterKeyEvent(key, NEKO_MOUSE_BTN_UNKNOWN, NEKO_INPUT_TYPE_KB,
                NEKO_INPUT_EVENT_TYPE_RELEASED);
        }
        break;

    default:
        break;
    }
} 


/// Check for any mouse button events
static void _neko_XHandleMouseEvents(neko_Window *p_win) {
    neko_MouseButton btn = NEKO_MOUSE_BTN_UNKNOWN;
    switch (p_win->x11.event.type) {
    case ButtonPress:
        btn = translateX11Btn(p_win->x11.event.xbutton.button);
        _neko_RegisterKeyEvent(NEKO_KEY_UNKNOWN, btn, NEKO_INPUT_TYPE_MOUSE, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        break;
    
    case ButtonRelease:
        btn = translateX11Btn(p_win->x11.event.xbutton.button);
        _neko_RegisterKeyEvent(NEKO_KEY_UNKNOWN, btn, NEKO_INPUT_TYPE_MOUSE, NEKO_INPUT_EVENT_TYPE_RELEASED);
        break;

    default:
        break;
    }
}


/// Set new width and height from the recieved event
static void _neko_XHandleResize(neko_Window *p_win) {
    p_win->width = p_win->x11.event.xconfigure.width;
    p_win->height = p_win->x11.event.xconfigure.height;
}


/// Force mouse cursor to certain location on window
void neko_SetMouseCoords (
    neko_Window *p_win, 
    uint64_t x, 
    uint64_t y
) {
    XWarpPointer(p_win->x11.display, None, p_win->x11.window, 0, 0, 0, 0, x, y);
}


/// Synchronise mouse position in neko_Window
void neko_GetMousePos (
    neko_Window *p_win, 
    bool init_vc
) {
    // dummy variables for x11
    Window return_window;
    int win_x, win_y, x, y;
    unsigned int mask;
    XQueryPointer(p_win->x11.display, p_win->x11.window, &return_window, &return_window, 
        &win_x, &win_y, &x, &y, &mask);

    if(p_win->vc_data.is_enabled && !init_vc) {
        double movement_x = (double) ((uint64_t) x - p_win->vc_data.orig_x);
        double movement_y = (double) ((uint64_t) y - p_win->vc_data.orig_y);

        if(x != p_win->vc_data.orig_x || y != p_win->vc_data.orig_y)
            neko_SetMouseCoords(p_win, p_win->vc_data.orig_x, p_win->vc_data.orig_y);

        // Check for overflow on x position
        if(p_win->vc_data.x + movement_x >= (double) __max_vc_x) {
            if(__x_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION) {
                p_win->vc_data.x = __min_vc_x;
            }
        }
        
        else if(p_win->vc_data.x + movement_x <= (double) __min_vc_x) {
            if(__x_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                p_win->vc_data.x = __max_vc_x;
        }

        else p_win->vc_data.x += movement_x;


        // Check for overflow on y position
        if(p_win->vc_data.y + movement_y >= (double) __max_vc_y) {
            if(__y_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                p_win->vc_data.y = __min_vc_y;
        }
        
        else if(p_win->vc_data.y + movement_y <= (double) __min_vc_y) {
            if(__y_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                p_win->vc_data.y = __max_vc_y;
        }

        else p_win->vc_data.y += movement_y;
    }

    else {
        p_win->mx = (uint64_t) x;
        p_win->my = (uint64_t) y;
    }
}


/// Currently only mouse hidden and mouse shown are supported 
/*static void _neko_XSetCursor (*/
    /*neko_Window *p_win,*/
    /*bool hide*/
/*) {*/
    /*if(hide)*/
        /*XDefineCursor(p_win->x11.display, p_win->x11.window, __hidden_cur);*/
    /*else XDefineCursor(p_win->x11.display, p_win->x11.window, __default_cur);*/
/*}*/


/// Switch mouse cursor behaviour within the DENG window 
void neko_SetMouseCursorMode (
    neko_Window *p_win, 
    neko_MouseMode mouse_mode
) {
    switch(mouse_mode) {
    case NEKO_MOUSE_MODE_INVISIBLE:
        /*_neko_XSetCursor(p_win, false);*/
        p_win->vc_data.is_enabled = false;
        neko_GetMousePos(p_win, true);
        neko_SetMouseCoords(p_win, p_win->vc_data.orig_x, p_win->vc_data.orig_y);
        p_win->vc_data.is_enabled = true;
        break;

    case NEKO_MOUSE_MODE_CURSOR_VISIBLE: 
        /*_neko_XSetCursor(p_win, true);*/
        p_win->vc_data.is_enabled = false;
        break;

    default:
        break;
    }
}


/// Update window events and key arrays
/// This function is meant to be called with every loop iteration 
void neko_UpdateWindow(neko_Window *p_win) {
    // Check for exit event
    if(XCheckTypedWindowEvent(p_win->x11.display, p_win->x11.window, 
       ClientMessage, &p_win->x11.event)) {
        __is_running = false;
        return;
    }

    _neko_UnreleaseKeys();
    
    if(XCheckWindowEvent(p_win->x11.display, p_win->x11.window, 
       KeyPressMask | KeyReleaseMask, &p_win->x11.event)) 
        _neko_XHandleKeyEvents(p_win);
    
    if(XCheckWindowEvent(p_win->x11.display, p_win->x11.window, 
       ButtonPressMask | ButtonReleaseMask, &p_win->x11.event)) 
        _neko_XHandleMouseEvents(p_win);

    XWindowAttributes attribs;
    XGetWindowAttributes(p_win->x11.display, p_win->x11.window, &attribs);
    p_win->width = attribs.width;
    p_win->height = attribs.height;

    // Check if the window is used as in OpenGL context
    if(p_win->is_opengl)
        glXSwapBuffers(p_win->x11.display, p_win->x11.window);
}


/// Destroy window instance and free all resources that were used
void neko_DestroyWindow(neko_Window *p_win) {
    if(!p_win->is_opengl) {
        XFreeGC(p_win->x11.display, p_win->x11.gc);
        /*_neko_XFreeCursors(p_win);*/
    }
    XDestroyWindow(p_win->x11.display, p_win->x11.window);
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
