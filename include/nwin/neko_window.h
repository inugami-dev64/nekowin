/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: neko_window.h - neko_Window structure definition
/// author: Karl-Mihkel Ott

#ifndef NEKO_WINDOW_H
#define NEKO_WINDOW_H


// Hint declarations
typedef enum _neko_SizeHint {
	NEKO_HINT_UNKNOWN,
	NEKO_HINT_FULL_SCREEN,
	NEKO_HINT_FIXED_SIZE,
	NEKO_HINT_RESIZEABLE
} neko_SizeHint;

// Platform specific window properties
#if defined(_WIN32)

typedef struct _neko_NativeWindowWin32 {
	HWND handle;
	HINSTANCE instance;
	MSG message;
	HGLRC gl;
} neko_NativeWindowWin32;


typedef struct _neko_Cursors {
	HCURSOR standard;
	HCURSOR waiting;
	HCURSOR pointer;
	HCURSOR hidden;
	HCURSOR custom;
} neko_Cursors;

#elif defined(__linux__)

typedef struct _neko_NativeWindowX11 {
	Window window;
	Display* display;
	GC gc;
	GLXContext glc;
	GLXDrawable drawable;
	XVisualInfo* p_vi;
	XVisualInfo vi;
} neko_NativeWindowX11;


typedef struct _neko_AtomsX11 {
	Atom WM_DELETE_WINDOW;
	Atom _NET_WM_STATE;
	Atom _NET_WM_STATE_FULLSCREEN;
	Atom _NET_WM_ICON;
	Atom _NET_WM_PID;
} neko_AtomsX11;


#define EVENT_MASK KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask |\
				   LeaveWindowMask | StructureNotifyMask | ClientMessage

#define VALUE_MASK CWBorderPixel | CWColormap | CWEventMask


typedef struct _neko_Cursors {
	Cursor standard;
	Cursor waiting;
	Cursor pointer;
	Cursor hidden;
} neko_Cursors;

#endif


typedef struct _neko_Window {
	neko_Input input;
	#if defined(_WIN32)
	neko_NativeWindowWin32 win32;
	#elif defined(__linux__)
	neko_NativeWindowX11 x11;
	#endif

	const char* title;
	int32_t cwidth;
	int32_t cheight;
	int32_t owidth;
	int32_t oheight;
	int32_t oposx;
	int32_t oposy;
	int32_t cposx;
	int32_t cposy;
	bool is_running;
	neko_SizeHint hints;
	neko_CursorMode cursor_mode;
} neko_Window;

#endif