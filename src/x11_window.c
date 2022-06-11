/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: x11_surface.c - x11 window / surface creation handler source file
/// author: Karl-Mihkel Ott

#define __NWIN_C
#define __X11_WINDOW_C
#include <nwin.h>

/*****************************/
/****** Inner functions ******/
/*****************************/

/// Unlike WIN32 api X11 doesn't have a callback system on events, which
/// means that key events must be checked manually on every frame update 
void _neko_HandleKeyEvents(int _type, XKeyEvent *_kev) {
    neko_HidEvent hid_ev = _neko_TranslateX11Key(XLookupKeysym(_kev, 0));

    switch(_type) {
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
void _neko_HandleMouseEvents(int _type, XButtonEvent *_bev) {
    neko_HidEvent hid_ev = _neko_TranslateX11Btn(_bev->button);

    switch(_type) {
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
void _neko_HandleMouseMovement(neko_Window *_win, int64_t _x, int64_t _y) {
    if(_win->vc_data.is_enabled) {
        int64_t delta_x = _x - _win->vc_data.orig_x;
        int64_t delta_y = _y - _win->vc_data.orig_y;

        if(_x != _win->vc_data.orig_x || _y != _win->vc_data.orig_y)
            neko_SetMouseCoords(_win, _win->vc_data.orig_x, _win->vc_data.orig_y);

        // Check for overflow on x position
        if(_win->vc_data.x + delta_x >= __max_vc_x) {
            if(__x_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                _win->vc_data.x = __min_vc_x;
        }
        
        else if(_win->vc_data.x + delta_x <= __min_vc_x) {
            if(__x_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                _win->vc_data.x = __max_vc_x;
        }

        else _win->vc_data.x += delta_x;


        // Check for overflow on y position
        if(_win->vc_data.y + delta_y >= __max_vc_y) {
            if(__y_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                _win->vc_data.y = __min_vc_y;
        }
        
        else if(_win->vc_data.y + delta_y <= __min_vc_y) {
            if(__y_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                _win->vc_data.y = __max_vc_y;
        }

        else _win->vc_data.y += delta_y;
    }

    else {
        _win->mx = _x;
        _win->my = _y;
    }
}


void _neko_GetVisualInfo(neko_Window *_win) {
    if(_win->hints & NEKO_HINT_API_OPENGL) {
        GLint attrs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
        XVisualInfo *vi = glXChooseVisual(_neko_API.display, 0, attrs);

        neko_assert(vi, "Failed to choose GLX visual");
        _win->x11.p_vi = vi;
    }

    else {
        memset(&_win->x11.vi, 0, sizeof(Visual));
        _win->x11.vi.screen = _neko_API.scr;
        _win->x11.vi.visual = DefaultVisual(_neko_API.display, _neko_API.scr);
        _win->x11.vi.depth = DefaultDepth(_neko_API.display, _neko_API.scr);
        _win->x11.vi.bits_per_rgb = 32;
    }
}


void _neko_SendClientMessage(neko_Window *_win, Atom _msg_type, long *_data) {
    XEvent ev = { ClientMessage };
    ev.xclient.window = _win->x11.window;
    ev.xclient.format = 32;
    ev.xclient.message_type = _msg_type;
    ev.xclient.data.l[0] = _data[0];
    ev.xclient.data.l[1] = _data[1];
    ev.xclient.data.l[2] = _data[2];
    ev.xclient.data.l[3] = _data[3];
    ev.xclient.data.l[4] = _data[4];

    XSendEvent(_neko_API.display, _neko_API.root, False, SubstructureNotifyMask | SubstructureRedirectMask, &ev);
}


void _neko_UpdateWindowSize(neko_Window *_win) {
    if (!(_win->hints & NEKO_HINT_FIXED_SIZE) && !(_win->hints & NEKO_HINT_RESIZEABLE) && !(_win->hints & NEKO_HINT_FULL_SCREEN))
        _win->hints |= NEKO_HINT_FIXED_SIZE;

    if(_win->hints & NEKO_HINT_FULL_SCREEN) {
        // Send _NET_WM_STATE_FULLSCREEN atom to the window manager and the window manager should make 
        // the window into fullscreen
        long ldata[5] = { _NET_WM_STATE_ADD, _neko_API.atoms._NET_WM_STATE_FULLSCREEN, 0, 1, 0 };
        _neko_SendClientMessage(_win, _neko_API.atoms._NET_WM_STATE, ldata);
    }

    else {
        // Send _NET_WM_STATE_REMOVE event to the window manager
        long ldata[] = { _NET_WM_STATE_REMOVE, _neko_API.atoms._NET_WM_STATE_FULLSCREEN, 0, 1, 0 };
        _neko_SendClientMessage(_win, _neko_API.atoms._NET_WM_STATE, ldata);
    }

    /// Set flags for creating a fixed window
    /// however it is up to windows manager to decide if the size hint flags 
    /// are respected or not
    if(_win->hints & NEKO_HINT_FIXED_SIZE) {
        XSizeHints size_hints = { 0 };
        size_hints.flags |= (PMinSize | PMaxSize);
        size_hints.min_width = size_hints.max_width = _win->owidth;
        size_hints.min_height = size_hints.max_height = _win->oheight;   
        
        // Submit size hints
        XSetWMNormalHints(_neko_API.display, _win->x11.window, &size_hints);
    }

    else if(_win->hints & NEKO_HINT_RESIZEABLE) {
        XSizeHints size_hints = { 0 };

        // Submit size hints
        XSetWMNormalHints(_neko_API.display, _win->x11.window, &size_hints);
        XResizeWindow(_neko_API.display, _win->x11.window, _win->owidth, _win->oheight);
    }
}


/// Load all available platform specific nekowin cursors
void _neko_LoadCursors() {
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


    // verify that GLX_EXT_swap_control extension is present
    const char* extensions = glXQueryExtensionsString(_neko_API.display, _neko_API.scr);

    // Check if any swap interval function pointers are present
    bool ext_found = false;
    const char* ptr = extensions;
    const char* pt = NULL;
    while ((pt = strchr(ptr, ' ')) && ptr != (const char*)1) {
        if (!strncmp(GLX_SWAP_CONTROL_EXT_NAME, ptr, strlen(GLX_SWAP_CONTROL_EXT_NAME)))
            _neko_API.glXSwapIntervalEXT = (PFN_glXSwapIntervalEXT)glXGetProcAddress((const GLubyte*)GLX_SWAP_CONTROL_EXT_NAME);
        else if (!strncmp(GLX_SWAP_CONTROL_SGI_NAME, ptr, strlen(GLX_SWAP_CONTROL_SGI_NAME)))
            _neko_API.glXSwapIntervalSGI = (PFN_glXSwapIntervalSGI)glXGetProcAddress((const GLubyte*)GLX_SWAP_CONTROL_SGI_NAME);
        else if (!strncmp(GLX_SWAP_CONTROL_MESA_NAME, ptr, strlen(GLX_SWAP_CONTROL_MESA_NAME)))
            _neko_API.glXSwapIntervalMESA = (PFN_glXSwapIntervalMESA)glXGetProcAddress((const GLubyte*)GLX_SWAP_CONTROL_MESA_NAME);

        ptr = pt + 1;
    }

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
    int32_t _width,
    int32_t _height,
    neko_Hint _hints,
    int32_t _spawn_x,
    int32_t _spawn_y,
    const char *_title
) {
    neko_assert(_neko_API.is_init, "Please initialise neko library with neko_InitAPI() before creating new windows");
    neko_Window win = {};

    // Fill the window structure
    win.owidth = (win.cwidth = _width);
    win.oheight = (win.cheight = _height);
    win.oposx = (win.cposx = _spawn_x);
    win.oposy = (win.cposy = _spawn_y);
    win.window_title = _title;

    win.hints = _hints;
    win.vc_data.is_enabled = false;
    win.vc_data.orig_x = (float) _width / 2.0f;
    win.vc_data.orig_y = (float) _height / 2.0f;
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
                                       win.oposx, win.oposy, win.owidth, win.oheight,
                                       0, win.x11.p_vi->depth,
                                       InputOutput, win.x11.p_vi->visual,
                                       VALUE_MASK,
                                       &swa);
    } else {
        swa.colormap = XCreateColormap(_neko_API.display, _neko_API.root, win.x11.vi.visual, AllocNone);
        win.x11.window = XCreateWindow(_neko_API.display, _neko_API.root,
                                       _spawn_x, _spawn_y, win.cwidth, win.cheight,
                                       0, win.x11.vi.depth, 
                                       InputOutput, win.x11.vi.visual,
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

    if(_hints & NEKO_HINT_API_OPENGL)
        win.x11.glc = glXCreateContext(_neko_API.display, win.x11.p_vi, NULL, GL_TRUE);

    win.is_running = true;
    XFlush(_neko_API.display);
    return win;
}


VkResult neko_InitVkSurface (
    neko_Window *_win, 
    VkInstance _ins,
    VkSurfaceKHR *_surface
) {
    VkXlibSurfaceCreateInfoKHR surface_info;
    surface_info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    surface_info.window = _win->x11.window;
    surface_info.dpy = _neko_API.display;
    surface_info.flags = 0;
    surface_info.pNext = NULL;

    PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR;
    vkCreateXlibSurfaceKHR = (PFN_vkCreateXlibSurfaceKHR) vkGetInstanceProcAddr(_ins, "vkCreateXlibSurfaceKHR");

    return vkCreateXlibSurfaceKHR(_ins, &surface_info, NULL, _surface);
} 


void neko_UpdateWindow(neko_Window *_win) {
    // Set notfiy booleans to false for updating resize update information
    _win->resize_notify = false;
    _neko_UnreleaseKeys();
    
    XPending(_neko_API.display);
    while(QLength(_neko_API.display)) {
        XEvent ev;
        XNextEvent(_neko_API.display, &ev);

        switch(ev.type) {
            case ClientMessage:
                _win->is_running = false;
                return;

            case ConfigureNotify: 
                {
                    XConfigureEvent *xce = &ev.xconfigure;

                    if(xce->width != _win->cwidth || xce->height != _win->cheight) {
                        _win->resize_notify = true;
                        _win->cwidth = xce->width;
                        _win->cheight = xce->height;
                        _win->vc_data.orig_x = _win->cwidth / 2;
                        _win->vc_data.orig_y = _win->cheight / 2;
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
                    _neko_HandleMouseMovement(_win, mev->x, mev->y);
                }
                break;

            default:
                break;
        }
    }

    // Check if the window is used as in OpenGL context
    if(_win->hints & NEKO_HINT_API_OPENGL) {
        glXSwapBuffers(_neko_API.display, _win->x11.window);
    }

    XFlush(_neko_API.display);
}


void neko_UpdateSizeMode(neko_Window *_win, neko_Hint _hints) {
    if(_win->hints & NEKO_HINT_API_OPENGL)
        _win->hints = _hints | NEKO_HINT_API_OPENGL;
    else if(_win->hints & NEKO_HINT_API_VULKAN)
        _win->hints = _hints | NEKO_HINT_API_VULKAN;
    else _win->hints = _hints;

    _neko_UpdateWindowSize(_win);
}


void neko_glMakeCurrent(neko_Window* _win) {
    glXMakeCurrent(_neko_API.display, _win->x11.window, _win->x11.glc);
    _win->x11.drawable = glXGetCurrentDrawable();
}


void neko_DestroyWindow(neko_Window *_win) {
    if(_win->hints & NEKO_HINT_API_OPENGL)
        glXDestroyContext(_neko_API.display, _win->x11.glc);
    XDestroyWindow(_neko_API.display, _win->x11.window);
}


/****************************************/
/****** Input device communication ******/
/****************************************/
void neko_SetMouseCursorMode (
    neko_Window *_win, 
    neko_CursorMode _mouse_mode
) {
    switch(_mouse_mode) {
        case NEKO_CURSOR_MODE_HIDDEN:
            XDefineCursor(_neko_API.display, _win->x11.window, _neko_API.cursors.hidden);
            break;

        case NEKO_CURSOR_MODE_WAITING:
            XDefineCursor(_neko_API.display, _win->x11.window, _neko_API.cursors.waiting);
            break;

        case NEKO_CURSOR_MODE_STANDARD:
            XDefineCursor(_neko_API.display, _win->x11.window, _neko_API.cursors.standard);
            break;

        case NEKO_CURSOR_MODE_POINTER: 
            XDefineCursor(_neko_API.display, _win->x11.window, _neko_API.cursors.pointer);
            break;

        default:
            break;
    }
}


void neko_SetMouseCoords (
    neko_Window *_win,
    uint64_t _x, 
    uint64_t _y
) {
    XWarpPointer(_neko_API.display, 
                 None, 
                 _win->x11.window, 
                 0, 0, 0, 0, _x, _y);
}


void neko_UpdateMousePos(neko_Window *_win) {
    // dummy variables for x11
    Window return_window;
    int win_x, win_y, x, y;
    unsigned int mask;
    XQueryPointer(_neko_API.display, _win->x11.window, &return_window, &return_window, &win_x, &win_y, &x, &y, &mask);
    _neko_HandleMouseMovement(_win, x, y);
}


char **neko_FindRequiredVkExtensionStrings(uint32_t *_ext_c) {
    static char lexts[3][32] = { 0 };
    static char *exts[3] = { lexts[0], lexts[1], lexts[2] };

#ifdef _DEBUG
    *_ext_c = 3;
    strcpy(exts[2], NEKO_VK_DEBUG_UTILS_EXT_NAME);
#else
    *_ext_c = 2;
#endif
    strcpy(exts[0], NEKO_VK_WSI_EXT_NAME);
    strcpy(exts[1], NEKO_VK_XLIB_SURFACE_EXT_NAME);

    return exts;
}
