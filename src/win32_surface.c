/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: win32_handler.c - win32 window / surface creation handler header file
/// author: Karl-Mihkel Ott

#define __NEKO_SURFACE_C
#include <surface_window.h>


static HCURSOR __old_cursor = NULL;


/// Create new platform independant neko_SurfaceWindow instance for vulkan
neko_SurfaceWindow *neko_InitVKSurfaceWindow (
    int32_t width,
    int32_t height, 
    char *title
) {
    static neko_SurfaceWindow win = { 0 };
    // printf("Title: %s\n", title);
    // wchar_t *win_title = cm_ToWideString(title);
    // printf("title: 0x%p\n", title);
    // printf("win_title: 0x%p\n", win_title);
	wchar_t *w_title = L"Test title";


    win.width = width;
    win.height = height;
    win.window_title = title;
    win.vc_data.is_enabled = false;
    win.vc_data.orig_x = width / 2;
    win.vc_data.orig_y = height / 2;

    win.win32_handler.win.hInstance = GetModuleHandle(NULL);
    win.win32_handler.win.lpszClassName = DENG_WIN32_CLASS_NAME;
    win.win32_handler.win.hCursor = NULL;
    win.win32_handler.win.hbrBackground = (HBRUSH) COLOR_WINDOW;
    win.win32_handler.win.lpfnWndProc = __neko_Win32MessageHandler;
    
    RegisterClass(&win.win32_handler.win);
    LPCTSTR win_class = DENG_WIN32_CLASS_NAME;
    DWORD window_style;

    switch (window_mode)
    {
    case DENG_WINDOW_MODE_FIXED:
        window_style = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;
        break;

    case DENG_WINDOW_MODE_FULL_SCREEN:
        win.width = GetSystemMetrics(SM_CXSCREEN);
        win.height = GetSystemMetrics(SM_CYSCREEN);
        window_style = WS_POPUP | WS_VISIBLE;
        break;

    default:
        return NULL;
        break;
    }
    
    win.win32_handler.hwnd = CreateWindowEx(0, win_class, w_title, window_style, CW_USEDEFAULT, 
        CW_USEDEFAULT, win.width, win.height, NULL, NULL, win.win32_handler.win.hInstance, NULL );

    if(!win.win32_handler.hwnd) {
        printf("%s\n", "Failed to initialize win32 window!");
        exit(EXIT_FAILURE);
    }

    win.win32_handler.rids[0].usUsagePage = 0x01;
    win.win32_handler.rids[0].usUsage = 0x02;
    win.win32_handler.rids[0].dwFlags = 0;
    win.win32_handler.rids[0].hwndTarget = win.win32_handler.hwnd;

    win.win32_handler.rids[1].usUsagePage = 0x01;
    win.win32_handler.rids[1].usUsage = 0x06;
    win.win32_handler.rids[1].dwFlags = 0;
    win.win32_handler.rids[1].hwndTarget = win.win32_handler.hwnd;

    if(!RegisterRawInputDevices(win.win32_handler.rids, 2, sizeof(win.win32_handler.rids[0]))) {
        printf("%ld\n", GetLastError());
        fflush(stdout);
        perror("Failed to register rid devices!");
    }

    win.mx = 0;
    win.my = 0;

    RECT size_rect;
    GetClientRect(win.win32_handler.hwnd, &size_rect);
    win.width = size_rect.right - size_rect.left;
    win.height = size_rect.bottom - size_rect.top;
    win.mode = WIN32_WINDOW;

    win.vc_data.orig_x = (int64_t)(win.width / 2);
    win.vc_data.orig_y = (int64_t)(win.height / 2);

	__is_running = true;

    return &win;
}


/// WIN32 messenger callback
static LRESULT CALLBACK __neko_Win32MessageHandler (
   HWND hwnd, 
   UINT msg, 
   WPARAM wparam, 
   LPARAM lparam
) {
    neko_Key key;
    neko_MouseButton btn;


    switch(msg) {  
        case WM_CLOSE:
        case WM_DESTROY:
            __is_running = false;
            return 0;

        case WM_SYSKEYDOWN: 
        case WM_KEYDOWN:
            key = translateWIN32Key((uint16_t) wparam);
            __neko_RegisterKeyEvent (
                key,
                DENG_MOUSE_BTN_UNKNOWN,
                DENG_INPUT_TYPE_KB,
                DENG_INPUT_EVENT_TYPE_ACTIVE
            );
            break;

        case WM_SYSKEYUP: 
        case WM_KEYUP: 
        case WM_IME_KEYUP: 
            key = translateWIN32Key((uint16_t) wparam);
            __neko_RegisterKeyEvent (
                key,
                DENG_MOUSE_BTN_UNKNOWN,
                DENG_INPUT_TYPE_KB,
                DENG_INPUT_EVENT_TYPE_RELEASED
            );
            break;

        case WM_LBUTTONDOWN: 
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
            btn = translateWIN32Btn(msg);
            __neko_RegisterKeyEvent (
                DENG_KEY_UNKNOWN,
                btn,
                DENG_INPUT_TYPE_MOUSE,
                DENG_INPUT_EVENT_TYPE_ACTIVE
            );
            break;

        case WM_LBUTTONUP: 
        case WM_MBUTTONUP: 
        case WM_RBUTTONUP: 
            btn = translateWIN32Btn(msg);
            __neko_RegisterKeyEvent (
                DENG_KEY_UNKNOWN,
                btn,
                DENG_INPUT_TYPE_MOUSE,
                DENG_INPUT_EVENT_TYPE_RELEASED
            );
            break;
        
        case WM_MOUSEWHEEL: {
            LONG delta = GET_WHEEL_DELTA_WPARAM(wparam);
            if (delta > 0) {
                __neko_RegisterKeyEvent (
                    DENG_KEY_UNKNOWN,
                    DENG_MOUSE_SCROLL_UP,
                    DENG_INPUT_TYPE_MOUSE,
                    DENG_INPUT_EVENT_TYPE_ACTIVE
                );
            }

            else {
                __neko_RegisterKeyEvent(
                    DENG_KEY_UNKNOWN,
                    DENG_MOUSE_SCROLL_DOWN,
                    DENG_INPUT_TYPE_MOUSE,
                    DENG_INPUT_EVENT_TYPE_ACTIVE
                );
            }
            break;
        }
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}


/// Update window events and key arrays
/// This function is meant to be called with every loop iteration 
void neko_UpdateWindow(neko_SurfaceWindow *p_win) {
    // Find the scroll event status
    neko_bool_t is_scr_up = neko_FindKeyStatus(
        DENG_KEY_UNKNOWN,
        DENG_MOUSE_SCROLL_UP,
        DENG_INPUT_TYPE_MOUSE,
        DENG_INPUT_EVENT_TYPE_ACTIVE
    );

    neko_bool_t is_scr_down = neko_FindKeyStatus(
        DENG_KEY_UNKNOWN,
        DENG_MOUSE_SCROLL_DOWN,
        DENG_INPUT_TYPE_MOUSE,
        DENG_INPUT_EVENT_TYPE_ACTIVE
    );

    // Check if scroll events have occured
    if (is_scr_up) {
        __neko_RegisterKeyEvent (
            DENG_KEY_UNKNOWN,
            DENG_MOUSE_SCROLL_UP,
            DENG_INPUT_TYPE_MOUSE,
            DENG_INPUT_EVENT_TYPE_RELEASED
        );
    }

    if (is_scr_down) {
        __neko_RegisterKeyEvent (
            DENG_KEY_UNKNOWN,
            DENG_MOUSE_SCROLL_DOWN,
            DENG_INPUT_TYPE_MOUSE,
            DENG_INPUT_EVENT_TYPE_RELEASED
        );
    }
    __neko_UnreleaseKeys();
    ShowWindow(p_win->win32_handler.hwnd, SW_NORMAL);

    if (__mouse_mode == DENG_MOUSE_MODE_INVISIBLE) SetCursor(0);
    if (!__is_running) return;
    while(PeekMessageW(&p_win->win32_handler.message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&p_win->win32_handler.message);
        DispatchMessage(&p_win->win32_handler.message);
    }
}


/// Destroy window instance and free all resources that were used
void neko_DestroyWindow(neko_SurfaceWindow *p_win) {
    DestroyWindow(p_win->win32_handler.hwnd);
}


/// Check if window is still running and no close events have happened
neko_bool_t neko_IsRunning() {
    return __is_running;
}

/****************************************/
/****** Input device communication ******/
/****************************************/

/// Switch mouse cursor behaviour within the DENG window 
void neko_SetMouseCursorMode (
    neko_SurfaceWindow *p_win, 
    neko_MouseMode mouse_mode
) {
    __mouse_mode = mouse_mode;
    switch(mouse_mode) {
    case DENG_MOUSE_MODE_INVISIBLE:
        __old_cursor = SetCursor(0);
        neko_GetMousePos(p_win, true);
        p_win->win32_handler.win.hCursor = NULL;

        p_win->vc_data.is_enabled = true;
        break;

    case DENG_MOUSE_MODE_CURSOR_VISIBLE:
        SetCursor(__old_cursor);
        p_win->vc_data.is_enabled = false;
        break;
    }
}


/// Set mouse coordinates to certain position
/// This function is used to force set mouse cursor position
void neko_SetMouseCoords (
    neko_SurfaceWindow *p_win, 
    neko_px_t x, 
    neko_px_t y
) {
    POINT pnt;
    pnt.x = (LONG) x;
    pnt.y = (LONG) y;
    ClientToScreen(p_win->win32_handler.hwnd, &pnt);
    SetCursorPos((int) pnt.x, (int) pnt.y);
    if(!p_win->vc_data.is_enabled) {
        p_win->mx = (LONG) x;
        p_win->my = (LONG) y;
    }
}


void neko_GetMousePos (
    neko_SurfaceWindow *p_win, 
    neko_bool_t init_vc
) {
    POINT point;
    
    // Check if GetCursorPos and ScreenToClient calls are successful and update original positions
    if(GetCursorPos(&point) && ScreenToClient(p_win->win32_handler.hwnd, &point)) {
        p_win->mx = point.x;
        p_win->my = point.y;
    }

    else {
        fprintf(stderr, "Failed to read WIN32 cursor\n");
        exit(EXIT_FAILURE);
    }
    
    // Check if virtual mouse positioning is enabled
    if(p_win->vc_data.is_enabled && !init_vc) {
        neko_px_t movement_x = (point.x - p_win->vc_data.orig_x);
        neko_px_t movement_y = (point.y - p_win->vc_data.orig_y);

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
            if (__x_overflow_act == DENG_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                p_win->vc_data.x = __min_vc_x;
        }

        else if (p_win->vc_data.x + movement_x < __min_vc_x) {
            if (__x_overflow_act == DENG_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                p_win->vc_data.x =  __max_vc_x;
        }

        else p_win->vc_data.x += movement_x;

        // Check if overflow is detected on y position
        if (p_win->vc_data.y + movement_y >= __max_vc_y) {
            if (__y_overflow_act == DENG_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
                p_win->vc_data.y = __min_vc_y;
        }

        else if (p_win->vc_data.y + movement_y < __min_vc_y) {
            if (__y_overflow_act == DENG_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION)
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
    neko_px_t max_x,
    neko_px_t min_x,
    neko_px_t max_y,
    neko_px_t min_y
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
