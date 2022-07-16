/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: input.c - neko cross-platfrom input API source file
/// author: Karl-Mihkel Ott

#define INPUT_C
#include <input.h>


/// Register new input event to input table
/// This function is meant to be called only by DENG platform dependant surface instances
void _neko_RegisterInputEvent(neko_Input *_input, neko_HidEvent _event, bool _active) {
    if(_active) {
        _input->raw.active_table[_event] = true;
        _input->raw.released_table[_event] = false;
    } else {
        _input->raw.released_table[_event] = true;
        _input->raw.active_table[_event] = false;
    }
}


/// Clean released key and mouse button array 
void _neko_ClearReleasedInputs(neko_Input *_input) {
    // clear raw input buffers
    memset(_input->raw.released_table, 0, sizeof(_input->raw.released_table));
    _input->raw.active_queue.size = 0;
    _input->raw.released_queue.size = 0;

    _input->raw.active_table[NEKO_MOUSE_SCROLL_DOWN] = false;
    _input->raw.active_table[NEKO_MOUSE_SCROLL_UP] = false;

    // clear unicode text input buffers
    _input->text.size = 0;
}


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