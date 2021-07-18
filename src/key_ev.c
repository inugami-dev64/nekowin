/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: key_ev.c - key event handler source file
/// author: Karl-Mihkel Ott

#define __KEY_EV_C
#include <key_ev.h>


/// Create new input bitmask from multiple input events
neko_InputBits neko_CreateInputMask(uint32_t ev_c, ...) {
    neko_InputBits bits = 0;
    va_list args;
    va_start(args, ev_c);

    if(ev_c > 8) {
        fprintf(stderr, "neko_CreateInputMask(), %u: %s\n", __LINE__, "Cannot have more than 8 input events!");
        exit(-1);
    }

    for(uint32_t i = 0; i < 8; i++) {
        if(i < ev_c)
            bits |= va_arg(args, uint32_t);
        else bits |= NEKO_KEY_UNKNOWN;

        if(i != 7) bits = bits << 8;
    }

    return bits;
}


/// Unmask neko_InputBits instance and return array
/// of neko_InputEv, whose size is exactly 8.
/// NOTE: Array elements that have no event index attached use NEKO_KEY_UNKNOWN as a value
neko_InputEv *neko_UnmaskInput(neko_InputBits bits) {
    static neko_InputEv out_ev[8];
    uint32_t ev = NEKO_KEY_UNKNOWN;

    for(int32_t i = 7; i >= 0; i--) {
        // If not the last index, shift input bits right
        if(i != 7) bits = bits >> 8;

        // Perform event id unmasking 
        ev = bits & 0b11111111;

        if(ev >= NEKO_KEY_UNKNOWN && ev <= NEKO_KEY_MENU)
            out_ev[i].key = (neko_Key) ev;
        else if(ev >= NEKO_MOUSE_BTN_UNKNOWN && ev <= NEKO_MOUSE_SCROLL_UP)
            out_ev[i].btn = (neko_MouseButton) ev;
        else if(ev >= NEKO_MOUSE_DELTA_UNKNOWN && ev <= NEKO_MOUSE_DELTA_NY)
            out_ev[i].md_mov = (neko_MouseDeltaMovement) ev;
    }

    return out_ev;
}


/// Register new keyevent to key register
/// This function is meant to be called only by DENG platform dependant surface instances
void _neko_RegisterKeyEvent (
    neko_Key key, 
    neko_MouseButton btn, 
    neko_InputType in_type, 
    neko_InputEventType ev_type
) {
    input_ch = 0x00;
    if(in_type == NEKO_INPUT_TYPE_KB) {
        if(key == NEKO_KEY_UNKNOWN) return;
        if(ev_type == NEKO_INPUT_EVENT_TYPE_ACTIVE) {
            // Check if the input char should be recorded
            if(key >= NEKO_KEY_A && key <= NEKO_KEY_Z) {
                if(active_ev[NEKO_KEY_LEFT_SHIFT] || active_ev[NEKO_KEY_RIGHT_SHIFT])
                    input_ch = key - NEKO_KEY_A + 'A';
                else input_ch = key - NEKO_KEY_A + 'a';
            }
            else if(key >= NEKO_KEY_0 && key <= NEKO_KEY_9)
                input_ch = key - NEKO_KEY_0 + '0';
            else if(key >= NEKO_KEY_NP_0 && key <= NEKO_KEY_NP_9)
                input_ch = key - NEKO_KEY_NP_0 + '0';
            else if(!active_ev[NEKO_KEY_LEFT_SHIFT] && !active_ev[NEKO_KEY_RIGHT_SHIFT]) {
                // More direct key value to ascii translation
                switch(key) {
                case NEKO_KEY_SPACE: 
                    input_ch = ' ';
                    break;
                case NEKO_KEY_APOSTROPHE:
                    input_ch = '\'';
                    break;
                case NEKO_KEY_COMMA:
                    input_ch = ',';
                    break;
                case NEKO_KEY_DOT:
                    input_ch = '.';
                    break;
                case NEKO_KEY_SLASH:
                    input_ch = '/';
                    break;
                case NEKO_KEY_MINUS:
                    input_ch = '-';
                    break;
                case NEKO_KEY_SEMICOLON:
                    input_ch = ';';
                    break;
                case NEKO_KEY_EQUAL:
                    input_ch = '=';
                    break;
                default: break;
                }
            }
            
            active_ev[key] = true;
            released_ev[key] = false;
        }
        else if(ev_type == NEKO_INPUT_EVENT_TYPE_RELEASED) {
            active_ev[key] = false;
            released_ev[key] = true;
        }
    }

    else if(in_type == NEKO_INPUT_TYPE_MOUSE) {
        if(btn == NEKO_MOUSE_BTN_UNKNOWN) return;
        if(ev_type == NEKO_INPUT_EVENT_TYPE_ACTIVE) {
            active_ev[btn] = true;
            released_ev[btn] = false;
        }
        else if(ev_type == NEKO_INPUT_EVENT_TYPE_RELEASED) {
            active_ev[btn] = false;
            released_ev[btn] = true;
        }
    }
}


/// Find given key or mouse button status from specified event array
const bool neko_FindKeyStatus (
    uint8_t index, 
    neko_InputEventType ev_type
) {
    bool stat = false;
    if(ev_type == NEKO_INPUT_EVENT_TYPE_ACTIVE)
        stat = active_ev[index];
    else if(ev_type == NEKO_INPUT_EVENT_TYPE_RELEASED)
        stat = released_ev[index];
    return stat;
}


/// Clean released key and mouse button array 
void _neko_UnreleaseKeys() {
    memset(released_ev, 0x00, sizeof(released_ev));
    memset(released_ev, 0x00, sizeof(released_ev));
}


const char neko_GetActiveInput() { return input_ch; }
