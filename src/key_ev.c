/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: key_ev.c - key event handler source file
/// author: Karl-Mihkel Ott

#define __KEY_EV_C
#include <key_ev.h>


/// Create new input bitmask from multiple input events
neko_InputBits neko_CreateInputMask(neko_HidEvent evs[8]) {
    neko_InputBits bits = 0;

    for(uint32_t i = 0; i < 8; i++) {
        bits |= evs[i];
        if(i != 7) bits = bits << 8;
    }

    return bits;
}


/// Unmask neko_InputBits instance and return array
/// of neko_InputEv, whose size is exactly 8.
/// NOTE: Array elements that have no event index attached use NEKO_KEY_UNKNOWN as a value
neko_HidEvent *neko_UnmaskInput(neko_InputBits bits) {
    static neko_HidEvent out_ev[8] = {0};

    for(int32_t i = 7; i >= 0; i--) {
        // If not the last index, shift input bits right
        if(i != 7) bits = bits >> 8;
        // Perform event id unmasking 
        out_ev[i] = (neko_HidEvent) bits & (uint64_t) UINT8_MAX;
    }

    return out_ev;
}

/// Find given key or mouse button status from specified event array
bool neko_FindKeyStatus(neko_HidEvent event, neko_InputEventType ev_type) {
    switch(ev_type) {
        case NEKO_INPUT_EVENT_TYPE_ACTIVE:
            return active_ev[event];

        case NEKO_INPUT_EVENT_TYPE_RELEASED:
            return released_ev[event];

        default:
            return false;
    }
}


uint32_t neko_GetActiveInput() { return active_mask; }


/// Clean released key and mouse button array 
void _neko_UnreleaseKeys() {
    memset(released_ev, 0x00, sizeof(released_ev));
    active_ev[NEKO_MOUSE_SCROLL_DOWN] = false;
    active_ev[NEKO_MOUSE_SCROLL_UP] = false;
}


/// Register new keyevent to key register
/// This function is meant to be called only by DENG platform dependant surface instances
void _neko_RegisterKeyEvent(neko_HidEvent event, neko_InputEventType ev_type) {
    switch(ev_type) {
        case NEKO_INPUT_EVENT_TYPE_ACTIVE:
            active_ev[event] = true;
            released_ev[event] = false;
            break;

        case NEKO_INPUT_EVENT_TYPE_RELEASED:
            // ignore mouse scroll release events
            if(event != NEKO_MOUSE_SCROLL_DOWN && event != NEKO_MOUSE_SCROLL_UP) {
                active_ev[event] = false;
                released_ev[event] = true;
            }
            break;

        default:
            break;
    }
}
