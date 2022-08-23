/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: nekowgl.c - wgl function caller abstraction
/// author: Karl-Mihkel Ott

#define NEKO_GL_C
#include "nwin/nekogl.h"

typedef HGLRC(WINAPI* PFN_wglCreateContext)(HDC);
typedef BOOL(WINAPI* PFN_wglDeleteContext)(HGLRC);
typedef BOOL(WINAPI* PFN_wglMakeCurrent)(HDC, HGLRC);

static struct {
	PFN_wglCreateContext CreateContext;
	PFN_wglDeleteContext DeleteContext;
	PFN_wglMakeCurrent MakeCurrent;
} wgl = { 0 };


void neko_LoadOpenGLPlatform() {
	HMODULE libopengl = LoadLibraryW(L"opengl32.dll");
	except(libopengl, "Could not load opengl32.dll. Please make sure that graphics drivers are correctly installed");
	wgl.CreateContext = (PFN_wglCreateContext) GetProcAddress(libopengl, "wglCreateContext");
	wgl.DeleteContext = (PFN_wglDeleteContext) GetProcAddress(libopengl, "wglDeleteContext");
	wgl.MakeCurrent = (PFN_wglMakeCurrent) GetProcAddress(libopengl, "wglMakeCurrent");
}


void neko_SwapBuffers(neko_Window *_win) {
	HDC hdc = GetDC(_win->win32.handle);
	SwapBuffers(hdc);
}


void neko_CreateOpenGLContext(neko_Window* _win) {
	PIXELFORMATDESCRIPTOR pfd = { 0 };
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	HDC hdc = GetDC(_win->win32.handle);
	int pxf = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pxf, &pfd);

	_win->win32.gl = wgl.CreateContext(hdc);
	except(hdc, "Failed to create a wgl context for OpenGL rendering");
}


void neko_MakeOpenGLContextCurrent(neko_Window *_win) {
	HDC hdc = GetDC(_win->win32.handle);
	BOOL stat = wgl.MakeCurrent(hdc, _win->win32.gl);
	except(stat, "Failed to make current win32 context suitable for opengl rendering");
}


void neko_LoadOpenGLFunctions() {
	// load opengl functions from glad
	int stat = gladLoadGL();
	except(stat, "Could not load opengl functions");
}


void neko_SetSwapInterval(neko_Window *_win, int _interval) {
    // please implement
}


void neko_DeleteOpenGLContext(neko_Window *_win) {
	BOOL stat = wgl.DeleteContext(_win->win32.gl);
	except(stat, "Failed to delete wgl context");
}
