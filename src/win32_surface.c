/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: win32.c - win32 window / surface creation handler header file
/// author: Karl-Mihkel Ott

#define __NEKO_SURFACE_C
#include <surface_window.h>


static HCURSOR _old_cursor = NULL;


void neko_InitAPI() {
    // TODO: Load wgl function pointers
    _neko_API.instance = LoadLibraryA("opengl32.dll");
    _neko_API.CreateContext = (PFN_wglCreateContext) GetProcAddress(_neko_API.instance, "wglCreateContext");
    _neko_API.DeleteContext = (PFN_wglDeleteContext) GetProcAddress(_neko_API.instance, "wglDeleteContext");
    _neko_API.MakeCurrent = (PFN_wglMakeCurrent) GetProcAddress(_neko_API.instance, "wglMakeCurrent");
}


/// Create new platform independant neko_Window instance for vulkan
neko_Window *neko_NewWindow (
    int32_t width,
    int32_t height, 
    neko_Hint hints,
    const char *title
) {
    static neko_Window win = { 0 };

    win.cwidth = win.swidth = width;
    win.cheight = win.sheight = height;
    win.hints = hints;
    win.window_title = title;
    win.vc_data.is_enabled = false;
    win.vc_data.orig_x = width / 2;
    win.vc_data.orig_y = height / 2;

    DWORD window_style;
    WNDCLASSEX class = { 0 };
    class.cbSize = sizeof(class);
    class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    class.lpfnWndProc = _neko_Win32MessageHandler;
    class.hInstance = win.win32.instance = GetModuleHandleW(NULL);
    class.lpszClassName = __NEKO_CLASS_NAME;

    _neko_ZeroValueErrorHandler((ULONG) RegisterClassEx(&class), "Failed to register neko window class");


    // Check size hints and resize accordingly
    _neko_HandleSizeHints(&win, &window_style);

    LPWSTR w_title = _neko_CreateWideStringFromUTF8(win.window_title);
    LPWSTR w_cname = _neko_CreateWideStringFromUTF8(__NEKO_CLASS_NAME);
    win.win32.handle = CreateWindowExW(0, w_cname, w_title, 
                                       window_style, 
                                       0, 
                                       0, 
                                       win.cwidth, win.cheight, 
                                       NULL, NULL, 
                                       win.win32.instance, NULL);
                                    
    // Free all memory allocated for the wide title
    free(w_title);
    free(w_cname);
    _neko_ZeroValueErrorHandler((ULONG) win.win32.handle, "Failed to initialise win32 window");

    win.win32.rids[0].usUsagePage = 0x01;
    win.win32.rids[0].usUsage = 0x02;
    win.win32.rids[0].dwFlags = 0;
    win.win32.rids[0].hwndTarget = win.win32.handle;

    win.win32.rids[1].usUsagePage = 0x01;
    win.win32.rids[1].usUsage = 0x06;
    win.win32.rids[1].dwFlags = 0;
    win.win32.rids[1].hwndTarget = win.win32.handle;

    BOOL rid_stat = RegisterRawInputDevices(win.win32.rids, 2, sizeof(win.win32.rids[0]));
    _neko_ZeroValueErrorHandler((ULONG) rid_stat, "Failed to register raw input devices");

    // Create OpenGL context if necessary
    // if(win.hints & NEKO_HINT_API_OPENGL)
    //    _neko_CreateGLContext(&win);

    win.mx = 0;
    win.my = 0;

    win.vc_data.orig_x = (int64_t)(win.cwidth / 2);
    win.vc_data.orig_y = (int64_t)(win.cheight / 2);

	__is_running = true;
    return &win;
}


/// WIN32 messenger callback
static LRESULT CALLBACK _neko_Win32MessageHandler (
    HWND handle, 
    UINT msg, 
    WPARAM wparam, 
    LPARAM lparam
) {
    neko_Key key;
    neko_MouseButton btn;

    switch(msg) {  
        case WM_CREATE:
            _neko_CreateGLContext(handle);
            break;
        case WM_CLOSE:
        case WM_DESTROY:
            __is_running = false;
            return 0;

        case WM_SYSKEYDOWN: 
        case WM_KEYDOWN:
            key = translateWIN32Key((uint16_t) wparam);
            _neko_RegisterKeyEvent (
                key,
                NEKO_MOUSE_BTN_UNKNOWN,
                NEKO_INPUT_TYPE_KB,
                NEKO_INPUT_EVENT_TYPE_ACTIVE
            );
            break;

        case WM_SYSKEYUP: 
        case WM_KEYUP: 
        case WM_IME_KEYUP: 
            key = translateWIN32Key((uint16_t) wparam);
            _neko_RegisterKeyEvent (
                key,
                NEKO_MOUSE_BTN_UNKNOWN,
                NEKO_INPUT_TYPE_KB,
                NEKO_INPUT_EVENT_TYPE_RELEASED
            );
            break;

        case WM_LBUTTONDOWN: 
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
            btn = translateWIN32Btn(msg);
            _neko_RegisterKeyEvent (
                NEKO_KEY_UNKNOWN,
                btn,
                NEKO_INPUT_TYPE_MOUSE,
                NEKO_INPUT_EVENT_TYPE_ACTIVE
            );
            break;

        case WM_LBUTTONUP: 
        case WM_MBUTTONUP: 
        case WM_RBUTTONUP: 
            btn = translateWIN32Btn(msg); _neko_RegisterKeyEvent (
                NEKO_KEY_UNKNOWN,
                btn,
                NEKO_INPUT_TYPE_MOUSE,
                NEKO_INPUT_EVENT_TYPE_RELEASED
            );
            break;
        
        case WM_MOUSEWHEEL: {
            LONG delta = GET_WHEEL_DELTA_WPARAM(wparam);
            if (delta > 0) {
                _neko_RegisterKeyEvent (
                    NEKO_KEY_UNKNOWN,
                    NEKO_MOUSE_SCROLL_UP,
                    NEKO_INPUT_TYPE_MOUSE,
                    NEKO_INPUT_EVENT_TYPE_ACTIVE
                );
            }

            else {
                _neko_RegisterKeyEvent(
                    NEKO_KEY_UNKNOWN,
                    NEKO_MOUSE_SCROLL_DOWN,
                    NEKO_INPUT_TYPE_MOUSE,
                    NEKO_INPUT_EVENT_TYPE_ACTIVE
                );
            }
            break;
        }
    }

    return DefWindowProc(handle, msg, wparam, lparam);
}


static void _neko_HandleSizeHints(neko_Window *win, DWORD *ws) {
    if(win->hints & NEKO_HINT_FIXED_SIZE)
        *ws = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;
    else if(win->hints & NEKO_HINT_RESIZEABLE)
        *ws = WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
    if(win->hints & NEKO_HINT_FULL_SCREEN) {
        win->cwidth = GetSystemMetrics(SM_CXSCREEN);
        win->cheight = GetSystemMetrics(SM_CYSCREEN);
        *ws = WS_POPUP | WS_VISIBLE;
    }
}


static void _neko_CreateGLContext(HWND hWnd) {
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 
        32,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        24,
        8,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    HDC dc = GetDC(hWnd);
    int pixel_format = ChoosePixelFormat(dc, &pfd);
    SetPixelFormat(dc, pixel_format, &pfd);

    HGLRC context = wglCreateContext(dc);
    wglMakeCurrent(dc, context);
}


/// Load all useable cursors
static void _neko_LoadCursors(neko_Window *win) {
    _neko_API.cursors.standard = LoadCursor(NULL, IDC_APPSTARTING);
    _neko_API.cursors.pointer = LoadCursor(NULL, IDC_HAND);
    _neko_API.cursors.waiting = LoadCursor(NULL, IDC_WAIT);
    _neko_API.cursors.hidden = NULL;
}


static WCHAR *_neko_CreateWideStringFromUTF8(const char *str) {
    WCHAR *out_str = (WCHAR*) calloc(strlen(str) + 1, sizeof(WCHAR));
    for(size_t i = 0; i < strlen(str); i++)
        out_str[i] = (WCHAR) str[i];

    return out_str;
}


static void _neko_ZeroValueErrorHandler(ULONG val, const char *err_msg) {
    if(!val) {
        DWORD code = GetLastError();
        fprintf(stderr, "Error code: 0x%lx\n", code);
        neko_assert(val, err_msg);
    }
}


void neko_UpdateSizeMode(neko_Window *win, neko_Hint hints) {
    if(win->hints & NEKO_HINT_API_OPENGL)
        win->hints = hints | NEKO_HINT_API_OPENGL;
    else if(win->hints & NEKO_HINT_API_VULKAN)
        win->hints = hints | NEKO_HINT_API_VULKAN;
    else win->hints = hints;

    DWORD ws;
    _neko_HandleSizeHints(win, &ws);

    // Retrieve information about the current window rectangle
    RECT wrect; 
    neko_assert(GetWindowRect(win->win32.handle, &wrect), "Failed to retrieve information from GetWindowRect()");
    SetWindowLongPtr(win->win32.handle, GWL_STYLE, ws);
    SetWindowPos(win->win32.handle, HWND_TOPMOST, wrect.left, wrect.top, 
                 win->cwidth, win->cheight, SWP_SHOWWINDOW);
}


/// Update window events and key arrays
/// This function is meant to be called with every loop iteration 
void neko_UpdateWindow(neko_Window *p_win) {
    // Find the scroll event status
    bool is_scr_up = neko_FindKeyStatus(
        NEKO_MOUSE_SCROLL_UP,
        NEKO_INPUT_EVENT_TYPE_ACTIVE
    );

    bool is_scr_down = neko_FindKeyStatus (
        NEKO_MOUSE_SCROLL_DOWN,
        NEKO_INPUT_EVENT_TYPE_ACTIVE
    );

    // Check if scroll events have occured
    if (is_scr_up) {
        _neko_RegisterKeyEvent (
            NEKO_KEY_UNKNOWN,
            NEKO_MOUSE_SCROLL_UP,
            NEKO_INPUT_TYPE_MOUSE,
            NEKO_INPUT_EVENT_TYPE_RELEASED
        );
    }

    if (is_scr_down) {
        _neko_RegisterKeyEvent (
            NEKO_KEY_UNKNOWN,
            NEKO_MOUSE_SCROLL_DOWN,
            NEKO_INPUT_TYPE_MOUSE,
            NEKO_INPUT_EVENT_TYPE_RELEASED
        );
    }
    _neko_UnreleaseKeys();
    ShowWindow(p_win->win32.handle, SW_NORMAL);

    if (!__is_running) return;
    while(PeekMessageW(&p_win->win32.message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&p_win->win32.message);
        DispatchMessage(&p_win->win32.message);
    }
}


/// Destroy window instance and free all resources that were used
void neko_DestroyWindow(neko_Window *p_win) {
    if(p_win->hints & NEKO_HINT_API_OPENGL)
        wglDeleteContext(p_win->win32.gl_context);
    DestroyWindow(p_win->win32.handle);
}


/// Check if window is still running and no close events have happened
bool neko_IsRunning() {
    return __is_running;
}

/****************************************/
/****** Input device communication ******/
/****************************************/

/// Switch mouse cursor behaviour within the DENG window 
void neko_SetMouseCursorMode (
    neko_Window *win, 
    neko_CursorMode cur_mode
) {
    win->cursor_mode = cur_mode;
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


/// Set mouse coordinates to certain position
/// This function is used to force set mouse cursor position
void neko_SetMouseCoords (
    neko_Window *p_win, 
    uint64_t x, 
    uint64_t y
) {
    POINT pnt;
    pnt.x = (LONG) x;
    pnt.y = (LONG) y;
    ClientToScreen(p_win->win32.handle, &pnt);
    SetCursorPos((int) pnt.x, (int) pnt.y);
    if(!p_win->vc_data.is_enabled) {
        p_win->mx = (LONG) x;
        p_win->my = (LONG) y;
    }
}


void neko_GetMousePos (
    neko_Window *p_win, 
    bool init_vc
) {
    POINT point;
    
    // Check if GetCursorPos and ScreenToClient calls are successful and update original positions
    if(GetCursorPos(&point) && ScreenToClient(p_win->win32.handle, &point)) {
        p_win->mx = point.x;
        p_win->my = point.y;
    }

    else {
        fprintf(stderr, "Failed to read WIN32 cursor\n");
        exit(EXIT_FAILURE);
    }
    
    // Check if virtual mouse positioning is enabled
    if(p_win->vc_data.is_enabled && !init_vc) {
        uint64_t movement_x = (point.x - p_win->vc_data.orig_x);
        uint64_t movement_y = (point.y - p_win->vc_data.orig_y);

        // Check if cursor should be set to the origin position
        if(point.x  != (LONG) p_win->vc_data.orig_x || point.y  != (LONG) p_win->vc_data.orig_y) {
            neko_SetMouseCoords (
                p_win, 
                p_win->vc_data.orig_x, 
                p_win->vc_data.orig_y
            );
        }

        // Check if overflow is detected on x position
        if (p_win->vc_data.x + movement_x >= __max_vc_x) {
            if (__x_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                p_win->vc_data.x = __min_vc_x;
        }

        else if (p_win->vc_data.x + movement_x < __min_vc_x) {
            if (__x_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                p_win->vc_data.x = __max_vc_x;
        }

        else p_win->vc_data.x += movement_x;

        // Check if overflow is detected on y position
        if (p_win->vc_data.y + movement_y >= __max_vc_y) {
            if (__y_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                p_win->vc_data.y = __min_vc_y;
        }

        else if (p_win->vc_data.y + movement_y < __min_vc_y) {
            if (__y_overflow_act == NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                p_win->vc_data.y = __max_vc_y;
        }

        else p_win->vc_data.y += movement_y;
    }

    // Check if virtual mouse position initialisation is neccesary
    else if(init_vc) {
        p_win->mx = p_win->vc_data.orig_x;
        p_win->my = p_win->vc_data.orig_y;

        neko_SetMouseCoords (
            p_win, 
            p_win->mx, 
            p_win->my
        );
    }
}


/// Limit the largest and smallest virtual cursor position that can be achieved using 
/// virtual mouse positioning
void neko_LimitVirtualPos(
    uint64_t max_x,
    uint64_t min_x,
    uint64_t max_y,
    uint64_t min_y
) {
    __min_vc_x = min_x;
    __max_vc_x = max_x;
    __min_vc_y = min_y;
    __max_vc_y = max_y;
}


/// Set virtual mouse position overflow actions that specify what
/// should happen if virtual mouse position limit has been reached
void neko_SetOverflowAction(
    neko_VCPOverflowAction x_overflow_act,
    neko_VCPOverflowAction y_overflow_act
) {
    __x_overflow_act = x_overflow_act;
    __y_overflow_act = y_overflow_act;
}
