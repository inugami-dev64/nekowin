/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: win32.c - win32 window / surface creation handler header file
/// author: Karl-Mihkel Ott

#define __NWIN_C
#define __WIN32_WINDOW_C
#include <nwin.h>


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
    const char *title
) {
    neko_assert(wslot_reserved + 1 >= __MAX_WSLOT_C, "There are no free window slots available");
    neko_Window win = (wslot_reserved++);
    __active_win = win;

    wslots[win].owidth = width;
    wslots[win].oheight = height;
    wslots[win].hints = hints;
    wslots[win].window_title = title;
    wslots[win].vc_data.is_enabled = false;
    wslots[win].vc_data.orig_x = width / 2;
    wslots[win].vc_data.orig_y = height / 2;

    DWORD window_style;
    WNDCLASSA class = { 0 };
    class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    class.lpfnWndProc = _neko_Win32MessageHandler;
    class.hInstance = wslots[win].win32.instance = GetModuleHandleA(NULL);
    class.lpszClassName = __NEKO_CLASS_NAME;

    _neko_ZeroValueErrorHandler((ULONG) RegisterClassA(&class), 
                                (ULONG) __LINE__, 
                                "Failed to register neko window class");


    // Check size hints and resize accordingly
    _neko_HandleSizeHints(win, &window_style);
    wslots[win].owidth = wslots[win].cwidth;
    wslots[win].oheight = wslots[win].cheight;

    wslots[win].win32.handle = CreateWindowExA(0, __NEKO_CLASS_NAME, wslots[win].window_title,
                                               window_style,
                                               0, 
                                               0, 
                                               wslots[win].owidth, wslots[win].oheight, 
                                               NULL, NULL, 
                                               wslots[win].win32.instance, NULL);

    _neko_ZeroValueErrorHandler(pcast(ULONG, wslots[win].win32.handle), (ULONG)__LINE__, "Failed to create win32 window");
    // NOTE: Whenever using WIN32 api, you must keep in mind that the created window size is absolute and contains all window decorations as well.
    //       In order to find the actual client are we must call GetClientRect() function
    RECT rect;
    GetClientRect(wslots[win].win32.handle, &rect);
    wslots[win].cwidth = (int32_t) (rect.right - rect.left);
    wslots[win].cheight = (int32_t) (rect.bottom - rect.top);


    __handles[win] = wslots[win].win32.handle;
    __handle_c++;
                                    
    // Free all memory allocated for the wide title
    _neko_ZeroValueErrorHandler(pcast(ULONG, wslots[win].win32.handle), (ULONG) __LINE__, "Failed get win32 window client rect");

    wslots[win].win32.rids[0].usUsagePage = 0x01;
    wslots[win].win32.rids[0].usUsage = 0x02;
    wslots[win].win32.rids[0].dwFlags = 0;
    wslots[win].win32.rids[0].hwndTarget = wslots[win].win32.handle;

    wslots[win].win32.rids[1].usUsagePage = 0x01;
    wslots[win].win32.rids[1].usUsage = 0x06;
    wslots[win].win32.rids[1].dwFlags = 0;
    wslots[win].win32.rids[1].hwndTarget = wslots[win].win32.handle;

    BOOL rid_stat = RegisterRawInputDevices(wslots[win].win32.rids, 2, sizeof(wslots[win].win32.rids[0]));
    _neko_ZeroValueErrorHandler((ULONG) rid_stat, (ULONG) __LINE__, "Failed to register raw input devices");

    wslots[win].mx = 0;
    wslots[win].my = 0;

    wslots[win].vc_data.orig_x = (int64_t)(wslots[win].cwidth / 2);
    wslots[win].vc_data.orig_y = (int64_t)(wslots[win].cheight / 2);

    wslots[win].is_running = true;
    ShowWindow(wslots[win].win32.handle, SW_NORMAL);
    return win;
}


VkResult neko_InitVKSurface(neko_Window win, VkInstance i, VkSurfaceKHR *s) {
    VkWin32SurfaceCreateInfoKHR surface_info;
    memset(&surface_info, 0, sizeof(surface_info));
    surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surface_info.hinstance = wslots[win].win32.instance;
    surface_info.hwnd = wslots[win].win32.handle;
    surface_info.pNext = NULL;
    
    PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
    vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR) vkGetInstanceProcAddr(i, "vkCreateWin32SurfaceKHR");

    return vkCreateWin32SurfaceKHR(i, &surface_info, NULL, s);
}


/// Update window events and key arrays
/// This function is meant to be called with every loop iteration 
void neko_UpdateWindow(neko_Window win) {
    // Find the scroll event status
    _neko_UnreleaseKeys();
    _neko_RegisterKeyEvent(NEKO_MOUSE_SCROLL_UP, NEKO_INPUT_EVENT_TYPE_RELEASED);
    _neko_RegisterKeyEvent(NEKO_MOUSE_SCROLL_DOWN, NEKO_INPUT_EVENT_TYPE_RELEASED);

    if(wslots[win].hints & NEKO_HINT_API_OPENGL) {
        HDC dc = GetDC(wslots[win].win32.handle);
        SwapBuffers(dc);
    }

    if (!wslots[win].is_running) return;
    
    while(PeekMessageW(&wslots[win].win32.message, wslots[win].win32.handle, 0, 0, PM_REMOVE)) {
        TranslateMessage(&wslots[win].win32.message);
        DispatchMessage(&wslots[win].win32.message);
    }
}


/// Set new resettable hints for neko window
void neko_UpdateSizeMode(neko_Window win, neko_Hint hints) {
    if(wslots[win].hints & NEKO_HINT_API_OPENGL)
        wslots[win].hints = hints | NEKO_HINT_API_OPENGL;
    else if(wslots[win].hints & NEKO_HINT_API_VULKAN)
        wslots[win].hints = hints | NEKO_HINT_API_VULKAN;

    DWORD ws;
    _neko_HandleSizeHints(win, &ws);

    if(wslots[win].hints & NEKO_HINT_FULL_SCREEN)
        MoveWindow(wslots[win].win32.handle, 0, 0, wslots[win].cwidth, wslots[win].cheight, TRUE);
    else {
        MoveWindow(wslots[win].win32.handle, wslots[win].cposx, wslots[win].cposy, wslots[win].cwidth, wslots[win].cheight, TRUE);
    }
    SetWindowLongPtr(wslots[win].win32.handle, GWL_STYLE, ws);
    ShowWindow(wslots[win].win32.handle, SW_NORMAL);
}


/// Destroy window instance and free all resources that were used
void neko_DestroyWindow(neko_Window win) {
    if(wslots[win].hints & NEKO_HINT_API_OPENGL)
        _neko_API.wgl.DeleteContext(wslots[win].win32.gl_context);
    DestroyWindow(wslots[win].win32.handle);
}


/// Check if window is still running and no close events have happened
bool neko_IsRunning(neko_Window win) {
    return wslots[win].is_running;
}


/// Switch mouse cursor behaviour within the DENG window 
void neko_SetMouseCursorMode (
    neko_Window win, 
    neko_CursorMode cur_mode
) {
    wslots[win].cursor_mode = cur_mode;

    // Check if the cursor should be hidden
    if(cur_mode == NEKO_CURSOR_MODE_HIDDEN)
        ShowCursor(FALSE);
    else ShowCursor(TRUE);

    switch(cur_mode) {
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
}


/// Force mouse cursor to certain location on window
void neko_SetMouseCoords (
    neko_Window win, 
    uint64_t x, 
    uint64_t y
) {
    POINT pnt;
    pnt.x = (LONG) x;
    pnt.y = (LONG) y;
    ClientToScreen(wslots[win].win32.handle, &pnt);
    SetCursorPos((int) pnt.x, (int) pnt.y);
}


void neko_UpdateMousePos(neko_Window win) {
    // Query mouse position
    POINT pos;
    GetCursorPos(&pos);
    ScreenToClient(wslots[win].win32.handle, &pos);

    // Check if virtual mouse positioning is enabled
    if(wslots[win].vc_data.is_enabled) {
        int64_t movement_x = ((int64_t) pos.x - wslots[win].vc_data.orig_x);
        int64_t movement_y = ((int64_t) pos.y - wslots[win].vc_data.orig_y);

        // Check if cursor should be set to the origin position
        if(pos.x != (LONG) wslots[win].vc_data.orig_x || pos.y != (LONG) wslots[win].vc_data.orig_y)
            neko_SetMouseCoords(win, wslots[win].vc_data.orig_x, wslots[win].vc_data.orig_y);

        // Check if overflow is detected on x position
        if (wslots[win].vc_data.x + movement_x >= __max_vc_x) {
            if (__x_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                wslots[win].vc_data.x = __min_vc_x;
        }

        else if (wslots[win].vc_data.x + movement_x < __min_vc_x) {
            if (__x_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                wslots[win].vc_data.x = __max_vc_x;
        }

        else wslots[win].vc_data.x += movement_x;

        // Check if overflow is detected on y position
        if (wslots[win].vc_data.y + movement_y >= __max_vc_y) {
            if (__y_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                wslots[win].vc_data.y = __min_vc_y;
        }

        else if (wslots[win].vc_data.y + movement_y < __min_vc_y) {
            if (__y_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                wslots[win].vc_data.y = __max_vc_y;
        }

        else wslots[win].vc_data.y += movement_y;
    }

    // Check if virtual mouse position initialisation is neccesary
    else {
        wslots[win].mx = (int64_t) pos.x;
        wslots[win].my = (int64_t) pos.y;
    }
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


/*****************************/
/****** Inner functions ******/
/*****************************/


/// WIN32 messenger callback
static LRESULT CALLBACK _neko_Win32MessageHandler (
    HWND handle, 
    UINT msg, 
    WPARAM wparam, 
    LPARAM lparam
) {
    neko_HidEvent hid_ev = NEKO_HID_UNKNOWN;
    neko_Window wid;

    switch(msg) {
        case WM_CREATE:
            {
                wid = __active_win;
                wslots[wid].win32.handle = handle;
                neko_Hint hints = wslots[wid].hints;
                _neko_ZeroValueErrorHandler(wid != UINT32_MAX, __LINE__, "Invalid handle");
                if ((hints & NEKO_HINT_API_OPENGL) == NEKO_HINT_API_OPENGL)
                    _neko_CreateGLContext(wid);
                }
            break;

        case WM_CLOSE:
        case WM_DESTROY:
            wid = _neko_FindWindowIndexFromHandle(handle);
            if (wid != UINT32_MAX)
                wslots[wid].is_running = false;
            return 0;

        case WM_SYSKEYDOWN: 
        case WM_KEYDOWN:
            hid_ev = translateWIN32Key((uint16_t) wparam);
            _neko_RegisterKeyEvent(hid_ev, NEKO_INPUT_EVENT_TYPE_ACTIVE);
            break;

        case WM_SYSKEYUP: 
        case WM_KEYUP: 
        case WM_IME_KEYUP: 
            hid_ev = translateWIN32Key((uint16_t) wparam);
            _neko_RegisterKeyEvent(hid_ev, NEKO_INPUT_EVENT_TYPE_RELEASED);
            break;

        case WM_LBUTTONDOWN: 
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
            hid_ev = translateWIN32Btn(msg);
            _neko_RegisterKeyEvent(hid_ev, NEKO_INPUT_EVENT_TYPE_ACTIVE);
            break;

        case WM_LBUTTONUP: 
        case WM_MBUTTONUP: 
        case WM_RBUTTONUP: 
            hid_ev = translateWIN32Btn(msg); 
            _neko_RegisterKeyEvent(hid_ev, NEKO_INPUT_EVENT_TYPE_RELEASED);
            break;
        
        case WM_MOUSEWHEEL: {
            LONG delta = GET_WHEEL_DELTA_WPARAM(wparam);
            if (delta > 0) _neko_RegisterKeyEvent(NEKO_MOUSE_SCROLL_UP, NEKO_INPUT_EVENT_TYPE_ACTIVE);
            else _neko_RegisterKeyEvent(NEKO_MOUSE_SCROLL_DOWN, NEKO_INPUT_EVENT_TYPE_ACTIVE);
            break;
        }

        case WM_MOUSEMOVE: {
            wid = _neko_FindWindowIndexFromHandle(handle);
            POINT pt;
            pt.x = GET_X_LPARAM(lparam);
            pt.y = GET_Y_LPARAM(lparam);
            neko_UpdateMousePos(wid);
            break;
        }

        case WM_SIZE:
        case WM_MOVE:
            {
                if (!_full_screen_ev) {
                    RECT win_rect = { 0 };
                    GetClientRect(handle, &win_rect);
                    wid = _neko_FindWindowIndexFromHandle(handle);
                    if (wid != UINT32_MAX) {
                        wslots[wid].oposx = wslots[wid].cposx;
                        wslots[wid].oposy = wslots[wid].cposy;
                        wslots[wid].cwidth = (int32_t)(win_rect.right - win_rect.left);
                        wslots[wid].cheight = (int32_t)(win_rect.bottom - win_rect.top);
                        wslots[wid].cposx = (int32_t)win_rect.left;
                        wslots[wid].cposy = (int32_t)win_rect.top;
                    }
                }
            }
            break;

        default:
            break;
    }

    return DefWindowProc(handle, msg, wparam, lparam);
}


static void _neko_HandleSizeHints(neko_Window win, DWORD *ws) {
    HDC hdc = GetDC(wslots[win].win32.handle);

    // Check if no window size hints were given and if so give fixed size hint
    if (!(wslots[win].hints & NEKO_HINT_FIXED_SIZE) && !(wslots[win].hints & NEKO_HINT_RESIZEABLE) && !(wslots[win].hints & NEKO_HINT_FULL_SCREEN)) {
        wslots[win].hints |= NEKO_HINT_FIXED_SIZE;
        _full_screen_ev = false;
    }

    if(wslots[win].hints & NEKO_HINT_FIXED_SIZE) {
        _full_screen_ev = false;
        *ws = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;
        wslots[win].cwidth = wslots[win].owidth;
        wslots[win].cheight = wslots[win].cheight;
    }
    else if(wslots[win].hints & NEKO_HINT_RESIZEABLE) {
        _full_screen_ev = false;
        *ws = WS_OVERLAPPEDWINDOW;
        wslots[win].cwidth = wslots[win].owidth;
        wslots[win].cheight = wslots[win].oheight;
        _neko_API.wgl.MakeCurrent(hdc, wslots[win].win32.gl_context);
    }
    if(wslots[win].hints & NEKO_HINT_FULL_SCREEN) {
        _full_screen_ev = true;
        wslots[win].cwidth = GetSystemMetrics(SM_CXSCREEN);
        wslots[win].cheight = GetSystemMetrics(SM_CYSCREEN);
        *ws = WS_POPUP | WS_VISIBLE;
    }
}


static void _neko_CreateGLContext(neko_Window win) {
    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    HDC handle_to_device_context = GetDC(wslots[win].win32.handle);
    int px_format = ChoosePixelFormat(handle_to_device_context, &pfd);
    SetPixelFormat(handle_to_device_context, px_format, &pfd);

    _neko_ZeroValueErrorHandler(pcast(ULONG, wslots[win].win32.gl_context = _neko_API.wgl.CreateContext(handle_to_device_context)),
                                (ULONG) __LINE__,
                                "Failed to create WGL context using wglCreateContext functi on");
    _neko_API.wgl.MakeCurrent(handle_to_device_context, wslots[win].win32.gl_context);
}



static void _neko_ZeroValueErrorHandler(ULONG val, ULONG line, const char *err_msg) {
    if(!val) {
        DWORD code = GetLastError();
        fprintf(stderr, "Error code: 0x%08lx\n", code);
        fprintf(stderr, "%s:%u; %s\n", __FILE__, line, err_msg);
        exit(-1);
    }
}


static uint32_t _neko_FindWindowIndexFromHandle(HWND handle) {
    if (handle) {
        for (uint32_t i = 0; i < __handle_c; i++) {
            if (__handles[i] == handle)
                return i;
        }
    }

    return UINT32_MAX;
}
