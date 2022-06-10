/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: window.h - abstaction header for platform specific headers and 
///                  generic window structure definition 
/// author: Karl-Mihkel Ott


#ifndef __WINDOW_H
#define __WINDOW_H


#if defined(__linux__)
    #include <x11_window.h>
#elif defined(_WIN32)
    #include <win32_window.h>
#endif

#endif
