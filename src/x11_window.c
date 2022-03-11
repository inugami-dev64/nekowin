/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: x11_surface.c - x11 window / surface creation handler source file
/// author: Karl-Mihkel Ott

#define __NWIN_C
#define __X11_WINDOW_C
#include <nwin.h>


/*******************************/
/******* NEKO API CALLS ********/
/*******************************/

/// Initialise platform dependent backend api for nekowin library
void neko_InitAPI() {
    XInitThreads();
    _neko_API.display = XOpenDisplay(NULL);
    _neko_API.root = DefaultRootWindow(_neko_API.display);
    _neko_API.scr = DefaultScreen(_neko_API.display);

    // Get basic atom values
    _neko_API.atoms.WM_DELETE_WINDOW = XInternAtom(_neko_API.display, "WM_DELETE_WINDOW", False);
    _neko_API.atoms._NET_WM_STATE = XInternAtom(_neko_API.display, "_NET_WM_STATE", False);
    _neko_API.atoms._NET_WM_STATE_FULLSCREEN = XInternAtom(_neko_API.display, "_NET_WM_STATE_FULLSCREEN", False);

    _neko_LoadCursors();

    // Check if detectable keyboard autorepeat is supported
    Bool supported;
    neko_assert(XkbSetDetectableAutoRepeat(_neko_API.display, True, &supported), 
                "DetectableAutoRepeat is not supported");

    _neko_API.is_init = true;
}


bool neko_APIInitStatus() {
    return _neko_API.is_init;
};



void neko_DeinitAPI() {
    XFreeCursor(_neko_API.display, _neko_API.cursors.standard);
    XFreeCursor(_neko_API.display, _neko_API.cursors.pointer);
    XFreeCursor(_neko_API.display, _neko_API.cursors.hidden);
    XFreeCursor(_neko_API.display, _neko_API.cursors.waiting);

    _neko_API.cursors.standard = 0UL;
    _neko_API.cursors.pointer = 0UL;
    _neko_API.cursors.hidden = 0UL;
    _neko_API.cursors.waiting = 0UL;

    // This causes a memory leak but calling XCloseDisplay causes segmentation fault instead :/
    _neko_API.display = NULL;
    _neko_API.is_init = false;
}


neko_Window neko_NewWindow (
    int32_t width,
    int32_t height,
    neko_Hint hints,
    const char *title
) {
    neko_assert(_neko_API.is_init, "Please initialise neko library with neko_InitAPI() before creating new windows");
    neko_assert(wslot_reserved + 1 >= __MAX_WSLOT_C, "There are no free window slots available");
    neko_Window win = (wslot_reserved++);

    // Fill the window structure
    wslots[win].owidth = width = wslots[win].cwidth = width;
    wslots[win].oheight = wslots[win].cheight = height;
    wslots[win].window_title = title;

    wslots[win].hints = hints;
    wslots[win].vc_data.is_enabled = false;
    wslots[win].vc_data.orig_x = (float) width / 2.0f;
    wslots[win].vc_data.orig_y = (float) height / 2.0f;
    wslots[win].vc_data.x = 0;
    wslots[win].vc_data.y = 0;
 
    // Retrieve visual info about the window
    _neko_GetVisualInfo(win);

    XSetWindowAttributes swa = { 0 };
    swa.event_mask = EVENT_MASK;
    
    // Create a new window
    if(wslots[win].x11.p_vi) {
        swa.colormap = XCreateColormap(_neko_API.display, _neko_API.root, wslots[win].x11.p_vi->visual, AllocNone);
        wslots[win].x11.window = XCreateWindow(_neko_API.display, _neko_API.root, 
                                               0, 0, wslots[win].cwidth, wslots[win].cheight,
                                               0, wslots[win].x11.p_vi->depth, 
                                               InputOutput, wslots[win].x11.p_vi->visual, 
                                               VALUE_MASK, 
                                               &swa);
    } else {
        swa.colormap = XCreateColormap(_neko_API.display, _neko_API.root, wslots[win].x11.vi.visual, AllocNone);
        wslots[win].x11.window = XCreateWindow(_neko_API.display, _neko_API.root, 
                                               0, 0, wslots[win].cwidth, wslots[win].cheight,
                                               0, wslots[win].x11.vi.depth, 
                                               InputOutput, wslots[win].x11.vi.visual, 
                                               VALUE_MASK, 
                                               &swa);
    }

    // Check if the window was created successfully
    neko_assert(wslots[win].x11.window, "Failed to create x11 window!");
    XMapWindow(_neko_API.display, wslots[win].x11.window);
    XStoreName(_neko_API.display, wslots[win].x11.window, wslots[win].window_title);

    _neko_UpdateWindowSize(win);
    
    // Set the base WM protocols
    XSetWMProtocols(_neko_API.display, wslots[win].x11.window, &_neko_API.atoms.WM_DELETE_WINDOW, True);

    if(hints & NEKO_HINT_API_OPENGL)
        _neko_CreateGLContext(win);

    wslots[win].is_running = true;
    XFlush(_neko_API.display);
    return win;
}


VkResult neko_InitVKSurface (
    neko_Window win, 
    VkInstance instance,
    VkSurfaceKHR *surface
) {
    VkXlibSurfaceCreateInfoKHR surface_info;
    surface_info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    surface_info.window = wslots[win].x11.window;
    surface_info.dpy = _neko_API.display;
    surface_info.flags = 0;
    surface_info.pNext = NULL;

    PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR;
    vkCreateXlibSurfaceKHR = (PFN_vkCreateXlibSurfaceKHR) vkGetInstanceProcAddr(instance, "vkCreateXlibSurfaceKHR");

    return vkCreateXlibSurfaceKHR(instance, &surface_info, NULL, surface);
} 


/// Update window events and key arrays
/// This function is meant to be called in every frame
void neko_UpdateWindow(neko_Window win) {
    // Set notfiy booleans to false for updating resize update information
    wslots[win].resize_notify = false;
    _neko_UnreleaseKeys();
    
    XPending(_neko_API.display);
    while(QLength(_neko_API.display)) {
        XEvent ev;
        XNextEvent(_neko_API.display, &ev);

        switch(ev.type) {
            case ClientMessage:
                wslots[win].is_running = false;
                return;

            case ConfigureNotify: 
                {
                    XConfigureEvent *xce = &ev.xconfigure;

                    if(xce->width != wslots[win].cwidth || xce->height != wslots[win].cheight) {
                        wslots[win].resize_notify = true;
                        wslots[win].cwidth = xce->width;
                        wslots[win].cheight = xce->height;
                        wslots[win].vc_data.orig_x = wslots[win].cwidth / 2;
                        wslots[win].vc_data.orig_y = wslots[win].cheight / 2;
                    }
                }
                break;

            case KeyPress:
            case KeyRelease:
                {
                    XKeyEvent *kev = &ev.xkey;
                    _neko_HandleKeyEvents(ev.type, kev);
                }
                break;

            case ButtonPress:
            case ButtonRelease:
                {
                    XButtonEvent *bev = &ev.xbutton;
                    _neko_HandleMouseEvents(ev.type, bev);
                }
                break;

            case MotionNotify:
                {
                    XMotionEvent *mev = &ev.xmotion;
                    _neko_HandleMouseMovement(win, mev->x, mev->y);
                }
                break;

            default:
                break;
        }
    }

    // Check if the window is used as in OpenGL context
    if(wslots[win].hints & NEKO_HINT_API_OPENGL)
        glXSwapBuffers(_neko_API.display, wslots[win].x11.window);

    XFlush(_neko_API.display);
}


void neko_UpdateSizeMode(neko_Window win, neko_Hint hints) {
    if(wslots[win].hints & NEKO_HINT_API_OPENGL)
        wslots[win].hints = hints | NEKO_HINT_API_OPENGL;
    else if(wslots[win].hints & NEKO_HINT_API_VULKAN)
        wslots[win].hints = hints | NEKO_HINT_API_VULKAN;
    else wslots[win].hints = hints;

    _neko_UpdateWindowSize(win);
}


/// Destroy window instance and free all resources that were used
void neko_DestroyWindow(neko_Window win) {
    if(wslots[win].hints & NEKO_HINT_API_OPENGL)
        glXDestroyContext(_neko_API.display, wslots[win].x11.glc);
    XDestroyWindow(_neko_API.display, wslots[win].x11.window);
}


/// Check if window is still running and no close events have happened
bool neko_IsRunning(neko_Window win) {
    return wslots[win].is_running;
}


/// Switch mouse cursor behaviour within the DENG window 
void neko_SetMouseCursorMode (
    neko_Window win, 
    neko_CursorMode mouse_mode
) {
    switch(mouse_mode) {
    case NEKO_CURSOR_MODE_HIDDEN:
        XDefineCursor(_neko_API.display, wslots[win].x11.window, _neko_API.cursors.hidden);
        break;

    case NEKO_CURSOR_MODE_WAITING:
        XDefineCursor(_neko_API.display, wslots[win].x11.window, _neko_API.cursors.waiting);
        break;

    case NEKO_CURSOR_MODE_STANDARD:
        XDefineCursor(_neko_API.display, wslots[win].x11.window, _neko_API.cursors.standard);
        break;

    case NEKO_CURSOR_MODE_POINTER: 
        XDefineCursor(_neko_API.display, wslots[win].x11.window, _neko_API.cursors.pointer);
        break;

    default:
        break;
    }
}


/// Force mouse cursor to certain location on window
void neko_SetMouseCoords (
    neko_Window win, 
    uint64_t x, 
    uint64_t y
) {
    XWarpPointer(_neko_API.display, None, 
                 wslots[win].x11.window, 0, 0, 0, 0, x, y);
}


void neko_UpdateMousePos(neko_Window win) {
    // dummy variables for x11
    Window return_window;
    int win_x, win_y, x, y;
    unsigned int mask;
    XQueryPointer(_neko_API.display, wslots[win].x11.window, &return_window, &return_window, &win_x, &win_y, &x, &y, &mask);
    _neko_HandleMouseMovement(win, x, y);
}


void neko_FindRequiredVkExtensionsStrings(char ***p_exts, size_t *ext_s) {
    static char *sptr[3] = { 0 };
    static char lexts[3][32] = { 0 };

    *p_exts = sptr;
    (*p_exts)[0] = lexts[0];
    (*p_exts)[1] = lexts[1];
    (*p_exts)[2] = lexts[2];

#ifdef _DEBUG
    *ext_s = 3;
    strcpy((*p_exts)[2], NEKO_VK_DEBUG_UTILS_EXT_NAME);
#else
    *ext_s = 2;
#endif
    strcpy((*p_exts)[0], NEKO_VK_WSI_EXT_NAME);
    strcpy((*p_exts)[1], NEKO_VK_XLIB_SURFACE_EXT_NAME);
}


/*****************************/
/****** Inner functions ******/
/*****************************/

/// Unlike WIN32 api X11 doesn't have a callback system on events, which
/// means that key events must be checked manually on every frame update 
static void _neko_HandleKeyEvents(int type, XKeyEvent *kev) {
    neko_HidEvent hid_ev = translateX11Key(XLookupKeysym(kev, 0));

    switch (type) {
        case KeyPress: 
            _neko_RegisterKeyEvent(hid_ev, NEKO_INPUT_EVENT_TYPE_ACTIVE);
            break;
        
        case KeyRelease:
            _neko_RegisterKeyEvent(hid_ev, NEKO_INPUT_EVENT_TYPE_RELEASED);
            break;

        default:
            break;
    }
} 


/// Check for any mouse button events
static void _neko_HandleMouseEvents(int type, XButtonEvent *bev) {
    neko_HidEvent hid_ev = translateX11Btn(bev->button);

    switch (type) {
        case ButtonPress:
            _neko_RegisterKeyEvent(hid_ev, NEKO_INPUT_EVENT_TYPE_ACTIVE);
            break;
        
        case ButtonRelease:
            _neko_RegisterKeyEvent(hid_ev, NEKO_INPUT_EVENT_TYPE_RELEASED);
            break;

        default:
            break;
    }
}


// Set the new mouse position according to the current mouse movement mode
static void _neko_HandleMouseMovement(neko_Window win, int x, int y) {
    if(wslots[win].vc_data.is_enabled) {
        int64_t movement_x = (int64_t) x - wslots[win].vc_data.orig_x;
        int64_t movement_y = (int64_t) y - wslots[win].vc_data.orig_y;

        if(x != wslots[win].vc_data.orig_x || y != wslots[win].vc_data.orig_y)
            neko_SetMouseCoords(win, wslots[win].vc_data.orig_x, wslots[win].vc_data.orig_y);

        // Check for overflow on x position
        if(wslots[win].vc_data.x + movement_x >= __max_vc_x) {
            if(__x_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                wslots[win].vc_data.x = __min_vc_x;
        }
        
        else if(wslots[win].vc_data.x + movement_x <= __min_vc_x) {
            if(__x_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                wslots[win].vc_data.x = __max_vc_x;
        }

        else wslots[win].vc_data.x += movement_x;


        // Check for overflow on y position
        if(wslots[win].vc_data.y + movement_y >= __max_vc_y) {
            if(__y_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                wslots[win].vc_data.y = __min_vc_y;
        }
        
        else if(wslots[win].vc_data.y + movement_y <= __min_vc_y) {
            if(__y_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                wslots[win].vc_data.y = __max_vc_y;
        }

        else wslots[win].vc_data.y += movement_y;
    }

    else {
        wslots[win].mx = (int64_t) x;
        wslots[win].my = (int64_t) y;
    }
}


static void _neko_GetVisualInfo(neko_Window win) {
    if(wslots[win].hints & NEKO_HINT_API_OPENGL) {
        GLint attrs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
        XVisualInfo *vi = glXChooseVisual(_neko_API.display, 0, attrs);

        neko_assert(vi, "Failed to choose GLX visual");
        wslots[win].x11.p_vi = vi;
    }

    else {
        memset(&wslots[win].x11.vi, 0, sizeof(Visual));
        wslots[win].x11.vi.screen = _neko_API.scr;
        wslots[win].x11.vi.visual = DefaultVisual(_neko_API.display, _neko_API.scr);
        wslots[win].x11.vi.depth = DefaultDepth(_neko_API.display, _neko_API.scr);
        wslots[win].x11.vi.bits_per_rgb = 32;
    }
}


static void _neko_SendClientMessage(neko_Window win, Atom msg_type, long *data) {
    XEvent ev = { ClientMessage };
    ev.xclient.window = wslots[win].x11.window;
    ev.xclient.format = 32;
    ev.xclient.message_type = msg_type;
    ev.xclient.data.l[0] = data[0];
    ev.xclient.data.l[1] = data[1];
    ev.xclient.data.l[2] = data[2];
    ev.xclient.data.l[3] = data[3];
    ev.xclient.data.l[4] = data[4];

    XSendEvent(_neko_API.display, _neko_API.root, False, SubstructureNotifyMask | SubstructureRedirectMask, &ev);
}


static void _neko_UpdateWindowSize(neko_Window win) {
    if (!(wslots[win].hints & NEKO_HINT_FIXED_SIZE) && !(wslots[win].hints & NEKO_HINT_RESIZEABLE) && !(wslots[win].hints & NEKO_HINT_FULL_SCREEN))
        wslots[win].hints |= NEKO_HINT_FIXED_SIZE;

    if(wslots[win].hints & NEKO_HINT_FULL_SCREEN) {
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

    /// Set flags for creating a fixed window
    /// however it is up to windows manager to decide if the size hint flags 
    /// are respected or not
    if(wslots[win].hints & NEKO_HINT_FIXED_SIZE) {
        XSizeHints size_hints = { 0 };
        size_hints.flags |= (PMinSize | PMaxSize);
        size_hints.min_width = size_hints.max_width = wslots[win].owidth;
        size_hints.min_height = size_hints.max_height = wslots[win].oheight;   
        
        // Submit size hints
        XSetWMNormalHints(_neko_API.display, wslots[win].x11.window, &size_hints);
    }

    else if(wslots[win].hints & NEKO_HINT_RESIZEABLE) {
        XSizeHints size_hints = { 0 };

        // Submit size hints
        XSetWMNormalHints(_neko_API.display, wslots[win].x11.window, &size_hints);
        XResizeWindow(_neko_API.display, wslots[win].x11.window, wslots[win].owidth, wslots[win].oheight);
    }
}


static void _neko_CreateGLContext(neko_Window win) {
    wslots[win].x11.glc = glXCreateContext(_neko_API.display, wslots[win].x11.p_vi, NULL, GL_TRUE);
    glXMakeCurrent(_neko_API.display, wslots[win].x11.window, wslots[win].x11.glc);
}


/// Load all available platform specific nekowin cursors
static void _neko_LoadCursors() {
    _neko_API.cursors.standard = XCreateFontCursor(_neko_API.display, XC_left_ptr);
    _neko_API.cursors.pointer = XCreateFontCursor(_neko_API.display, XC_hand2);
    _neko_API.cursors.waiting = XCreateFontCursor(_neko_API.display, XC_watch);

    // Creating an empty cursor using xlib can be a bit of a pain, but the way it is done in glut
    // is to make a dummy pixmap and color instances and use them to create a zero sized pixmap cursor
    const char data = 0;
    Pixmap pix;
    XColor color;
    pix = XCreateBitmapFromData(_neko_API.display, _neko_API.root, &data, 1, 1);
    _neko_API.cursors.hidden = XCreatePixmapCursor(_neko_API.display, pix, pix, &color, &color, 0, 0);
    XFreePixmap(_neko_API.display, pix);
}
