/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: x11_surface.c - x11 window / surface creation handler source file
/// author: Karl-Mihkel Ott

#define __NEKO_SURFACE_C
#include <surface_window.h>

// Static X11 exclusive functions
static void __neko_XInitCursors(neko_Window *p_win);
static void __neko_XFreeCursors(neko_Window *p_win);
static void __neko_XHandleKeyEvents(); 
static void __neko_XHandleMouseEvents(); 
static void __neko_XHandleResize(neko_Window *p_win);
static void __neko_XSetCursor (
    neko_Window *p_win,
    bool hide
);


static void __neko_CreateVkWindow(neko_Window *p_win);
static void __neko_CreateGlWindow(neko_Window *p_win);
static XSizeHints __neko_SetSizeHints(neko_Window *p_win);
static Atom __atom_kill;
static Cursor __hidden_cur;
static Cursor __default_cur;


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
 
    win.x11_handler.p_display = XOpenDisplay(NULL);
    win.x11_handler.screen = DefaultScreen(win.x11_handler.p_display);

    // Check the requested API and create a window instance accordingly
    if(hints & NEKO_HINT_API_VULKAN) {
        win.is_opengl = false;
        __neko_CreateVkWindow(&win);
    }
    
    else if(hints & NEKO_HINT_API_OPENGL) {
        win.is_opengl = true;
        __neko_CreateGlWindow(&win);
    }

    return &win;
}


/// Create a simple X window to act as a Vulkan surface
static void __neko_CreateVkWindow(neko_Window *p_win) {
    p_win->x11_handler.window = XCreateSimpleWindow(p_win->x11_handler.p_display, DefaultRootWindow(p_win->x11_handler.p_display), 
        0, 0, p_win->width, p_win->height, __NEKO_DEFAULT_WINDOW_BORDER, WhitePixel(p_win->x11_handler.p_display, p_win->x11_handler.screen), 
        BlackPixel(p_win->x11_handler.p_display, p_win->x11_handler.screen));

    // Set standard properties needed for x11 window creation
    XSetStandardProperties(p_win->x11_handler.p_display, p_win->x11_handler.window, p_win->window_title, 
        p_win->window_title, None, NULL, 0, NULL);

    // Set the window manager size hints
    XSizeHints hints = __neko_SetSizeHints(p_win);
    XSetWMNormalHints(p_win->x11_handler.p_display, p_win->x11_handler.window, &hints);

    XSelectInput(p_win->x11_handler.p_display, p_win->x11_handler.window, EVENT_MASKS);
    p_win->x11_handler.gc = XCreateGC(p_win->x11_handler.p_display, 
                                      p_win->x11_handler.window, 0, 0);

    XSetBackground(p_win->x11_handler.p_display, p_win->x11_handler.gc, 
        BlackPixel(p_win->x11_handler.p_display, p_win->x11_handler.screen));

    XSetForeground(p_win->x11_handler.p_display, p_win->x11_handler.gc, 
        WhitePixel(p_win->x11_handler.p_display, p_win->x11_handler.screen));

    // Check if keyboard autorepeat is enabled
    Bool supported;
    if(!XkbSetDetectableAutoRepeat(p_win->x11_handler.p_display, True, &supported)) {
        fprintf(stderr, "neko_InitVKSurfaceWindow(), %u: %s\n", __LINE__, "Detectable auto repeat is not supported on this system");
        exit(-1);
    }

    XClearWindow(p_win->x11_handler.p_display, p_win->x11_handler.window);
    XMapRaised(p_win->x11_handler.p_display, p_win->x11_handler.window);

    __is_running = true;

    __atom_kill = XInternAtom(p_win->x11_handler.p_display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(p_win->x11_handler.p_display, p_win->x11_handler.window, &__atom_kill, True);
    __neko_XInitCursors(p_win);
}


static void __neko_CreateGlWindow(neko_Window *p_win) {
    // Array of OpenGL attributes
    GLint attributes[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    p_win->x11_handler.vi = glXChooseVisual(p_win->x11_handler.p_display, 0, attributes);

    XSetWindowAttributes swa = { 0 };
    Window root = DefaultRootWindow(p_win->x11_handler.p_display);
    swa.colormap = XCreateColormap(p_win->x11_handler.p_display, root, p_win->x11_handler.vi->visual, AllocNone);
    swa.event_mask = EVENT_MASKS;

    // Create a new window instance for OpenGL renderer targets
    p_win->x11_handler.window = XCreateWindow(p_win->x11_handler.p_display, root, 0, 0, p_win->width, p_win->height,
        0, p_win->x11_handler.vi->depth, InputOutput, p_win->x11_handler.vi->visual, CWColormap | CWEventMask, &swa);

    XMapWindow(p_win->x11_handler.p_display, p_win->x11_handler.window);
    XStoreName(p_win->x11_handler.p_display, p_win->x11_handler.window, p_win->window_title);

    // Apply all given size hints
    XSizeHints s_hints = __neko_SetSizeHints(p_win);
    XSetWMNormalHints(p_win->x11_handler.p_display, p_win->x11_handler.window, &s_hints);

    GLXContext glc = glXCreateContext(p_win->x11_handler.p_display, p_win->x11_handler.vi, NULL, GL_TRUE);
    glXMakeCurrent(p_win->x11_handler.p_display, p_win->x11_handler.window, glc);
    // glEnable(GL_DEPTH_TEST);

    __is_running = true;
    __atom_kill = XInternAtom(p_win->x11_handler.p_display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(p_win->x11_handler.p_display, p_win->x11_handler.window, &__atom_kill, True);
    __neko_XInitCursors(p_win);
}


/// Set size hints according to the specified window mode
static XSizeHints __neko_SetSizeHints(neko_Window *p_win) {
    /// Set flags for creating a fixed window
    /// however it is up to windows manager to decide if the size hint flag
    /// is respected or not
    XSizeHints size_hints = { 0 };
    if(p_win->hints & NEKO_HINT_FIXED_SIZE) {
        size_hints.flags |= (PMinSize | PMaxSize);
        size_hints.min_width = size_hints.max_width = p_win->width;
        size_hints.min_height = size_hints.max_height = p_win->height;   
    }

    return size_hints;
}


/// Initialise all cursors to use with xlib
static void __neko_XInitCursors(neko_Window *p_win) {
    __hidden_cur = None;

    // Load default cursor
    __default_cur = XcursorLibraryLoadCursor(p_win->x11_handler.p_display, __NEKO_XLIB_DEFAULT_CURSOR);
}


/// Destroy all cursors used in DENG
static void __neko_XFreeCursors(neko_Window *p_win) {
    XFreeCursor(p_win->x11_handler.p_display, __default_cur);
    XFreeCursor(p_win->x11_handler.p_display, __hidden_cur);
}


/// Unlike WIN32 api X11 doesn't have a callback system on events, which
/// means that key events must be checked manually on every frame update 
static void __neko_XHandleKeyEvents(neko_Window *p_win) {
    neko_Key key;
    switch (p_win->x11_handler.event.type) {
    case KeyPress: {
        key = translateX11Key(XLookupKeysym(&p_win->x11_handler.event.xkey, 0));
        __neko_RegisterKeyEvent(key, NEKO_MOUSE_BTN_UNKNOWN, NEKO_INPUT_TYPE_KB, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        break;
    }
        
    case KeyRelease:
        if(XEventsQueued(p_win->x11_handler.p_display, QueuedAfterReading)) {
            key = translateX11Key(XLookupKeysym(&p_win->x11_handler.event.xkey, 0));
            __neko_RegisterKeyEvent(key, NEKO_MOUSE_BTN_UNKNOWN, NEKO_INPUT_TYPE_KB,
                NEKO_INPUT_EVENT_TYPE_RELEASED);
        }
        break;

    default:
        break;
    }
} 


/// Check for any mouse button events
static void __neko_XHandleMouseEvents(neko_Window *p_win) {
    neko_MouseButton btn = NEKO_MOUSE_BTN_UNKNOWN;
    switch (p_win->x11_handler.event.type) {
    case ButtonPress:
        btn = translateX11Btn(p_win->x11_handler.event.xbutton.button);
        __neko_RegisterKeyEvent(NEKO_KEY_UNKNOWN, btn, NEKO_INPUT_TYPE_MOUSE, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        break;
    
    case ButtonRelease:
        btn = translateX11Btn(p_win->x11_handler.event.xbutton.button);
        __neko_RegisterKeyEvent(NEKO_KEY_UNKNOWN, btn, NEKO_INPUT_TYPE_MOUSE, NEKO_INPUT_EVENT_TYPE_RELEASED);
        break;

    default:
        break;
    }
}


/// Set new width and height from the recieved event
static void __neko_XHandleResize(neko_Window *p_win) {
    p_win->width = p_win->x11_handler.event.xconfigure.width;
    p_win->height = p_win->x11_handler.event.xconfigure.height;
}


/// Force mouse cursor to certain location on window
void neko_SetMouseCoords (
    neko_Window *p_win, 
    uint64_t x, 
    uint64_t y
) {
    XWarpPointer(p_win->x11_handler.p_display, None, p_win->x11_handler.window, 0, 0, 0, 0, x, y);
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
    XQueryPointer(p_win->x11_handler.p_display, p_win->x11_handler.window, &return_window, &return_window, 
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
static void __neko_XSetCursor (
    neko_Window *p_win,
    bool hide
) {
    if(hide)
        XDefineCursor(p_win->x11_handler.p_display, p_win->x11_handler.window, __hidden_cur);
    else XDefineCursor(p_win->x11_handler.p_display, p_win->x11_handler.window, __default_cur);
}


/// Switch mouse cursor behaviour within the DENG window 
void neko_SetMouseCursorMode (
    neko_Window *p_win, 
    neko_MouseMode mouse_mode
) {
    switch(mouse_mode) {
    case NEKO_MOUSE_MODE_INVISIBLE:
        __neko_XSetCursor(p_win, false);
        p_win->vc_data.is_enabled = false;
        neko_GetMousePos(p_win, true);
        neko_SetMouseCoords(p_win, p_win->vc_data.orig_x, p_win->vc_data.orig_y);
        p_win->vc_data.is_enabled = true;
        break;

    case NEKO_MOUSE_MODE_CURSOR_VISIBLE: 
        __neko_XSetCursor(p_win, true);
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
    if(XCheckTypedWindowEvent(p_win->x11_handler.p_display, p_win->x11_handler.window, 
       ClientMessage, &p_win->x11_handler.event)) {
        __is_running = false;
        return;
    }

    __neko_UnreleaseKeys();
    
    if(XCheckWindowEvent(p_win->x11_handler.p_display, p_win->x11_handler.window, 
       KeyPressMask | KeyReleaseMask, &p_win->x11_handler.event)) 
        __neko_XHandleKeyEvents(p_win);
    
    if(XCheckWindowEvent(p_win->x11_handler.p_display, p_win->x11_handler.window, 
       ButtonPressMask | ButtonReleaseMask, &p_win->x11_handler.event)) 
        __neko_XHandleMouseEvents(p_win);

    XWindowAttributes attribs;
    XGetWindowAttributes(p_win->x11_handler.p_display, p_win->x11_handler.window, &attribs);
    p_win->width = attribs.width;
    p_win->height = attribs.height;

    // Check if the window is used as in OpenGL context
    if(p_win->is_opengl)
        glXSwapBuffers(p_win->x11_handler.p_display, p_win->x11_handler.window);
}


/// Destroy window instance and free all resources that were used
void neko_DestroyWindow(neko_Window *p_win) {
    if(!p_win->is_opengl) {
        XFreeGC(p_win->x11_handler.p_display, p_win->x11_handler.gc);
        __neko_XFreeCursors(p_win);
    }
    XDestroyWindow(p_win->x11_handler.p_display, p_win->x11_handler.window);
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
