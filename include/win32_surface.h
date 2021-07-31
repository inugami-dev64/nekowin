/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: surface_window.h - abstracted surface window header file
/// author: Karl-Mihkel Ott

#ifndef __WIN32_SURFACE_H
#define __WIN32_SURFACE_H

#include <wingdi.h>
#include <windows.h>
#include <synchapi.h>    
#include <stdio.h>


// Extension attribute values
#define WGL_NUMBER_PIXEL_FORMATS_ARB 0x2000
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_TYPE_RGBA_ARB 0x202b
#define WGL_ACCELERATION_ARB 0x2003
#define WGL_NO_ACCELERATION_ARB 0x2025
#define WGL_RED_BITS_ARB 0x2015
#define WGL_RED_SHIFT_ARB 0x2016
#define WGL_GREEN_BITS_ARB 0x2017
#define WGL_GREEN_SHIFT_ARB 0x2018
#define WGL_BLUE_BITS_ARB 0x2019
#define WGL_BLUE_SHIFT_ARB 0x201a
#define WGL_ALPHA_BITS_ARB 0x201b
#define WGL_ALPHA_SHIFT_ARB 0x201c
#define WGL_ACCUM_BITS_ARB 0x201d
#define WGL_ACCUM_RED_BITS_ARB 0x201e
#define WGL_ACCUM_GREEN_BITS_ARB 0x201f
#define WGL_ACCUM_BLUE_BITS_ARB 0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB 0x2021
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_AUX_BUFFERS_ARB 0x2024
#define WGL_STEREO_ARB 0x2012
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_SAMPLES_ARB 0x2042
#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB 0x20a9
#define WGL_CONTEXT_DEBUG_BIT_ARB 0x00000001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_ES2_PROFILE_BIT_EXT 0x00000004
#define WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB 0x00000004
#define WGL_LOSE_CONTEXT_ON_RESET_ARB 0x8252
#define WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
#define WGL_NO_RESET_NOTIFICATION_ARB 0x8261
#define WGL_CONTEXT_RELEASE_BEHAVIOR_ARB 0x2097
#define WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB 0
#define WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB 0x2098
#define WGL_CONTEXT_OPENGL_NO_ERROR_ARB 0x31b3

typedef struct _neko_SurfaceWIN32 {
    HINSTANCE instance;
    HWND handle;
    MSG message;
    RAWINPUTDEVICE rids[2];
    RAWINPUT raw_input;
    UINT raw_input_size;
    HGLRC gl_context;
} neko_SurfaceWIN32;


#ifdef __NEKO_SURFACE_C
    typedef struct _neko_Cursors {
        HCURSOR standard;
        HCURSOR waiting;
        HCURSOR pointer;
        HCURSOR hidden;
    } _neko_Cursors;

    /// wgl function pointer type defintions
    typedef HGLRC (WINAPI *PFN_wglCreateContext)(HDC);
    typedef BOOL (WINAPI *PFN_wglDeleteContext)(HGLRC);
    typedef BOOL (WINAPI *PFN_wglMakeCurrent)(HDC, HGLRC);
    typedef PROC (WINAPI *PFN_wglGetProcAddress)(LPCSTR);
    typedef HGLRC (WINAPI *PFN_wglGetCurrentContext)();
    typedef HDC (WINAPI *PFN_wglGetCurrentDC)();

    /// wgl extension function pointer type defintions
    typedef BOOL (WINAPI *PFN_wglSwapIntervalEXT)(int);
    typedef BOOL (WINAPI *PFN_wglGetPixelFormatAttribivARB)(HDC,int,int,UINT,const int*,int*);
    typedef const char* (WINAPI *PFN_wglGetExtensionsStringEXT)();
    typedef const char* (WINAPI *PFN_wglGetExtensionsStringARB)(HDC);
    typedef HGLRC (WINAPI *PFN_wglCreateContextAttribsARB)(HDC,HGLRC,const int*);

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
        HINSTANCE instance;
        WGL wgl;
        _neko_Cursors cursors;
    } _neko_API; 
#endif

#define __NEKO_CLASS_NAME           "NWIN"
#define __NEKO_WGL_PF_ATTRIB_C      40


typedef struct neko_Window neko_Window;


/// WIN32 message callback function 
static LRESULT CALLBACK _neko_Win32MessageHandler(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam);


// Win32 implementation specific functions
static void _neko_HandleSizeHints(neko_Window *win, DWORD *ws);

static void _neko_CreateGLContext(neko_Window *win);
static int _neko_FindPixelFormat();
static void _neko_LoadWGL(neko_Window *win);
static void _neko_LoadWGLExtensions();
static BOOL _neko_FindExtensionSupport(const char *ext);

static void _neko_LoadCursors(neko_Window *win);
static WCHAR *_neko_CreateWideStringFromUTF8(const char *title);
static void _neko_ZeroValueErrorHandler(ULONG val, ULONG line, const char *err_msg);
#endif
