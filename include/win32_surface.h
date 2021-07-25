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

    typedef HGLRC (WINAPI * PFN_wglCreateContext)(HDC);
    typedef BOOL (WINAPI * PFN_wglDeleteContext)(HGLRC);
    typedef BOOL (WINAPI * PFN_wglMakeCurrent)(HDC, HGLRC);

    struct {
        HINSTANCE instance;
        PFN_wglCreateContext CreateContext;
        PFN_wglDeleteContext DeleteContext;
        PFN_wglMakeCurrent MakeCurrent;
        _neko_Cursors cursors;
    } _neko_API; 

    #define wglCreateContext _neko_API.CreateContext
    #define wglDeleteContext _neko_API.DeleteContext
    #define wglMakeCurrent _neko_API.MakeCurrent
#endif

#define __NEKO_CLASS_NAME   "NWIN"


typedef struct neko_Window neko_Window;


/// WIN32 message callback function 
static LRESULT CALLBACK _neko_Win32MessageHandler(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam);


// Win32 implementation specific functions
static void _neko_HandleSizeHints(neko_Window *win, DWORD *ws);

// TEMPORARY!!!
static void _neko_CreateGLContext(HWND hWnd);
static void _neko_LoadCursors(neko_Window *win);
static WCHAR *_neko_CreateWideStringFromUTF8(const char *title);
static void _neko_ZeroValueErrorHandler(ULONG val, const char *err_msg);
#endif
