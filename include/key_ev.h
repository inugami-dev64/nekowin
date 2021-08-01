/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: key_ev.h - key event handler header file
/// author: Karl-Mihkel Ott

#ifndef __KEY_EV_H
#define __KEY_EV_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __KEY_EV_C
    #include <stdlib.h>
    #include <stdarg.h>   
    #include <stdint.h>
    #include <stdbool.h>
    #include <string.h>
    #include <math.h>

    #include <vulkan/vulkan.h>
    #ifdef __linux__
        #include <X11/XKBlib.h>
    #endif

    #ifdef _WIN32
        #include <Windows.h>
    #endif

    #include <key_translation.h>
    #include <nwin.h>


    /// Key event registry arrays
    static char input_ch = 0x00;
    bool active_ev[NEKO_INPUT_EV_COUNT] = { 0 };
    bool released_ev[NEKO_INPUT_EV_COUNT] = { 0 };
#endif


/// Create new input bitmask from multiple input events
neko_InputBits neko_CreateInputMask(uint32_t ev_c, ...);


/// Unmask neko_InputBits instance and return a static array of neko_InputEv, which size is exactly 8.
/// NOTE: Array elements that have no event attached use NEKO_KEY_UNKNOWN as a value
neko_InputEv *neko_UnmaskInput(neko_InputBits bits);


/// Register new keyevent to key register
/// This function is meant to be called only by DENG platform dependant surface instances
void _neko_RegisterKeyEvent (
    neko_Key key, 
    neko_MouseButton btn, 
    neko_InputType in_type, 
    neko_InputEventType ev_type
);


/// Clean released key and mouse button array 
void _neko_UnreleaseKeys();


/// Find given key or mouse button status from specified event array
const bool neko_FindKeyStatus(uint8_t key, neko_InputEventType ev_type);


/// Find the active ascii input key
const char neko_GetActiveInput();

#ifdef __cplusplus
}
#endif

#endif

