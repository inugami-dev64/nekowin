/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: nwin.h - abstracted surface window header file
/// author: Karl-Mihkel Ott

#ifndef __NWIN_H
#define __NWIN_H


#ifdef __cplusplus
extern "C" {
#endif


#ifdef __NWIN_C
    #include <signal.h>
    #include <stdbool.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdio.h>
    #ifdef __linux__
        #include <X11/keysym.h>
        #include <X11/XKBlib.h> 
    #endif
#endif


/// Custom macros
#define neko_assert(val, msg) if(!val) fprintf(stderr, "Assertion failed in %s:%u\n%s\n", __FILE__, __LINE__, msg), abort()
#define pcast(T, src) ((T) ((uint64_t) (src)))

/// OpenGL version definition macros
#define OPENGL_SUPPORTED_MAJOR  4
#define OPENGL_SUPPORTED_MINOR  6

#define neko_LoadGL gladLoadGL

#include <vulkan/vulkan.h>
#include "../include/nekodll.h"

#ifdef _WIN32
    #include <windows.h>
#endif

#include <glad/glad.h>
#include "../include/key_translation.h"
#include "../include/key_ev.h"

#ifdef __NWIN_C

    // Global static varible declarations
    // Specify maximum and minimum virtual cursor positions
    static int64_t __min_vc_x = INT64_MIN;
    static int64_t __max_vc_x = INT64_MAX;
    static int64_t __min_vc_y = INT64_MIN;
    static int64_t __max_vc_y = INT64_MAX;

    static int64_t __prev_x = 0;
    static int64_t __prev_y = 0;

    // VCP overflow action specifiers
    neko_VCPOverflowAction __x_overflow_act = NEKO_VCP_OVERFLOW_ACTION_BLOCK_POSITION;
    neko_VCPOverflowAction __y_overflow_act = NEKO_VCP_OVERFLOW_ACTION_BLOCK_POSITION;

    // Vulkan extension name macros
    #define NEKO_VK_WSI_EXT_NAME            "VK_KHR_surface"
    #define NEKO_VK_XLIB_SURFACE_EXT_NAME   "VK_KHR_xlib_surface"
    #define NEKO_VK_WIN32_SURFACE_EXT_NAME  "VK_KHR_win32_surface"
    #define NEKO_VK_DEBUG_UTILS_EXT_NAME    "VK_EXT_debug_utils"

    static bool __api_init = false;
#endif


// Linux X11 includes and definitions
#if defined(__linux__)
    // X11 includes 
    #include <X11/Xutil.h>
    #include <X11/Xatom.h>
    #include <GL/glx.h>
    #include <X11/cursorfont.h>
    #include <X11/Xcursor/Xcursor.h>
    #include <X11/extensions/Xrandr.h>
    #include <vulkan/vulkan_xlib.h>

    /// Commonly used atoms used in API instance structure
    typedef struct _neko_X11Atoms {
        Atom WM_DELETE_WINDOW;
        Atom _NET_WM_STATE;
        Atom _NET_WM_STATE_FULLSCREEN;
    } _neko_X11Atoms;


    typedef struct {
        Window window;
        GC gc;
        GLXContext glc;
        GLXDrawable drawable;
        XVisualInfo *p_vi;
        XVisualInfo vi;
    } neko_NativeWindowX11;


    #ifdef __NWIN_C
        #define EVENT_MASK KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | LeaveWindowMask | \
                               StructureNotifyMask | ClientMessage
        
        
        #define VALUE_MASK CWBorderPixel | CWColormap | CWEventMask
        
        
            // Message type definitions
        #define _NET_WM_STATE_REMOVE    0
        #define _NET_WM_STATE_ADD       1
        #define _NET_WM_STATE_TOGGLE    2
        
        #define _NEKO_XLIB_DEFAULT_CURSOR      "dnd_none"
        #define _NEKO_DEFAULT_WINDOW_BORDER    5

        // GLX extension name macros
        #define GLX_SWAP_CONTROL_EXT_NAME       "GLX_EXT_swap_control"
        #define GLX_SWAP_CONTROL_SGI_NAME       "GLX_SGI_swap_control"
        #define GLX_SWAP_CONTROL_MESA_NAME      "GLX_MESA_swap_control"

        /// Structure for storing all cursor data
        typedef struct _neko_Cursors {
            Cursor standard;
            Cursor waiting;
            Cursor pointer;
            Cursor hidden;
        } _neko_XCursors;

        typedef void(*PFN_glXSwapIntervalEXT)(Display*, GLXDrawable, int); 
        typedef int(*PFN_glXSwapIntervalSGI)(int);
        typedef int(*PFN_glXSwapIntervalMESA)(unsigned int);

        /// Structure for containing all API specific information 
        struct {
            bool is_init;
            Display *display;
            _neko_X11Atoms atoms;
            Window root;
            int32_t scr;
            _neko_XCursors cursors;
            PFN_glXSwapIntervalEXT glXSwapIntervalEXT;
            PFN_glXSwapIntervalSGI glXSwapIntervalSGI;
            PFN_glXSwapIntervalMESA glXSwapIntervalMESA;
        } _neko_API = { 0 };

    #endif

#elif defined(_WIN32)
    #include <windowsx.h>
    #include <vulkan/vulkan_win32.h>


    // Extension attribute values
    #define WGL_CONTEXT_DEBUG_BIT_ARB               0x00000001
    #define WGL_CONTEXT_CORE_PROFILE_BIT_ARB        0x00000001
    #define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
    #define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
    #define WGL_CONTEXT_FLAGS_ARB                   0x2094
    #define WGL_CONTEXT_RELEASE_BEHAVIOR_ARB        0x2097
    #define WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB  0x2098
    #define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

    typedef struct _neko_SurfaceWIN32 {
        HINSTANCE instance;
        HWND handle;
        MSG message;
        RAWINPUTDEVICE rids[2];
        RAWINPUT raw_input;
        UINT raw_input_size;
        HGLRC gl_context;
    } neko_NativeWindowWin32;

#ifdef __NWIN_C
    typedef struct _neko_Cursors {
        HCURSOR standard;
        HCURSOR waiting;
        HCURSOR pointer;
        HCURSOR hidden;
    } _neko_Cursors;


    volatile sig_atomic_t __is_running;

    /// wgl function pointer type defintions
    typedef HGLRC(WINAPI* PFN_wglCreateContext)(HDC);
    typedef BOOL(WINAPI* PFN_wglDeleteContext)(HGLRC);
    typedef BOOL(WINAPI* PFN_wglMakeCurrent)(HDC, HGLRC);
    typedef PROC(WINAPI* PFN_wglGetProcAddress)(LPCSTR);
    typedef HGLRC(WINAPI* PFN_wglGetCurrentContext)();
    typedef HDC(WINAPI* PFN_wglGetCurrentDC)();

    /// wgl extension function pointer type defintions
    typedef BOOL(WINAPI* PFN_wglSwapIntervalEXT)(int);
    typedef BOOL(WINAPI* PFN_wglGetPixelFormatAttribivARB)(HDC, int, int, UINT, const int*, int*);
    typedef const char* (WINAPI* PFN_wglGetExtensionsStringEXT)();
    typedef const char* (WINAPI* PFN_wglGetExtensionsStringARB)(HDC);
    typedef HGLRC(WINAPI* PFN_wglCreateContextAttribsARB)(HDC, HGLRC, const int*);

    typedef struct WGL {
        BOOL is_init;
        PFN_wglCreateContext CreateContext;
        PFN_wglDeleteContext DeleteContext;
        PFN_wglMakeCurrent MakeCurrent;
        PFN_wglGetProcAddress GetProcAddress;
        PFN_wglGetCurrentContext GetCurrentContext;
        PFN_wglGetCurrentDC GetCurrentDC;

        PFN_wglSwapIntervalEXT SwapIntervalEXT;
        PFN_wglGetPixelFormatAttribivARB GetPixelFormatAttribivARB;
        PFN_wglGetExtensionsStringARB GetExtensionsStringARB;
        PFN_wglGetExtensionsStringEXT GetExtensionsStringEXT;
        PFN_wglCreateContextAttribsARB CreateContextAttribsARB;

        BOOL    EXT_swap_control;
        BOOL    ARB_multisample;
        BOOL    ARB_framebuffer_sRGB;
        BOOL    EXT_framebuffer_sRGB;
        BOOL    ARB_pixel_format;
        BOOL    ARB_create_context;
        BOOL    ARB_create_context_profile;
        BOOL    EXT_create_context_es2_profile;
        BOOL    ARB_create_context_robustness;
        BOOL    ARB_create_context_no_error;
        BOOL    ARB_context_flush_control;
    } WGL;

    struct {
        bool is_init;
        HINSTANCE instance;
        WGL wgl;
        _neko_Cursors cursors;
    } _neko_API = { 0 };
#endif

    #define __NEKO_CLASS_NAME           "NWIN"
    #define __NEKO_WGL_PF_ATTRIB_C      40
#endif

/*
 * Virtual cursor (VC) position in nekowin means that mouse cursor is stuck to certain position
 * and is only allowed to move within one frame cycle.
 * That also means that the recorded mouse position is not corresponding to the real
 * position of mouse cursor.
 */
typedef struct neko_VCData {
   bool is_enabled;
    #ifdef __linux__
        char * cursor;
        bool is_lib_cur;
    #endif
    // X and Y virtual position are in 64bit floating point integer since 
    // Arithmetic operation with these types are needed in camera classes
    int64_t x;
    int64_t y;
    int64_t orig_x;
    int64_t orig_y;
} neko_VCData;


/// Main structure for storing information about surface window and its parameters.
typedef struct neko_Window {
    int32_t cwidth;
    int32_t cheight;
    int32_t owidth;
    int32_t oheight;
    int32_t oposx;
    int32_t oposy;
    int32_t cposx;
    int32_t cposy;
    const char *window_title;
    int64_t mx;
    int64_t my;
    volatile sig_atomic_t is_running;
    volatile sig_atomic_t resize_notify;
    neko_Hint hints;
    neko_VCData vc_data;
    neko_CursorMode cursor_mode;
    #if defined(_WIN32)
        neko_NativeWindowWin32 win32;
    #elif defined(__linux__)
        neko_NativeWindowX11 x11;
    #endif
} neko_Window;


#if defined(__linux__) && defined(__NWIN_C)
    /// Inner function declarations for X11 backend
    static void _neko_HandleKeyEvents(int _type, XKeyEvent *_kev); 
    static void _neko_HandleMouseEvents(int _type, XButtonEvent *_bev); 
    static void _neko_HandleMouseMovement(neko_Window *_win, int64_t _x, int64_t _y);
    static void _neko_GetVisualInfo(neko_Window *_win);
    static void _neko_SendClientMessage(neko_Window *_win, Atom _msg_type, long *_data);
    static void _neko_UpdateWindowSize(neko_Window *_win);
    static void _neko_LoadCursors();
#elif defined(_WIN32) && defined(__NWIN_C)
    static neko_Window  *__p_win = NULL;

    // Win32 implementation specific functions
    static LRESULT CALLBACK _neko_Win32MessageHandler(HWND _hwnd, UINT _msg, WPARAM _param, LPARAM _lparam);
    static DWORD _neko_HandleSizeHints(neko_Window *_win);
    static void _neko_CreateGLContext(neko_Window *_win);
    static void _neko_ZeroValueErrorHandler(ULONG _val, ULONG _line, const char* _err_msg);
#endif


/**************************/
/****** API bindings ******/
/**************************/

/// Initialise platform dependent backend api for nekowin library
LIBNWIN_API void neko_InitAPI();


/// Get the initialisation status of the API
LIBNWIN_API bool neko_APIInitStatus();


/// Deinitialise platform dependent backend api for nekowin library
LIBNWIN_API void neko_DeinitAPI();


/** 
 * Create new platform independant neko_Window instance for Vulkan or OpenGL
 * @param _width specifies the initial window width to use
 * @param _height specifies the initial window height to use
 * @param _hints specify window hints to use
 * @param _parent specifies a valid pointer to neko_Window instance that is as a parent for window to be created
 * @param _spawn_x specifies the initial window spawn location (X coordinate)
 * @param _spawn_y specifies the initial window spawn location (Y coordinate)
 * @param _title specifies the title to use for new window
 */
LIBNWIN_API neko_Window neko_NewWindow(
    int32_t _width, 
    int32_t _height, 
    neko_Hint _hints, 
    int32_t _spawn_x,
    int32_t _spawn_y,
    const char *_title
);


/// Initialise the given neko_Window instance for Vulkan surface 
LIBNWIN_API VkResult neko_InitVkSurface(neko_Window *_win, VkInstance _ins, VkSurfaceKHR *_surface);


/// Update window events and key arrays
/// This function is meant to be called in every frame
LIBNWIN_API void neko_UpdateWindow(neko_Window *_win);


/// Set new resettable hints for neko window
LIBNWIN_API void neko_UpdateSizeMode(neko_Window *_win, neko_Hint _hints);


/// Make specified neko window the current rendering context
LIBNWIN_API void neko_glMakeCurrent(neko_Window *_win);


/// Destroy window instance and free all resources that were used
LIBNWIN_API void neko_DestroyWindow(neko_Window *_win);


/****************************************/
/****** Input device communication ******/
/****************************************/

/// Change mouse cursor mode within neko window
LIBNWIN_API void neko_SetMouseCursorMode(neko_Window *_win, neko_CursorMode _cur_mode);


/// Force mouse cursor to certain location on window
LIBNWIN_API void neko_SetMouseCoords(neko_Window *_win, uint64_t _x, uint64_t _y);


/// Synchronise mouse position in neko_Window
LIBNWIN_API void neko_UpdateMousePos(neko_Window *_win);


/// Acquire all required Vulkan extension strings
LIBNWIN_API char **neko_FindRequiredVkExtensionStrings(uint32_t *_p_ext_s);

/// Set VSync on or off
//LIBNWIN_API void neko_SetVSync(neko_Window _win, bool _on);

#ifdef __cplusplus
}
#endif

#endif
