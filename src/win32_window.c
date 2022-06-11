/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: win32.c - win32 window / surface creation handler header file
/// author: Karl-Mihkel Ott

#define __NWIN_C
#define __WIN32_WINDOW_C
#include <nwin.h>

/*****************************/
/****** Inner functions ******/
/*****************************/
static LRESULT CALLBACK _neko_Win32MessageHandler(
    HWND _handle,
    UINT _msg,
    WPARAM _wparam,
    LPARAM _lparam
) {
    neko_assert(__p_win, "Invalid handle __p_win");
    neko_HidEvent hid_ev = NEKO_HID_UNKNOWN;

    switch (_msg) {
        case WM_CREATE:
            {
                __p_win->win32.handle = _handle;
                neko_Hint hints = __p_win->hints;
                if ((hints & NEKO_HINT_API_OPENGL) == NEKO_HINT_API_OPENGL)
                    _neko_CreateGLContext(__p_win);
            }
            break;

        case WM_CLOSE:
        case WM_DESTROY:
            __p_win->is_running = false;
            return 0;

        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
            hid_ev = _neko_TranslateWIN32Key((uint16_t) _wparam);
            _neko_RegisterKeyEvent(hid_ev, NEKO_INPUT_EVENT_TYPE_ACTIVE);
            break;

        case WM_SYSKEYUP:
        case WM_KEYUP:
        case WM_IME_KEYUP:
            hid_ev = _neko_TranslateWIN32Key((uint16_t) _wparam);
            _neko_RegisterKeyEvent(hid_ev, NEKO_INPUT_EVENT_TYPE_RELEASED);
            break;

        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
            hid_ev = _neko_TranslateWIN32Btn(_msg);
            _neko_RegisterKeyEvent(hid_ev, NEKO_INPUT_EVENT_TYPE_ACTIVE);
            break;

        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
            hid_ev = _neko_TranslateWIN32Btn(_msg);
            _neko_RegisterKeyEvent(hid_ev, NEKO_INPUT_EVENT_TYPE_RELEASED);
            break;

        case WM_MOUSEWHEEL: {
            LONG delta = GET_WHEEL_DELTA_WPARAM(_wparam);
            if (delta > 0) _neko_RegisterKeyEvent(NEKO_MOUSE_SCROLL_UP, NEKO_INPUT_EVENT_TYPE_ACTIVE);
            else _neko_RegisterKeyEvent(NEKO_MOUSE_SCROLL_DOWN, NEKO_INPUT_EVENT_TYPE_ACTIVE);
            break;
        }

        case WM_MOUSEMOVE: 
            {
                POINT pt;
                pt.x = GET_X_LPARAM(_lparam);
                pt.y = GET_Y_LPARAM(_lparam);
                neko_Window* old = __p_win;
                neko_UpdateMousePos(__p_win);
                __p_win = old;
            }
            break;

        case WM_SIZE:
        case WM_MOVE:
            {
                if (!(__p_win->hints & NEKO_HINT_FULL_SCREEN)) {
                    RECT win_rect = { 0 };
                    GetClientRect(_handle, &win_rect);
                    __p_win->cwidth = (int32_t)(win_rect.right - win_rect.left);
                    __p_win->cheight = (int32_t)(win_rect.bottom - win_rect.top);
                    GetWindowRect(_handle, &win_rect);
                    __p_win->cposx = (int32_t)win_rect.left;
                    __p_win->cposy = (int32_t)win_rect.top;
                }
            }
            break;

        default:
            break;
    }

    return DefWindowProc(_handle, _msg, _wparam, _lparam);
}


static DWORD _neko_HandleSizeHints(neko_Window *_win) {
    DWORD hints = 0;

    // Check if no window size hints were given and if so give fixed size hint
    if (!(_win->hints & NEKO_HINT_FIXED_SIZE) && !(_win->hints & NEKO_HINT_RESIZEABLE) && !(_win->hints & NEKO_HINT_FULL_SCREEN)) {
        _win->hints |= NEKO_HINT_FIXED_SIZE;
    }

    if (_win->hints & NEKO_HINT_FIXED_SIZE) {
        hints = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;
    } else if (_win->hints & NEKO_HINT_RESIZEABLE) {
        hints = WS_OVERLAPPEDWINDOW;
    } else if (_win->hints & NEKO_HINT_FULL_SCREEN) {
        hints = WS_POPUP | WS_VISIBLE;
    }

    return hints;
}


static void _neko_CreateGLContext(neko_Window *_win) {
    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    HDC handle_to_device_context = GetDC(_win->win32.handle);
    int px_format = ChoosePixelFormat(handle_to_device_context, &pfd);
    SetPixelFormat(handle_to_device_context, px_format, &pfd);

    _neko_ZeroValueErrorHandler(pcast(ULONG, _win->win32.gl_context = _neko_API.wgl.CreateContext(handle_to_device_context)),
        (ULONG)__LINE__,
        "Failed to create WGL context using wglCreateContext functi on");    
}


static void _neko_ZeroValueErrorHandler(ULONG _val, ULONG _line, const char* _err_msg) {
    if (!_val) {
        DWORD code = GetLastError();
        fprintf(stderr, "Error code: 0x%08lx\n", code);
        fprintf(stderr, "%s:%u; %s\n", __FILE__, _line, _err_msg);
        exit(-1);
    }
}


/*******************************/
/******* NEKO API CALLS ********/
/*******************************/
void neko_InitAPI() {
    // TODO: Load wgl function pointers
    _neko_API.instance = LoadLibraryA("opengl32.dll");
    neko_assert(_neko_API.instance, "Failed to open opengl32.dll");

    _neko_API.wgl.CreateContext = (PFN_wglCreateContext) GetProcAddress(_neko_API.instance, "wglCreateContext");
    _neko_API.wgl.DeleteContext = (PFN_wglDeleteContext) GetProcAddress(_neko_API.instance, "wglDeleteContext");
    _neko_API.wgl.MakeCurrent = (PFN_wglMakeCurrent) GetProcAddress(_neko_API.instance, "wglMakeCurrent");
    _neko_API.wgl.GetProcAddress = (PFN_wglGetProcAddress) GetProcAddress(_neko_API.instance, "wglGetProcAddress");
    _neko_API.wgl.GetCurrentContext = (PFN_wglGetCurrentContext) GetProcAddress(_neko_API.instance, "wglGetCurrentContext");
    _neko_API.wgl.GetCurrentDC = (PFN_wglGetCurrentDC) GetProcAddress(_neko_API.instance, "wglGetCurrentDC");

    _neko_API.cursors.standard = LoadCursorA(NULL, IDC_ARROW);
    _neko_API.cursors.pointer = LoadCursorA(NULL, IDC_HAND);
    _neko_API.cursors.waiting = LoadCursorA(NULL, IDC_WAIT);
    _neko_API.cursors.hidden = NULL;
    _neko_API.is_init = true;
}


bool neko_APIInitStatus() {
    return _neko_API.is_init;
}


void neko_DeinitAPI() {
    DestroyCursor(_neko_API.cursors.standard);
    DestroyCursor(_neko_API.cursors.pointer);
    DestroyCursor(_neko_API.cursors.waiting);
    _neko_API.is_init = false;
}


/// Create new platform independant neko_Window instance for vulkan
neko_Window neko_NewWindow (
    int32_t width,
    int32_t height,
    neko_Hint hints,
    int32_t _spawn_x,
    int32_t _spawn_y,
    const char *_title
) {
    neko_Window win = { 0 };

    win.cwidth = (win.owidth = width);
    win.cheight = (win.oheight = height);
    win.cposx = (win.oposx = _spawn_x);
    win.cposy = (win.oposy = _spawn_y);
    win.hints = hints;
    win.window_title = _title;
    win.vc_data.is_enabled = false;
    win.vc_data.orig_x = width / 2;
    win.vc_data.orig_y = height / 2;

    // Make a class name
    char class_name[1024] = { 0 };
    strcat(class_name, __NEKO_CLASS_NAME);
    strcat(class_name, "__");
    strcat(class_name, _title);

    WNDCLASSA class = { 0 };
    class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    class.lpfnWndProc = _neko_Win32MessageHandler;
    class.hInstance = (win.win32.instance = GetModuleHandleA(NULL));
    class.lpszClassName = class_name;
    class.hCursor = _neko_API.cursors.standard;

    _neko_ZeroValueErrorHandler((ULONG) RegisterClassA(&class), 
                                (ULONG) __LINE__, 
                                "Failed to register neko window class");


    // Check size hints and resize accordingly
    DWORD window_style = _neko_HandleSizeHints(&win);
    __p_win = &win;

    // Check if the parent window pointer is present
    win.win32.handle = CreateWindowExA(0, class_name, win.window_title,
                                       window_style, 
                                       win.oposx, win.oposy,
                                       win.owidth, win.oheight,
                                       NULL, NULL,
                                       win.win32.instance, NULL);

    _neko_ZeroValueErrorHandler(pcast(ULONG, win.win32.handle), (ULONG)__LINE__, "Failed to create win32 window");
    
    // NOTE: Whenever using WIN32 api, you must keep in mind that the created window size is absolute and contains all window decorations as well.
    //       In order to find the actual client are we must call GetClientRect() function
    RECT rect;
    GetClientRect(win.win32.handle, &rect);
    win.cwidth = (int32_t) (rect.right - rect.left);
    win.cheight = (int32_t) (rect.bottom - rect.top);

    // Free all memory allocated for the wide title
    _neko_ZeroValueErrorHandler(pcast(ULONG, win.win32.handle), (ULONG) __LINE__, "Failed get win32 window client rect");

    win.win32.rids[0].usUsagePage = 0x01;
    win.win32.rids[0].usUsage = 0x02;
    win.win32.rids[0].dwFlags = 0;
    win.win32.rids[0].hwndTarget = win.win32.handle;

    win.win32.rids[1].usUsagePage = 0x01;
    win.win32.rids[1].usUsage = 0x06;
    win.win32.rids[1].dwFlags = 0;
    win.win32.rids[1].hwndTarget = win.win32.handle;

    BOOL rid_stat = RegisterRawInputDevices(win.win32.rids, 2, sizeof(win.win32.rids[0]));
    _neko_ZeroValueErrorHandler((ULONG) rid_stat, (ULONG) __LINE__, "Failed to register raw input devices");

    win.mx = 0;
    win.my = 0;

    win.vc_data.orig_x = (int64_t)(win.cwidth / 2);
    win.vc_data.orig_y = (int64_t)(win.cheight / 2);

    win.is_running = true;
    ShowWindow(win.win32.handle, SW_NORMAL);
    __p_win = NULL;
    return win;
}


VkResult neko_InitVkSurface(neko_Window *_win, VkInstance _ins, VkSurfaceKHR *_surface) {
    __p_win = _win;
    VkWin32SurfaceCreateInfoKHR surface_info;
    memset(&surface_info, 0, sizeof(surface_info));
    surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surface_info.hinstance = _win->win32.instance;
    surface_info.hwnd = _win->win32.handle;
    surface_info.pNext = NULL;
    
    PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
    vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR) vkGetInstanceProcAddr(_ins, "vkCreateWin32SurfaceKHR");

    VkResult res = vkCreateWin32SurfaceKHR(_ins, &surface_info, NULL, _surface);
    __p_win = NULL;
    return res;
}


/// Update window events and key arrays
/// This function is meant to be called with every loop iteration 
void neko_UpdateWindow(neko_Window *_win) {
    __p_win = _win;

    // Find the scroll event status
    _neko_UnreleaseKeys();
    _neko_RegisterKeyEvent(NEKO_MOUSE_SCROLL_UP, NEKO_INPUT_EVENT_TYPE_RELEASED);
    _neko_RegisterKeyEvent(NEKO_MOUSE_SCROLL_DOWN, NEKO_INPUT_EVENT_TYPE_RELEASED);

    if(_win->hints & NEKO_HINT_API_OPENGL) {
        HDC dc = GetDC(_win->win32.handle);
        SwapBuffers(dc);
    }

    if (!_win->is_running) {
        __p_win = NULL;
        return;
    }
    
    while(PeekMessageW(&_win->win32.message, _win->win32.handle, 0, 0, PM_REMOVE)) {
        TranslateMessage(&_win->win32.message);
        DispatchMessage(&_win->win32.message);
    }

    __p_win = NULL;
}


/// Set new resettable hints for neko window
void neko_UpdateSizeMode(neko_Window *_win, neko_Hint _hints) {
    __p_win = _win;
    if(_win->hints & NEKO_HINT_API_OPENGL)
        _win->hints = _hints | NEKO_HINT_API_OPENGL;
    else if(_win->hints & NEKO_HINT_API_VULKAN)
        _win->hints = _hints | NEKO_HINT_API_VULKAN;

    DWORD ws = _neko_HandleSizeHints(_win);

    if (_win->hints & NEKO_HINT_FULL_SCREEN) {
        RECT rect;
        GetWindowRect(_win->win32.handle, &rect);
        _win->owidth = rect.right - rect.left;
        _win->oheight = rect.bottom - rect.top;
        _win->oposx = _win->cposx;
        _win->oposy = _win->cposy;
        
        _win->cwidth = GetSystemMetrics(SM_CXSCREEN);
        _win->cheight = GetSystemMetrics(SM_CYSCREEN);
        _win->cposx = 0;
        _win->cposy = 0;
    } else {
        _win->cwidth = _win->owidth;
        _win->cheight = _win->oheight;
        _win->cposx = _win->oposx;
        _win->cposy = _win->oposy;
    }

    MoveWindow(_win->win32.handle, _win->cposx, _win->cposy, _win->cwidth, _win->cheight, TRUE);
    SetWindowLongPtr(_win->win32.handle, GWL_STYLE, ws);
    ShowWindow(_win->win32.handle, SW_NORMAL);
    __p_win = NULL;
}


void neko_glMakeCurrent(neko_Window* _win) {
    HDC handle_to_device_context = GetDC(_win->win32.handle);
    _neko_API.wgl.MakeCurrent(handle_to_device_context, _win->win32.gl_context);
}


/// Destroy window instance and free all resources that were used
void neko_DestroyWindow(neko_Window *_win) {
    __p_win = _win;
    if(_win->hints & NEKO_HINT_API_OPENGL)
        _neko_API.wgl.DeleteContext(_win->win32.gl_context);
    DestroyWindow(_win->win32.handle);
    __p_win = NULL;
}


/// Switch mouse cursor behaviour within the DENG window 
void neko_SetMouseCursorMode (
    neko_Window *_win, 
    neko_CursorMode _cur_mode
) {
    __p_win = _win;
    _win->cursor_mode = _cur_mode;

    // Check if the cursor should be hidden
    if(_cur_mode == NEKO_CURSOR_MODE_HIDDEN)
        ShowCursor(FALSE);
    else ShowCursor(TRUE);

    switch(_cur_mode) {
        case NEKO_CURSOR_MODE_STANDARD:
            SetCursor(_neko_API.cursors.standard);
            break;

        case NEKO_CURSOR_MODE_POINTER:
            SetCursor(_neko_API.cursors.pointer);
            break;

        case NEKO_CURSOR_MODE_WAITING:
            SetCursor(_neko_API.cursors.waiting);
            break;

        case NEKO_CURSOR_MODE_HIDDEN:
            SetCursor(_neko_API.cursors.hidden);
            break;

        default:
            break;
    }
    __p_win = NULL;
}


/// Force mouse cursor to certain location on window
void neko_SetMouseCoords (
    neko_Window *_win, 
    uint64_t _x, 
    uint64_t _y
) {
    __p_win = _win;
    POINT pnt;
    pnt.x = (LONG) _x;
    pnt.y = (LONG) _y;
    ClientToScreen(_win->win32.handle, &pnt);
    SetCursorPos((int) pnt.x, (int) pnt.y);
    __p_win = _win;
}


void neko_UpdateMousePos(neko_Window *_win) {
    __p_win = _win;
    // Query mouse position
    POINT pos;
    GetCursorPos(&pos);
    ScreenToClient(_win->win32.handle, &pos);

    // Check if virtual mouse positioning is enabled
    if(_win->vc_data.is_enabled) {
        int64_t movement_x = ((int64_t) pos.x - _win->vc_data.orig_x);
        int64_t movement_y = ((int64_t) pos.y - _win->vc_data.orig_y);

        // Check if cursor should be set to the origin position
        if(pos.x != (LONG) _win->vc_data.orig_x || pos.y != (LONG) _win->vc_data.orig_y)
            neko_SetMouseCoords(_win, _win->vc_data.orig_x, _win->vc_data.orig_y);

        // Check if overflow is detected on x position
        if (_win->vc_data.x + movement_x >= __max_vc_x) {
            if (__x_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                _win->vc_data.x = __min_vc_x;
        }

        else if (_win->vc_data.x + movement_x < __min_vc_x) {
            if (__x_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                _win->vc_data.x = __max_vc_x;
        }

        else _win->vc_data.x += movement_x;

        // Check if overflow is detected on y position
        if (_win->vc_data.y + movement_y >= __max_vc_y) {
            if (__y_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                _win->vc_data.y = __min_vc_y;
        }

        else if (_win->vc_data.y + movement_y < __min_vc_y) {
            if (__y_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                _win->vc_data.y = __max_vc_y;
        }

        else _win->vc_data.y += movement_y;
    }

    // Check if virtual mouse position initialisation is neccesary
    else {
        _win->mx = (int64_t) pos.x;
        _win->my = (int64_t) pos.y;
    }

    __p_win = NULL;
}


char **neko_FindRequiredVkExtensionStrings(uint32_t *_ext_c) {
    static char lexts[3][32] = { 0 };
    static char *exts[3] = { lexts[0], lexts[1], lexts[2]};

#ifdef _DEBUG
    *_ext_c = 3;
    strcpy(exts[2], NEKO_VK_DEBUG_UTILS_EXT_NAME);
#else
    *_ext_c = 2;
#endif
    strcpy(exts[0], NEKO_VK_WSI_EXT_NAME);
    strcpy(exts[1], NEKO_VK_WIN32_SURFACE_EXT_NAME);
    
    return exts;
}
