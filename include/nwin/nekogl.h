/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: nekogl.h - platform specific OpenGL function handling header
/// author: Karl-Mihkel Ott

#ifndef NEKO_GL_H
#define NEKO_GL_H

#ifdef NEKO_GL_C
	#include <stdint.h>
	#include <stdbool.h>
#ifdef _WIN32
	#include <Windows.h>
#endif	

	#include "nwin/nekodll.h"
	#include "nwin/neko_except.h"
	#include "nwin/input.h"
	#include "nwin/neko_window.h"
	#include "nwin/glad/glad.h"
	
	#include "nwin/nwin.h"
#endif

/// OpenGL version definition macros
#define OPENGL_SUPPORTED_MAJOR  4
#define OPENGL_SUPPORTED_MINOR  3

void LIBNWIN_API neko_LoadOpenGLPlatform();
void LIBNWIN_API neko_SwapBuffers(neko_Window *_win);
void LIBNWIN_API neko_CreateOpenGLContext(neko_Window *_win);
void LIBNWIN_API neko_MakeOpenGLContextCurrent(neko_Window *_win);
void LIBNWIN_API neko_LoadOpenGLFunctions();
void LIBNWIN_API neko_DeleteOpenGLContext(neko_Window *_win);

#endif