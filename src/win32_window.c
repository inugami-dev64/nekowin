/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: win32.c - win32 window / surface creation handler header file
/// author: Karl-Mihkel Ott

#define __NWIN_C
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


void neko_DeinitAPI() {
    DestroyCursor(_neko_API.cursors.standard);
    DestroyCursor(_neko_API.cursors.pointer);
    DestroyCursor(_neko_API.cursors.waiting);
}


/// Create new platform independant neko_Window instance for vulkan
neko_Window neko_NewWindow (
    int32_t width,
    int32_t height, 
    neko_Hint hints,
    const char *title
) {
    neko_assert(wslot_reserved + 1 >= _MAX_WSLOT_C, "There are no free window slots available");
    neko_Window win = (wslot_reserved++);

    wslots[win].cwidth = wslots[win].swidth = width;
    wslots[win].cheight = wslots[win].sheight = height;
    wslots[win].hints = hints;
    wslots[win].window_title = title;
    wslots[win].vc_data.is_enabled = false;
    wslots[win].vc_data.orig_x = width / 2;
    wslots[win].vc_data.orig_y = height / 2;

    DWORD window_style;
    WNDCLASS class = { 0 };
    class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    class.lpfnWndProc = _neko_Win32MessageHandler;
    class.hInstance = wslots[win].win32.instance = GetModuleHandleW(NULL);
    class.lpszClassName = __NEKO_CLASS_NAME;

    _neko_ZeroValueErrorHandler((ULONG) RegisterClass(&class), 
                                (ULONG) __LINE__, 
                                "Failed to register neko window class");


    // Check size hints and resize accordingly
    _neko_HandleSizeHints(win, &window_style);

    LPWSTR w_title = _neko_CreateWideStringFromUTF8(wslots[win].window_title);
    wslots[win].win32.handle = CreateWindowExW(0, __NEKO_CLASS_NAME, w_title, 
                                       window_style, 
                                       0, 
                                       0, 
                                       wslots[win].cwidth, wslots[win].cheight, 
                                       NULL, NULL, 
                                       wslots[win].win32.instance, NULL);
                                    
    // Free all memory allocated for the wide title
    free(w_title);
    _neko_ZeroValueErrorHandler(pcast(ULONG, wslots[win].win32.handle), (ULONG) __LINE__, "Failed to initialise win32 window");

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

    // Create OpenGL context if necessary
    if(wslots[win].hints & NEKO_HINT_API_OPENGL)
        _neko_CreateGLContext(win);

    wslots[win].mx = 0;
    wslots[win].my = 0;

    wslots[win].vc_data.orig_x = (int64_t)(wslots[win].cwidth / 2);
    wslots[win].vc_data.orig_y = (int64_t)(wslots[win].cheight / 2);

	__is_running = true;
    return win;
}


VkResult neko_InitVKSurface(neko_Window win, VkInstance i, VkSurfaceKHR *s) {
    VkWin32SurfaceCreateInfoKHR surface_info;
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
    ShowWindow(wslots[win].win32.handle, SW_NORMAL);


    // NOTE: I am not sure how much of a overhead GetWindowRect possesses, but for now I guess it is okay
    if(!(wslots[win].hints & NEKO_HINT_FULL_SCREEN)) {
        RECT win_rect = { 0 };
        GetWindowRect(wslots[win].win32.handle, &win_rect);
        wslots[win].cwidth = (int32_t) (win_rect.right - win_rect.left);
        wslots[win].cheight = (int32_t) (win_rect.bottom - win_rect.top);
        wslots[win].cposx = (int32_t) win_rect.left;
        wslots[win].cposy = (int32_t) win_rect.top;
    }

    if(wslots[win].hints & NEKO_HINT_API_OPENGL) {
        HDC dc = GetDC(wslots[win].win32.handle);
        SwapBuffers(dc);
    }

    if (!__is_running) return;
    while(PeekMessageW(&wslots[win].win32.message, NULL, 0, 0, PM_REMOVE)) {
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
    else MoveWindow(wslots[win].win32.handle, wslots[win].cposx, wslots[win].cposy, wslots[win].cwidth, wslots[win].cheight, TRUE);
    SetWindowLongPtr(wslots[win].win32.handle, GWL_STYLE, ws);
}


/// Destroy window instance and free all resources that were used
void neko_DestroyWindow(neko_Window win) {
    if(wslots[win].hints & NEKO_HINT_API_OPENGL)
        _neko_API.wgl.DeleteContext(wslots[win].win32.gl_context);
    DestroyWindow(wslots[win].win32.handle);
}


/// Check if window is still running and no close events have happened
bool neko_IsRunning() {
    return __is_running;
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
    if(!wslots[win].vc_data.is_enabled) {
        wslots[win].mx = (LONG) x;
        wslots[win].my = (LONG) y;
    }
}


void neko_UpdateMousePos(neko_Window win) {
    POINT point;
    
    // Check if GetCursorPos and ScreenToClient calls are successful and update original positions
    if(GetCursorPos(&point) && ScreenToClient(wslots[win].win32.handle, &point)) {
        wslots[win].mx = point.x;
        wslots[win].my = point.y;
    }

    else {
        fprintf(stderr, "Failed to read WIN32 cursor\n");
        exit(EXIT_FAILURE);
    }
    
    // Check if virtual mouse positioning is enabled
    if(wslots[win].vc_data.is_enabled) {
        int64_t movement_x = ((int64_t) point.x - wslots[win].vc_data.orig_x);
        int64_t movement_y = ((int64_t) point.y - wslots[win].vc_data.orig_y);

        // Check if cursor should be set to the origin position
        if(point.x != (LONG) wslots[win].vc_data.orig_x || point.y != (LONG) wslots[win].vc_data.orig_y)
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
        wslots[win].mx = wslots[win].vc_data.orig_x;
        wslots[win].my = wslots[win].vc_data.orig_y;
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
    strcpy((*p_exts)[1], NEKO_VK_WIN32_SURFACE_EXT_NAME);
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


static void _neko_HandleSizeHints(neko_Window win, DWORD *ws) {
    if(wslots[win].hints & NEKO_HINT_FIXED_SIZE) {
        *ws = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;
        wslots[win].cwidth = wslots[win].swidth;
        wslots[win].cheight = wslots[win].cheight;
    }
    else if(wslots[win].hints & NEKO_HINT_RESIZEABLE) {
        *ws = WS_OVERLAPPEDWINDOW;
        wslots[win].cwidth = wslots[win].swidth;
        wslots[win].cheight = wslots[win].sheight;
    }
    if(wslots[win].hints & NEKO_HINT_FULL_SCREEN) {
        wslots[win].cwidth = GetSystemMetrics(SM_CXSCREEN);
        wslots[win].cheight = GetSystemMetrics(SM_CYSCREEN);
        *ws = WS_POPUP | WS_VISIBLE;
    }
}


static void _neko_CreateGLContext(neko_Window win) {
    // If needed create a wgl dummy window and load wgl extensions
    if(!_neko_API.wgl.is_init) 
        _neko_LoadWGL(win);
    
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
    int attribs[__NEKO_WGL_PF_ATTRIB_C] = { 0 };


    // TODO: Check if WGL_ARB_create_context extension is supported and create context using that extension when possible
    if(_neko_API.wgl.ARB_create_context && _neko_API.wgl.ARB_create_context_profile) {
        int index = 0, mask = 0;
        
        mask |= WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
        if(_neko_API.wgl.ARB_context_flush_control) {
            attribs[index++] = WGL_CONTEXT_RELEASE_BEHAVIOR_ARB;
            attribs[index++] = WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB;
        }

        // Set OpenGL version 
        attribs[index++] = WGL_CONTEXT_MAJOR_VERSION_ARB;
        attribs[index++] = OPENGL_SUPPORTED_MAJOR;

        attribs[index++] = WGL_CONTEXT_MINOR_VERSION_ARB;
        attribs[index++] = OPENGL_SUPPORTED_MINOR;

        attribs[index++] = WGL_CONTEXT_PROFILE_MASK_ARB;
        attribs[index++] = mask;

        #ifdef _DEBUG_
            int flags = WGL_CONTEXT_DEBUG_BIT_ARB;
            attribs[index++] = WGL_CONTEXT_FLAGS_ARB;
            attribs[index++] = flags;
        #endif

        _neko_ZeroValueErrorHandler(pcast(ULONG, wslots[win].win32.gl_context = _neko_API.wgl.CreateContextAttribsARB(handle_to_device_context, NULL, attribs)),
                                    (ULONG) __LINE__,
                                    "Failed to create WGL context using WGL_ARB_create_context extension");
    }

    else {
        _neko_ZeroValueErrorHandler(pcast(ULONG, wslots[win].win32.gl_context = _neko_API.wgl.CreateContext(handle_to_device_context)),
                                    (ULONG) __LINE__,
                                    "Failed to create WGL context using wglCreateContext function");
    }

    _neko_API.wgl.MakeCurrent(handle_to_device_context, wslots[win].win32.gl_context);
}


static void _neko_LoadWGL(neko_Window win) {
    _neko_API.wgl.is_init = TRUE;
    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;

    HDC dc = GetDC(NULL);

    // Attempt to set pixel format for dummy context
    int pixel_format = ChoosePixelFormat(dc, &pfd);
    _neko_ZeroValueErrorHandler((ULONG) SetPixelFormat(dc, pixel_format, &pfd), (ULONG) __LINE__, 
                                 "Failed to set pixel format for dummy context");

    // Create and make use of the dummy context
    HGLRC dummy_context = _neko_API.wgl.CreateContext(dc);
    _neko_ZeroValueErrorHandler(pcast(ULONG, dummy_context), 
                                (ULONG) __LINE__, 
                                "Failed to create dummy context");

    _neko_ZeroValueErrorHandler((ULONG) _neko_API.wgl.MakeCurrent(dc, dummy_context), 
                                (ULONG) __LINE__, 
                                "Failed to use correct wgl context");

    // Load all required extension related functions
    _neko_ZeroValueErrorHandler(pcast(ULONG, _neko_API.wgl.GetExtensionsStringEXT = (PFN_wglGetExtensionsStringEXT) _neko_API.wgl.GetProcAddress("wglGetExtensionsStringEXT")),
                                (ULONG) __LINE__,
                                "Failed to retrive wglGetExtensionsStringEXT()!");

    _neko_ZeroValueErrorHandler(pcast(ULONG, _neko_API.wgl.GetExtensionsStringARB = (PFN_wglGetExtensionsStringARB) _neko_API.wgl.GetProcAddress("wglGetExtensionsStringARB")),
                                (ULONG) __LINE__,
                                "Failed to retrieve wglGetExtensionsStringARB()!");

    _neko_ZeroValueErrorHandler(pcast(ULONG, _neko_API.wgl.SwapIntervalEXT = (PFN_wglSwapIntervalEXT) _neko_API.wgl.GetProcAddress("wglSwapIntervalEXT")), 
                                (ULONG) __LINE__,
                                "Failed to retrieve wglSwapIntervalEXT()!");

    _neko_ZeroValueErrorHandler(pcast(ULONG, _neko_API.wgl.GetPixelFormatAttribivARB = (PFN_wglGetPixelFormatAttribivARB) _neko_API.wgl.GetProcAddress("wglGetPixelFormatAttribivARB")),
                                (ULONG) __LINE__,
                                "Failed to retrieve wglGetPixelFormatAttribivARB()!");

    _neko_ZeroValueErrorHandler(pcast(ULONG, _neko_API.wgl.CreateContextAttribsARB = (PFN_wglCreateContextAttribsARB) _neko_API.wgl.GetProcAddress("wglCreateContextAttribsARB")),
                                (ULONG) __LINE__,
                                "Failed to retrieve wglCreateContextAttribsARB()!");



    _neko_LoadWGLExtensions();
    _neko_API.wgl.DeleteContext(dummy_context);
}


static void _neko_LoadWGLExtensions() {
    _neko_API.wgl.EXT_swap_control                  =   _neko_FindExtensionSupport("WGL_EXT_swap_control");
    _neko_API.wgl.ARB_multisample                   =   _neko_FindExtensionSupport("WGL_ARB_multisample");
    _neko_API.wgl.ARB_framebuffer_sRGB              =   _neko_FindExtensionSupport("WGL_ARB_framebuffer_sRGB");
    _neko_API.wgl.EXT_framebuffer_sRGB              =   _neko_FindExtensionSupport("WGL_EXT_framebuffer_sRGB");
    _neko_API.wgl.ARB_pixel_format                  =   _neko_FindExtensionSupport("WGL_ARB_pixel_format");
    _neko_API.wgl.ARB_create_context                =   _neko_FindExtensionSupport("WGL_ARB_create_context");
    _neko_API.wgl.ARB_create_context_profile        =   _neko_FindExtensionSupport("WGL_ARB_create_context_profile");
    _neko_API.wgl.EXT_create_context_es2_profile    =   _neko_FindExtensionSupport("WGL_EXT_create_context_es2_profile");
    _neko_API.wgl.ARB_create_context_robustness     =   _neko_FindExtensionSupport("WGL_ARB_create_context_robustness");
    _neko_API.wgl.ARB_create_context_no_error       =   _neko_FindExtensionSupport("WGL_ARB_create_context_no_error");
    _neko_API.wgl.ARB_context_flush_control         =   _neko_FindExtensionSupport("WGL_ARB_context_flush_control");
}


static BOOL _neko_FindExtensionSupport(const char *ext) {
    const char *extensions = _neko_API.wgl.GetExtensionsStringARB(_neko_API.wgl.GetCurrentDC());
    neko_assert(extensions, "Failed to retrieve any extensions from the extension list");

    const char *pt;
    char *max = (char*) (extensions + strlen(extensions));

    while(TRUE) {
        pt = strstr(extensions, ext);
        if(!pt) break;
        if(pt && (pt > extensions ? *(pt - 1) == ' ' : pt == extensions) && (pt + strlen(ext) < max))
            return TRUE;
    }

    return FALSE;
}


static WCHAR *_neko_CreateWideStringFromUTF8(const char *str) {
    WCHAR *out_str = (WCHAR*) calloc(strlen(str) + 1, sizeof(WCHAR));
    for(size_t i = 0; i < strlen(str); i++)
        out_str[i] = (WCHAR) str[i];

    return out_str;
}


static void _neko_ZeroValueErrorHandler(ULONG val, ULONG line, const char *err_msg) {
    if(!val) {
        DWORD code = GetLastError();
        fprintf(stderr, "Error code: 0x%08lx\n", code);
        fprintf(stderr, "%s:%u; %s\n", __FILE__, __LINE__, err_msg);
        exit(-1);
    }
}
