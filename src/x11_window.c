/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: x11_surface.c - x11 window / surface creation handler source file
/// author: Karl-Mihkel Ott

#define __NWIN_C
#include <nwin.h>


/*******************************/
/******* NEKO API CALLS ********/
/*******************************/

/// Initialise platform dependent backend api for nekowin library
void neko_InitAPI() {
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


void neko_DeinitAPI() {
    XFreeCursor(_neko_API.display, _neko_API.cursors.standard);
    XFreeCursor(_neko_API.display, _neko_API.cursors.hidden);
    XFreeCursor(_neko_API.display, _neko_API.cursors.pointer);
    XFreeCursor(_neko_API.display, _neko_API.cursors.waiting);
    XCloseDisplay(_neko_API.display);
}


neko_Window neko_NewWindow (
    int32_t width, 
    int32_t height, 
    neko_Hint hints,
    const char *title
) {
    neko_assert(_neko_API.is_init, "Please initialise neko library with neko_InitAPI() before creating new windows");
    neko_assert(wslot_reserved + 1 >= _MAX_WSLOT_C, "There are no free window slots available");
    neko_Window win = (wslot_reserved++);

    // Fill the window structure
    wslots[win].swidth = wslots[win].cwidth = width;
    wslots[win].sheight = wslots[win].cheight = height;
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

    __is_running = true;
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
    // Check for exit event
    if(XCheckTypedWindowEvent(_neko_API.display, wslots[win].x11.window, 
       ClientMessage, &_neko_API.fr_ev)) {
        __is_running = false;
        return;
    }

    _neko_UnreleaseKeys();
    
    if(XCheckWindowEvent(_neko_API.display, wslots[win].x11.window, 
       KeyPressMask | KeyReleaseMask, &_neko_API.fr_ev)) 
        _neko_HandleKeyEvents(win);
    
    if(XCheckWindowEvent(_neko_API.display, wslots[win].x11.window, 
       ButtonPressMask | ButtonReleaseMask, &_neko_API.fr_ev)) 
        _neko_HandleMouseEvents(win);

    XWindowAttributes attribs;
    XGetWindowAttributes(_neko_API.display, wslots[win].x11.window, &attribs);
    wslots[win].cwidth = attribs.width;
    wslots[win].cheight = attribs.height;
    neko_UpdateMousePos(win);


    // Check if the window is used as in OpenGL context
    if(wslots[win].hints & NEKO_HINT_API_OPENGL)
        glXSwapBuffers(_neko_API.display, wslots[win].x11.window);
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
    XDestroyWindow(_neko_API.display, wslots[win].x11.window);
}


/// Check if window is still running and no close events have happened
bool neko_IsRunning() {
    return __is_running;
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
    XQueryPointer(_neko_API.display, wslots[win].x11.window, &return_window, &return_window, 
        &win_x, &win_y, &x, &y, &mask);

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


void neko_FindRequiredVkExtensionsStrings(char ***p_exts, size_t *ext_s, bool is_validation_layer) {
    static char *sptr[3] = { 0 };
    static char lexts[3][32] = { 0 };

    *p_exts = sptr;
    (*p_exts)[0] = lexts[0];
    (*p_exts)[1] = lexts[1];
    (*p_exts)[2] = lexts[2];

    if(is_validation_layer) {
        *ext_s = 3;
        strcpy((*p_exts)[2], NEKO_VK_DEBUG_UTILS_EXT_NAME);
    }

    else *ext_s = 2;
    strcpy((*p_exts)[0], NEKO_VK_WSI_EXT_NAME);
    strcpy((*p_exts)[1], NEKO_VK_XLIB_SURFACE_EXT_NAME);
}


/*****************************/
/****** Inner functions ******/
/*****************************/

/// Unlike WIN32 api X11 doesn't have a callback system on events, which
/// means that key events must be checked manually on every frame update 
static void _neko_HandleKeyEvents(neko_Window win) {
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
static void _neko_HandleMouseEvents(neko_Window win) {
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
static void _neko_HandleResize(neko_Window win) {
    wslots[win].cwidth = _neko_API.fr_ev.xconfigure.width;
    wslots[win].cheight = _neko_API.fr_ev.xconfigure.height;
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

    XSendEvent(_neko_API.display, _neko_API.root, False, 
        SubstructureNotifyMask | SubstructureRedirectMask, &ev);
}


static void _neko_UpdateWindowSize(neko_Window win) {
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
        size_hints.min_width = size_hints.max_width = wslots[win].swidth;
        size_hints.min_height = size_hints.max_height = wslots[win].sheight;   
        
        // Submit size hints
        XSetWMNormalHints(_neko_API.display, wslots[win].x11.window, &size_hints);
    }

    else if(wslots[win].hints & NEKO_HINT_RESIZEABLE) {
        XSizeHints size_hints = { 0 };

        // Submit size hints
        XSetWMNormalHints(_neko_API.display, wslots[win].x11.window, &size_hints);
        XResizeWindow(_neko_API.display, wslots[win].x11.window, wslots[win].swidth, wslots[win].sheight);
    }
}


static void _neko_CreateGLContext(neko_Window win) {
    GLXContext glc = glXCreateContext(_neko_API.display, wslots[win].x11.p_vi, NULL, GL_TRUE);
    glXMakeCurrent(_neko_API.display, wslots[win].x11.window, glc);
}


/// Load all available platform specific nekowin cursors
static void _neko_LoadCursors(neko_Window win) {
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
}