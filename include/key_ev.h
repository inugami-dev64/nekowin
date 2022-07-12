/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: key_ev.h - key event handler header file
/// author: Karl-Mihkel Ott

#ifndef __KEY_EV_H
#define __KEY_EV_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__KEY_EV_C)
    #include <nekodll.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>   
    #include <stdint.h>
    #include <stdbool.h>
    #include <string.h>
    #include <signal.h>
    #include <math.h>

    #include <vulkan/vulkan.h>
    #ifdef __linux__
        #include <X11/XKBlib.h>
    #endif

    #ifdef _WIN32
        #include <Windows.h>
    #endif

    #include <key_translation.h>
#endif

/// Not really queue like, but it works
typedef struct {
    neko_HidEvent events[NEKO_INPUT_EV_COUNT];
    uint32_t size;
} EventQueue;

#ifdef __KEY_EV_C
    /// Key event registry arrays
    static uint32_t active_mask = 0x00;
    static bool active_ev[NEKO_INPUT_EV_COUNT] = { 0 };
    static bool released_ev[NEKO_INPUT_EV_COUNT] = { 0 };
    static EventQueue active_queue = { 0 };
    static EventQueue released_queue = { 0 };
#endif


#if defined(__NWIN_C) || defined(__KEY_EV_C)
    /// Register new keyevent to key register
    /// This function is meant to be called only by DENG platform dependant surface instances
    void _neko_RegisterKeyEvent(neko_HidEvent event, neko_InputEventType ev_type);


    /// Clean released key and mouse button array 
    void _neko_UnreleaseKeys();
#endif


/// Create new input bitmask from multiple input events
LIBNWIN_API neko_InputBits neko_CreateInputMask(neko_HidEvent evs[8]);


/// Unmask neko_InputBits instance and return a static array of neko_InputEv, which size is exactly 8.
/// NOTE: Array elements that have no event attached use NEKO_KEY_UNKNOWN as a value
LIBNWIN_API neko_HidEvent *neko_UnmaskInput(neko_InputBits bits);


/// Find given key or mouse button status from specified event array
LIBNWIN_API bool neko_FindKeyStatus(neko_HidEvent event, neko_InputEventType ev_type);


/// Get a pointer to active key table
LIBNWIN_API const bool *neko_GetActiveInputTable();


/// Get a pointer to released key table
LIBNWIN_API const bool *neko_GetReleasedInputTable();


/// Get a pointer to active event queue
LIBNWIN_API const EventQueue *neko_GetActiveInputQueue();


/// Get a pointer to released event queue
LIBNWIN_API const EventQueue *neko_GetReleasedInputQueue();

/// Find the active ascii input key
LIBNWIN_API uint32_t neko_GetActiveInput();

#ifdef __cplusplus
}
#endif

#endif

