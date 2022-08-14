/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: x11_translation.h - Xlib input symbol to neko_HidEvent translator header
/// author: Karl-Mihkel Ott

#ifndef X11_TRANSLATION_H
#define X11_TRANSLATION_H

#ifdef X11_TRANSLATION_C
    #include <X11/X.h>
    #include <X11/keysym.h>
    #include <stdbool.h>
    #include <stdint.h>

    #include "nekodll.h"
    #include "input.h"
#endif

neko_HidEvent _neko_TranslateKeyX11(KeySym _key);
neko_HidEvent _neko_TranslateMouseBtnX11(uint32_t _btn);


#endif