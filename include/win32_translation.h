/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: win32_translation.h - win32 event translation header
/// author: Karl-Mihkel Ott

#ifndef WIN32_TRANSLATION_H
#define WIN32_TRANSLATION_H

#ifdef WIN32_TRANSLATION_C
	#include <stdint.h>
	#include <stdbool.h>
	#include <windows.h>

	#include "nekodll.h"
	#include "input.h"
#endif

neko_HidEvent _neko_TranslateWin32Key(WPARAM _key);
neko_HidEvent _neko_TranslateWin32Btn(UINT _btn);

#endif