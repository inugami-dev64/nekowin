/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: xkb_unicode.h KeySym to unicode lookup header
/// author: Karl-Mihkel Ott


#ifndef XKB_UNICODE_H
#define XKB_UNICODE_H

#ifdef XKB_UNICODE_C
    #include <nekodll.h>
    #include <X11/X.h>
    #include <stdint.h>
    #include <stddef.h>
    #include <stdbool.h>
#endif

LIBNWIN_API uint16_t _neko_KeysymToUnicode(KeySym _key);

#endif