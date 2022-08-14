/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: nwin.h - abstracted surface window header file
/// author: Karl-Mihkel Ott

#ifndef __NWIN_H
#define __NWIN_H

#ifdef __cplusplus
extern "C" {
#endif


#if defined(X11_WINDOW_C) || defined(WIN32_WINDOW_C)
    #include <stdlib.h>
    #include <stdio.h>
    #define except(_cond, _msg) if(!_cond) { fprintf(stderr, "libneko: %s\n", _msg); abort(); }
#endif


/// OpenGL version definition macros
#define OPENGL_SUPPORTED_MAJOR  4
#define OPENGL_SUPPORTED_MINOR  6

#define neko_LoadGL gladLoadGL

// std includes
#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include "nwin/nekodll.h"
#include "nwin/input.h"
#include "nwin/gamepad.h"
#include "nwin/glad/glad.h"

// Hint declarations
typedef uint16_t neko_Hint;
#define NEKO_HINT_API_OPENGL        0x0001
#define NEKO_HINT_API_VULKAN        0x0002
#define NEKO_HINT_FULL_SCREEN       0x0004
#define NEKO_HINT_FIXED_SIZE        0x0008
#define NEKO_HINT_RESIZEABLE        0x0010
#define NEKO_HINT_NO_FULL_SCREEN    0x0010

/// Platform specific includes, structures and definitions
#if defined(_WIN32)
    #include <windows.h>
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

    typedef struct _neko_NativeWindowWin32 {
        HWND handle;
        MSG message;
        RAWINPUTDEVICE rids[2];
        RAWINPUT raw_input;
        UINT raw_input_size;
        HGLRC gl_context;
    } neko_NativeWindowWin32;

    #ifdef WIN32_WINDOW_C
        #include "nwin/win32_translation.h"
        typedef struct _neko_Cursors {
            HCURSOR standard;
            HCURSOR waiting;
            HCURSOR pointer;
            HCURSOR hidden;
        } _neko_Cursors;


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

        static struct {
            bool is_init;
            ATOM main_class;
            HINSTANCE instance;
            WGL wgl;
            _neko_Cursors cursors;
        } _neko_API = { 0 };
#endif

    #define __NEKO_CLASS_NAME           "NWIN"
    #define __NEKO_WGL_PF_ATTRIB_C      40

#elif defined(__linux__)
    // X11 includes
    #include <X11/X.h>
    #include <GL/glx.h>

    typedef struct {
        Window window;
        GC gc;
        GLXContext glc;
        GLXDrawable drawable;
        XVisualInfo *p_vi;
        XVisualInfo vi;
    } neko_NativeWindowX11;

    #ifdef X11_WINDOW_C
        #include <string.h>
        #include <X11/Xutil.h>
        #include <X11/XKBlib.h>
        #include <X11/Xatom.h>
        #include <X11/cursorfont.h>
        #include <X11/Xcursor/Xcursor.h>
        #include <X11/extensions/Xrandr.h>
        #include <vulkan/vulkan_xlib.h>
        #include "nwin/x11_translation.h"
        #include "nwin/xkb_unicode.h"

        #define STB_IMAGE_IMPLEMENTATION
        #include "nwin/stb_image.h"
        
        /// Commonly used atoms used in API instance structure
        typedef struct _neko_X11Atoms {
            Atom WM_DELETE_WINDOW;
            Atom _NET_WM_STATE;
            Atom _NET_WM_STATE_FULLSCREEN;
            Atom _NET_WM_ICON;
            Atom CARDINAL;
        } _neko_X11Atoms;

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
        static struct {
            bool is_init;
            Display *display;
            _neko_X11Atoms atoms;
            Window root;
            Pixmap icon_pixmap;
            int32_t scr;
            uint32_t icon_count;
            const char **icons;
            _neko_XCursors cursors;
            PFN_glXSwapIntervalEXT glXSwapIntervalEXT;
            PFN_glXSwapIntervalSGI glXSwapIntervalSGI;
            PFN_glXSwapIntervalMESA glXSwapIntervalMESA;
        } _neko_API = { 0 };
    #endif
#endif

#define UC_BUFFER_SIZE 32


/// Main structure for storing information about surface window and its parameters.
typedef struct neko_Window {
    neko_Input input;
    #if defined(_WIN32)
        neko_NativeWindowWin32 win32;
    #elif defined(__linux__)
        neko_NativeWindowX11 x11;
    #endif

    const char *window_title;
    int32_t cwidth;
    int32_t cheight;
    int32_t owidth;
    int32_t oheight;
    int32_t oposx;
    int32_t oposy;
    int32_t cposx;
    int32_t cposy;
    bool is_running;
    bool resize_notify;
    neko_Hint hints;
    neko_CursorMode cursor_mode;
} neko_Window;


#if defined(X11_WINDOW_C)
    /// Inner function declarations for X11 backend
    static void _neko_RegisterRaw(neko_Window *_win, int type, XKeyEvent *_kev);
    static void _neko_HandleKeyEvents(neko_Window *_win, int _type, XKeyEvent *_kev); 
    static void _neko_HandleMouseEvents(neko_Window *_win, int _type, XButtonEvent *_bev); 
    static void _neko_HandleMouseMovement(neko_Window *_win, int64_t _x, int64_t _y);
    static void _neko_GetVisualInfo(neko_Window *_win);
    static void _neko_SendClientMessage(neko_Window *_win, Atom _msg_type, long *_data);
    static void _neko_UpdateWindowSize(neko_Window *_win);
    static void _neko_LoadCursors();
#elif defined(WIN32_WINDOW_C)
    // Win32 implementation specific functions
    static LRESULT CALLBACK _neko_Win32MessageHandler(HWND _hwnd, UINT _msg, WPARAM _param, LPARAM _lparam);
    static bool _neko_CheckFunctionKey(WPARAM _key);
    static DWORD _neko_HandleSizeHints(neko_Window *_win);
    static void _neko_HandleMouseMovement(neko_Window* _win, POINT _pt);
    static void _neko_CreateGLContext(neko_Window *_win);
#endif


/**************************/
/****** API bindings ******/
/**************************/

/// Initialise platform dependent backend api for nekowin library
LIBNWIN_API void neko_InitAPI(uint32_t _count, const char **_icons);


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


/// Acquire all required Vulkan extension strings
LIBNWIN_API char **neko_FindRequiredVkExtensionStrings(uint32_t *_p_ext_s);

// #include "../include/limit_vc.h"

#ifdef __cplusplus
}
#endif

#endif
