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
