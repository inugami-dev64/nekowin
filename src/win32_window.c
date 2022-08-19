/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: win32_window.c - win32 window / surface creation handler header file
/// author: Karl-Mihkel Ott

#define WIN32_WINDOW_C
#include "nwin/nwin.h"

#define WIN32_CLASS_NAME L"NWIN100"
static neko_Window* _active_window = NULL;

/*****************************/
/****** Inner functions ******/
/*****************************/

// Callback function might be omitted, if possible
static LRESULT CALLBACK _neko_Win32MessageHandler(
    HWND _handle,
    UINT _msg,
    WPARAM _wparam,
    LPARAM _lparam
) {
    except(_active_window, "_active_window value is invalid");
    switch (_msg) {
        case WM_CREATE:
            {
                _active_window->win32.handle = _handle;
                neko_Hint hints = _active_window->hints;
                if ((hints & NEKO_HINT_API_OPENGL) == NEKO_HINT_API_OPENGL)
                    _neko_CreateGLContext(_active_window);
            }
            break;

        case WM_CLOSE:
        case WM_DESTROY:
            _active_window->is_running = false;
            return 0;

        // General key press events
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_IME_KEYDOWN:
        {
            neko_HidEvent hid = _neko_TranslateWin32Key(_wparam);
            if(!_active_window->input.use_text_mode || (hid >= NEKO_KEY_ESCAPE && hid <= NEKO_KEY_MENU)) {
                _neko_RegisterInputEvent(&_active_window->input, hid, true);
            }
            break;
        }

        case WM_SYSKEYUP:
        case WM_KEYUP:
        case WM_IME_KEYUP:
        {
            neko_HidEvent hid = _neko_TranslateWin32Key(_wparam);
            if (!_active_window->input.use_text_mode || (hid >= NEKO_KEY_ESCAPE && hid <= NEKO_KEY_MENU)) {
                _neko_RegisterInputEvent(&_active_window->input, hid, false);
            }
            break;
        }

        // Key character events
        case WM_CHAR:
        case WM_SYSCHAR:
        {
            if (_active_window->input.use_text_mode) {
                _active_window->input.text.ucs[_active_window->input.text.size++] = (uint16_t)_wparam;
            }
            break;
        }

        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        {
            neko_HidEvent hid = _neko_TranslateWin32Btn(_msg);
            _neko_RegisterInputEvent(&_active_window->input, hid, true);
            break;
        }

        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        {
            neko_HidEvent hid = _neko_TranslateWin32Btn(_msg);
            _neko_RegisterInputEvent(&_active_window->input, hid, false);
            break;
        }

        case WM_MOUSEWHEEL: 
        {
            LONG delta = GET_WHEEL_DELTA_WPARAM(_wparam);
            if (delta > 0) {
                _neko_RegisterInputEvent(&_active_window->input, NEKO_MOUSE_SCROLL_UP, true);
            } else {
                _neko_RegisterInputEvent(&_active_window->input, NEKO_MOUSE_SCROLL_DOWN, true);
            }
            break;
        }

        case WM_MOUSEMOVE: 
        {
            POINT pt;
            pt.x = GET_X_LPARAM(_lparam);
            pt.y = GET_Y_LPARAM(_lparam);
            _neko_HandleMouseMovement(_active_window, pt);
            break;
        }

        case WM_SIZE:
        case WM_MOVE:
        {
            if (!(_active_window->hints & NEKO_HINT_FULL_SCREEN)) {
                RECT win_rect = { 0 };
                GetClientRect(_handle, &win_rect);
                _active_window->cwidth = (int32_t)(win_rect.right - win_rect.left);
                _active_window->cheight = (int32_t)(win_rect.bottom - win_rect.top);
                GetWindowRect(_handle, &win_rect);
                _active_window->cposx = (int32_t)win_rect.left;
                _active_window->cposy = (int32_t)win_rect.top;
            }
            break;
        }

        default:
            break;
    }

    return DefWindowProcW(_handle, _msg, _wparam, _lparam);
}


static HICON _neko_CreateIcon(const neko_Icon* _icon) {
    BITMAPV5HEADER bmp_header;
    ZeroMemory(&bmp_header, sizeof(bmp_header));
    bmp_header.bV5Size = sizeof(bmp_header);
    bmp_header.bV5Width = _icon->width;
    bmp_header.bV5Height = _icon->height;
    bmp_header.bV5Planes = 1;
    bmp_header.bV5BitCount = 32;
    bmp_header.bV5Compression = BI_BITFIELDS;
    bmp_header.bV5RedMask = 0x000000ff;
    bmp_header.bV5GreenMask = 0x0000ff00;
    bmp_header.bV5BlueMask = 0x00ff0000;
    bmp_header.bV5AlphaMask = 0xff000000;

    HDC dc = GetDC(NULL);
    HBITMAP color = CreateBitmap(_icon->width, _icon->height, 1, 32, (void*)_icon->pixels);
    ReleaseDC(NULL, dc);

    if (!color) {
        fprintf(stderr, "Failed to create ARGB bitmap\n");
        exit(1);
    }

    HBITMAP mask = CreateBitmap(_icon->width, _icon->height, 1, 1, NULL);
    if (!mask) {
        fprintf(stderr, "Failed to create mask bitmap\n");
        DeleteObject(color);
        exit(1);
    }
    
    ICONINFO icon_info;
    ZeroMemory(&icon_info, sizeof(icon_info));
    icon_info.fIcon = TRUE;
    icon_info.hbmMask = mask;
    icon_info.hbmColor = color;

    HICON icon = CreateIconIndirect(&icon_info);
    DeleteObject(color);
    DeleteObject(mask);

    return icon;
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


static void _neko_HandleMouseMovement(neko_Window* _win, POINT _pt) {
    if (_win->input.cursor.is_virtual) {     // virtual cursor is enabled
        _win->input.cursor.delta_x = ((int64_t)_pt.x - _win->input.cursor.orig_x);
        _win->input.cursor.delta_y = ((int64_t)_pt.y - _win->input.cursor.orig_y);

        // Check if cursor should be set to the origin position
        if (_win->input.cursor.delta_x || _win->input.cursor.delta_y)
            neko_SetMouseCoords(_win, _win->input.cursor.orig_x, _win->input.cursor.orig_y);

        // Check if overflow is detected on x position
        if (_win->input.cursor.x + _win->input.cursor.delta_x > _win->input.cursor.max_vc_x) {
            if (_win->input.cursor.x_overflow == NEKO_VIRTUAL_CURSOR_OVERFLOW_ACTION_OVERWRITE)
                _win->input.cursor.x = _win->input.cursor.min_vc_x;
        }
        else if (_win->input.cursor.x + _win->input.cursor.delta_x < _win->input.cursor.min_vc_x) {
            if (_win->input.cursor.x_overflow == NEKO_VIRTUAL_CURSOR_OVERFLOW_ACTION_OVERWRITE)
                _win->input.cursor.x = _win->input.cursor.max_vc_x;
        }
        else {
            _win->input.cursor.x += _win->input.cursor.delta_x;
        }

        // Check if overflow is detected on y position
        if (_win->input.cursor.y + _win->input.cursor.delta_y > _win->input.cursor.max_vc_y) {
            if (_win->input.cursor.y_overflow == NEKO_VIRTUAL_CURSOR_OVERFLOW_ACTION_OVERWRITE)
                _win->input.cursor.y = _win->input.cursor.min_vc_y;
        }
        else if (_win->input.cursor.y + _win->input.cursor.delta_y < _win->input.cursor.min_vc_y) {
            if (_win->input.cursor.y_overflow == NEKO_VIRTUAL_CURSOR_OVERFLOW_ACTION_OVERWRITE)
                _win->input.cursor.y = _win->input.cursor.max_vc_y;
        }
        else {
            _win->input.cursor.y += _win->input.cursor.delta_y;
        }
    }
    else {                                // virtual cursor is disabled
        _win->input.cursor.delta_x = ((int64_t)_pt.x) - _win->input.cursor.x;
        _win->input.cursor.delta_y = ((int64_t)_pt.y) - _win->input.cursor.y;
        _win->input.cursor.x = (int64_t)_pt.x;
        _win->input.cursor.y = (int64_t)_pt.y;
    }
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
    
    _win->win32.gl_context = _neko_API.wgl.CreateContext(handle_to_device_context);
    except(_win->win32.gl_context, "Failed to create WGL context for OpenGL rendering");
}


/*******************************/
/******* NEKO API CALLS ********/
/*******************************/

void neko_InitAPI() {
    // TODO: Load wgl function pointers
    HINSTANCE opengl = LoadLibraryW(L"opengl32.dll");
    except(opengl, "Failed to open opengl32.dll");

    // Get main win32 instance
    _neko_API.instance = GetModuleHandleW(NULL);
    
    // Define cursors
    _neko_API.cursors.standard = LoadCursorA(NULL, IDC_ARROW);
    _neko_API.cursors.pointer = LoadCursorA(NULL, IDC_HAND);
    _neko_API.cursors.waiting = LoadCursorA(NULL, IDC_WAIT);
    _neko_API.cursors.hidden = NULL;
    _neko_API.is_init = true;

    // Register the main win32 class
    WNDCLASSEXW class = { 0 };
    class.cbSize = sizeof(class);
    class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    class.lpfnWndProc = _neko_Win32MessageHandler;
    class.hInstance = _neko_API.instance;
    class.lpszClassName = WIN32_CLASS_NAME;
    class.hCursor = _neko_API.cursors.standard;

    _neko_API.main_class = RegisterClassExW(&class);
    except(_neko_API.main_class, "Failed to register Win32 class");

    _neko_API.wgl.CreateContext = (PFN_wglCreateContext) GetProcAddress(opengl, "wglCreateContext");
    _neko_API.wgl.DeleteContext = (PFN_wglDeleteContext) GetProcAddress(opengl, "wglDeleteContext");
    _neko_API.wgl.MakeCurrent = (PFN_wglMakeCurrent) GetProcAddress(opengl, "wglMakeCurrent");
    _neko_API.wgl.GetProcAddress = (PFN_wglGetProcAddress) GetProcAddress(opengl, "wglGetProcAddress");
    _neko_API.wgl.GetCurrentContext = (PFN_wglGetCurrentContext) GetProcAddress(opengl, "wglGetCurrentContext");
    _neko_API.wgl.GetCurrentDC = (PFN_wglGetCurrentDC) GetProcAddress(opengl, "wglGetCurrentDC");
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
    win.input.cursor.orig_x = width / 2;
    win.input.cursor.orig_y = height / 2;

    // Check size hints and resize accordingly
    DWORD window_style = _neko_HandleSizeHints(&win);
    _active_window = &win;

    // wide title and wide icon
    wchar_t wide_title[2048] = { 0 };
    MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, win.window_title, -1, wide_title, 2048);
    
    win.win32.handle = CreateWindowExW(0, WIN32_CLASS_NAME, wide_title,
                                       window_style, 
                                       win.oposx, win.oposy,
                                       win.owidth, win.oheight,
                                       NULL, NULL,
                                       _neko_API.instance, NULL);
    except(win.win32.handle, "Failed to create a Win32 window");
    
    // NOTE: Whenever using WIN32 api, you must keep in mind that the created window size is absolute and contains all window decorations as well.
    //       In order to find the actual client are we must use GetClientRect() function
    RECT rect;
    GetClientRect(win.win32.handle, &rect);
    win.cwidth = (int32_t) (rect.right - rect.left);
    win.cheight = (int32_t) (rect.bottom - rect.top);

    win.is_running = true;
    _active_window = &win;
    ShowWindow(win.win32.handle, SW_NORMAL);
    _active_window = NULL;
    return win;
}


void neko_SetIcons(neko_Window* _win, uint32_t _count, neko_Icon* _icons) {
    HICON small_hicon, big_hicon;
    _active_window = _win;
    if (_count) {
        int32_t big_icon_area = GetSystemMetrics(SM_CXICON) * GetSystemMetrics(SM_CYICON);
        int32_t small_icon_area = GetSystemMetrics(SM_CXSMICON) * GetSystemMetrics(SM_CYSMICON);

        neko_Icon *small_ico = _icons, *big_ico = _icons;

        // find the closest icons representing big and small icons
        for (uint32_t i = 0; i < _count; i++) {
            const int32_t cur_area = _icons[i].width * _icons[i].height;
            const int32_t cur_small_area = small_ico->width * small_ico->height;
            const int32_t cur_big_area = big_ico->width * big_ico->height;

            if (abs(cur_area - small_icon_area) < abs(cur_small_area - small_icon_area))
                small_ico = &_icons[i];

            if (abs(cur_area - big_icon_area) < abs(cur_area - big_icon_area))
                big_ico = &_icons[i];
        }

        small_hicon = _neko_CreateIcon(small_ico);
        big_hicon = _neko_CreateIcon(big_ico);
    } else {
        small_hicon = (HICON)GetClassLongPtrW(_win->win32.handle, GCLP_HICON);
        big_hicon = (HICON)GetClassLongPtrW(_win->win32.handle, GCLP_HICONSM);
    }

    SendMessageW(_win->win32.handle, WM_SETICON, ICON_SMALL, (LPARAM)small_hicon);
    SendMessageW(_win->win32.handle, WM_SETICON, ICON_BIG, (LPARAM)big_hicon);
    _active_window = NULL;
}


VkResult neko_InitVkSurface(neko_Window *_win, VkInstance _ins, VkSurfaceKHR *_surface) {
    _active_window = _win;
    VkWin32SurfaceCreateInfoKHR surface_info;
    memset(&surface_info, 0, sizeof(surface_info));
    surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surface_info.hinstance = _neko_API.instance;
    surface_info.hwnd = _win->win32.handle;
    surface_info.pNext = NULL;
    
    PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
    vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR) vkGetInstanceProcAddr(_ins, "vkCreateWin32SurfaceKHR");

    VkResult res = vkCreateWin32SurfaceKHR(_ins, &surface_info, NULL, _surface);
    _active_window = NULL;
    return res;
}


/// Update window events and key arrays
/// This function is meant to be called with every loop iteration 
void neko_UpdateWindow(neko_Window *_win) {
    _active_window = _win;

    // Find the scroll event status
    _neko_ClearReleasedInputs(&_win->input);

    if(_win->hints & NEKO_HINT_API_OPENGL) {
        HDC dc = GetDC(_win->win32.handle);
        SwapBuffers(dc);
    }

    if (!_win->is_running) {
        _active_window = NULL;
        return;
    }
    
    while(PeekMessageW(&_win->win32.message, _win->win32.handle, 0, 0, PM_REMOVE)) {
        TranslateMessage(&_win->win32.message);
        DispatchMessage(&_win->win32.message);
    }

    _active_window = NULL;
}


/// Set new resettable hints for neko window
void neko_UpdateSizeMode(neko_Window *_win, neko_Hint _hints) {
    _active_window = _win;
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
    _active_window = NULL;
}


void neko_glMakeCurrent(neko_Window* _win) {
    HDC handle_to_device_context = GetDC(_win->win32.handle);
    _neko_API.wgl.MakeCurrent(handle_to_device_context, _win->win32.gl_context);
}


/// Destroy window instance and free all resources that were used
void neko_DestroyWindow(neko_Window *_win) {
    _active_window = _win;
    if(_win->hints & NEKO_HINT_API_OPENGL)
        _neko_API.wgl.DeleteContext(_win->win32.gl_context);
    DestroyWindow(_win->win32.handle);
    _active_window = NULL;
}


/// Switch mouse cursor behaviour within the DENG window 
void neko_SetMouseCursorMode (
    neko_Window *_win, 
    neko_CursorMode _cur_mode
) {
    _active_window = _win;
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
    _active_window = NULL;
}


/// Force mouse cursor to certain location on window
void neko_SetMouseCoords (
    neko_Window *_win, 
    uint64_t _x, 
    uint64_t _y
) {
    _active_window = _win;
    POINT pnt;
    pnt.x = (LONG) _x;
    pnt.y = (LONG) _y;
    ClientToScreen(_win->win32.handle, &pnt);
    SetCursorPos((int) pnt.x, (int) pnt.y);
    _active_window = _win;
}



char **neko_FindRequiredVkExtensionStrings(uint32_t *_ext_c) {
    static char lexts[3][32] = { 0 };
    static char *exts[3] = { lexts[0], lexts[1], lexts[2]};

#ifdef __DEBUG
    *_ext_c = 3;
    strcpy(exts[2], "VK_EXT_debug_utils");
#else
    *_ext_c = 2;
#endif
    strcpy(exts[0], "VK_KHR_surface");
    strcpy(exts[1], "VK_KHR_win32_surface");
    
    return exts;
}
