/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: nekogl.h - glx function caller abstraction
/// author: Karl-Mihkel Ott

#define NEKO_GL_C
#include "nwin/nekogl.h"


typedef void(*PFN_glXSwapIntervalEXT)(Display*, GLXDrawable, int);
typedef int(*PFN_glXSwapIntervalSGI)(int);
typedef int(*PFN_glXSwapIntervalMESA)(unsigned int);

static struct { 
    PFN_glXSwapIntervalEXT SwapIntervalEXT;
    PFN_glXSwapIntervalSGI SwapIntervalSGI;
    PFN_glXSwapIntervalMESA SwapIntervalMESA;
} glX = { 0 };

static Display *dpy = NULL;
static int32_t scr = 0;


void neko_LoadOpenGLPlatform() {
    dpy = GetDisplay();
    scr = GetScreen();

    // search for swap control extensions
    const char *extensions = glXQueryExtensionsString(GetDisplay(), GetScreen());
    const char *ptr = extensions;
    const char *pt = NULL;

    const char *required[] = {
        "GLX_EXT_swap_control",
        "GLX_SGI_swap_control",
        "GLX_MESA_swap_control"
    };

    while((pt = strchr(ptr, ' ')) && ptr != (const char*)1) {
        if(!strncmp(required[0], ptr, strlen(required[0])))       
            glX.SwapIntervalEXT = (PFN_glXSwapIntervalEXT) glXGetProcAddress((const GLubyte*) required[0]);
        else if(!strncmp(required[1], ptr, strlen(required[1])))
            glX.SwapIntervalSGI = (PFN_glXSwapIntervalSGI) glXGetProcAddress((const GLubyte*) required[1]);
        else if(!strncmp(required[2], ptr, strlen(required[2])))
            glX.SwapIntervalMESA = (PFN_glXSwapIntervalMESA) glXGetProcAddress((const GLubyte*) required[2]);

        ptr = pt + 1;
    }
}


void neko_SwapBuffers(neko_Window *_win) {
    glXSwapBuffers(dpy, _win->x11.window);
}


void neko_CreateOpenGLContext(neko_Window *_win) {
    GLint attrs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    XVisualInfo *vis = glXChooseVisual(dpy, scr, attrs);
    _win->x11.glc = glXCreateContext(dpy, vis, NULL, GL_TRUE);
    except(_win->x11.glc, "Could not create a glx context");
}


void neko_MakeOpenGLContextCurrent(neko_Window *_win) {
    glXMakeCurrent(dpy, _win->x11.window, _win->x11.glc);
    _win->x11.drawable = glXGetCurrentDrawable();
}


void neko_LoadOpenGLFunctions() {
    int stat = gladLoadGL();
    except(stat, "Could not load opengl functions");
}


void neko_SetSwapInterval(neko_Window *_win, int _interval) {
    if(glX.SwapIntervalEXT) 
        glX.SwapIntervalEXT(dpy, _win->x11.window, _interval);
    else if(glX.SwapIntervalSGI)
        glX.SwapIntervalSGI(_interval);
    else if(glX.SwapIntervalMESA)
        glX.SwapIntervalMESA(_interval);
}


void neko_DeleteOpenGLContext(neko_Window *_win) {
    glXDestroyContext(dpy, _win->x11.glc);
}
