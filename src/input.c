// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
// licence: Apache, see LICENCE.md
// file: input.c - neko cross-platfrom input API source file
// author: Karl-Mihkel Ott

#define INPUT_C
#include "nwin/input.h"


#if defined(_WIN32)
neko_HidEvent neko_TranslateNativeKeyCode(uint32_t _code) {
    switch (_code) {
        case VK_SPACE:              return NEKO_KEY_SPACE;
        case VK_OEM_7:              return NEKO_KEY_APOSTROPHE;
        case VK_OEM_COMMA:          return NEKO_KEY_COMMA;
        case VK_OEM_MINUS:          return NEKO_KEY_MINUS;
        case VK_OEM_PERIOD:         return NEKO_KEY_DOT;
        case VK_OEM_2:              return NEKO_KEY_SLASH;
        case 0x30:                  return NEKO_KEY_0;
        case 0x31:                  return NEKO_KEY_1;
        case 0x32:                  return NEKO_KEY_2;
        case 0x33:                  return NEKO_KEY_3;
        case 0x34:                  return NEKO_KEY_4;
        case 0x35:                  return NEKO_KEY_5;
        case 0x36:                  return NEKO_KEY_6;
        case 0x37:                  return NEKO_KEY_7;
        case 0x38:                  return NEKO_KEY_8;
        case 0x39:                  return NEKO_KEY_9;
        case VK_OEM_1:              return NEKO_KEY_SEMICOLON;
        case VK_OEM_PLUS:           return NEKO_KEY_EQUAL;
        case 0x41:                  return NEKO_KEY_A;
        case 0x42:                  return NEKO_KEY_B;
        case 0x43:                  return NEKO_KEY_C;
        case 0x44:                  return NEKO_KEY_D;
        case 0x45:                  return NEKO_KEY_E;
        case 0x46:                  return NEKO_KEY_F;
        case 0x47:                  return NEKO_KEY_G;
        case 0x48:                  return NEKO_KEY_H;
        case 0x49:                  return NEKO_KEY_I;
        case 0x4A:                  return NEKO_KEY_J;
        case 0x4B:                  return NEKO_KEY_K;
        case 0x4C:                  return NEKO_KEY_L;
        case 0x4D:                  return NEKO_KEY_M;
        case 0x4E:                  return NEKO_KEY_N;
        case 0x4F:                  return NEKO_KEY_O;
        case 0x50:                  return NEKO_KEY_P;
        case 0x51:                  return NEKO_KEY_Q;
        case 0x52:                  return NEKO_KEY_R;
        case 0x53:                  return NEKO_KEY_S;
        case 0x54:                  return NEKO_KEY_T;
        case 0x55:                  return NEKO_KEY_U;
        case 0x56:                  return NEKO_KEY_V;
        case 0x57:                  return NEKO_KEY_W;
        case 0x58:                  return NEKO_KEY_X;
        case 0x59:                  return NEKO_KEY_Y;
        case 0x5A:                  return NEKO_KEY_Z;

            // Function keys
        case VK_ESCAPE:             return NEKO_KEY_ESCAPE;
        case VK_RETURN:             return NEKO_KEY_ENTER;
        case VK_TAB:                return NEKO_KEY_TAB;
        case VK_BACK:               return NEKO_KEY_BACKSPACE;
        case VK_INSERT:             return NEKO_KEY_INSERT;
        case VK_DELETE:             return NEKO_KEY_DELETE;
        case VK_RIGHT:              return NEKO_KEY_RIGHT;
        case VK_LEFT:               return NEKO_KEY_LEFT;
        case VK_DOWN:               return NEKO_KEY_DOWN;
        case VK_UP:                 return NEKO_KEY_UP;
        case VK_PRIOR:              return NEKO_KEY_PAGE_UP;
        case VK_NEXT:               return NEKO_KEY_PAGE_DOWN;
        case VK_HOME:               return NEKO_KEY_HOME;
        case VK_END:                return NEKO_KEY_END;
        case VK_CAPITAL:            return NEKO_KEY_CAPS_LOCK;
        case VK_SCROLL:             return NEKO_KEY_SCROLL_LOCK;
        case VK_NUMLOCK:            return NEKO_KEY_NUM_LOCK;
        case VK_SNAPSHOT:           return NEKO_KEY_PRINT_SCREEN;
        case VK_PAUSE:              return NEKO_KEY_PAUSE;
        case VK_F1:                 return NEKO_KEY_F1;
        case VK_F2:                 return NEKO_KEY_F2;
        case VK_F3:                 return NEKO_KEY_F3;
        case VK_F4:                 return NEKO_KEY_F4;
        case VK_F5:                 return NEKO_KEY_F5;
        case VK_F6:                 return NEKO_KEY_F6;
        case VK_F7:                 return NEKO_KEY_F7;
        case VK_F8:                 return NEKO_KEY_F8;
        case VK_F9:                 return NEKO_KEY_F9;
        case VK_F10:                return NEKO_KEY_F10;
        case VK_F11:                return NEKO_KEY_F11;
        case VK_F12:                return NEKO_KEY_F12;
        case VK_F13:                return NEKO_KEY_F13;
        case VK_F14:                return NEKO_KEY_F14;
        case VK_F15:                return NEKO_KEY_F15;
        case VK_F16:                return NEKO_KEY_F16;
        case VK_F17:                return NEKO_KEY_F17;
        case VK_F18:                return NEKO_KEY_F18;
        case VK_F19:                return NEKO_KEY_F19;
        case VK_F20:                return NEKO_KEY_F20;
        case VK_F21:                return NEKO_KEY_F21;
        case VK_F22:                return NEKO_KEY_F22;
        case VK_F23:                return NEKO_KEY_F23;
        case VK_F24:                return NEKO_KEY_F24;
        case VK_NUMPAD0:            return NEKO_KEY_NP_0;
        case VK_NUMPAD1:            return NEKO_KEY_NP_1;
        case VK_NUMPAD2:            return NEKO_KEY_NP_2;
        case VK_NUMPAD3:            return NEKO_KEY_NP_3;
        case VK_NUMPAD4:            return NEKO_KEY_NP_4;
        case VK_NUMPAD5:            return NEKO_KEY_NP_5;
        case VK_NUMPAD6:            return NEKO_KEY_NP_6;
        case VK_NUMPAD7:            return NEKO_KEY_NP_7;
        case VK_NUMPAD8:            return NEKO_KEY_NP_8;
        case VK_NUMPAD9:            return NEKO_KEY_NP_9;
        case VK_DECIMAL:            return NEKO_KEY_NP_DECIMAL;
        case VK_DIVIDE:             return NEKO_KEY_NP_DIVIDE;
        case VK_MULTIPLY:           return NEKO_KEY_NP_MULTIPLY;
        case VK_SUBTRACT:           return NEKO_KEY_NP_SUBTRACT;
        case VK_ADD:                return NEKO_KEY_NP_ADD;
        case VK_LSHIFT:             return NEKO_KEY_LEFT_SHIFT;
        case VK_LCONTROL:           return NEKO_KEY_LEFT_CTRL;
        case VK_CONTROL:            return NEKO_KEY_LEFT_CTRL;
        case VK_LMENU:              return NEKO_KEY_LEFT_ALT;
        case VK_LWIN:               return NEKO_KEY_LEFT_SUPER;
        case VK_RSHIFT:             return NEKO_KEY_RIGHT_SHIFT;
        case VK_RCONTROL:           return NEKO_KEY_RIGHT_CTRL;
        case VK_RMENU:              return NEKO_KEY_RIGHT_ALT;
        case VK_RWIN:               return NEKO_KEY_RIGHT_SUPER;
        case VK_MENU:               return NEKO_KEY_MENU;
        default:                    return NEKO_HID_UNKNOWN;
    }
}


neko_HidEvent neko_TranslateNativeMouseCode(uint16_t _code) {
    switch (_code) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:          return NEKO_MOUSE_BTN_1;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:          return NEKO_MOUSE_BTN_2;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:          return NEKO_MOUSE_BTN_3;
        default:                    return NEKO_HID_UNKNOWN;
    }
}
#elif defined(__linux__)
neko_HidEvent neko_TranslateNativeKeyCode(uint32_t _code) {
    switch (_code) {
        // X11 keys 
        case XK_space:              return NEKO_KEY_SPACE;
        case XK_apostrophe:         return NEKO_KEY_APOSTROPHE;
        case XK_comma:              return NEKO_KEY_COMMA;
        case XK_minus:              return NEKO_KEY_MINUS;
        case XK_period:             return NEKO_KEY_DOT;
        case XK_slash:              return NEKO_KEY_SLASH;
        case XK_0:                  return NEKO_KEY_0;
        case XK_1:                  return NEKO_KEY_1;
        case XK_2:                  return NEKO_KEY_2;
        case XK_3:                  return NEKO_KEY_3;
        case XK_4:                  return NEKO_KEY_4;
        case XK_5:                  return NEKO_KEY_5;
        case XK_6:                  return NEKO_KEY_6;
        case XK_7:                  return NEKO_KEY_7;
        case XK_8:                  return NEKO_KEY_8;
        case XK_9:                  return NEKO_KEY_9;
        case XK_semicolon:          return NEKO_KEY_SEMICOLON;
        case XK_equal:              return NEKO_KEY_EQUAL;
        case XK_a:                  return NEKO_KEY_A;
        case XK_b:                  return NEKO_KEY_B;
        case XK_c:                  return NEKO_KEY_C;
        case XK_d:                  return NEKO_KEY_D;
        case XK_e:                  return NEKO_KEY_E;
        case XK_f:                  return NEKO_KEY_F;
        case XK_g:                  return NEKO_KEY_G;
        case XK_h:                  return NEKO_KEY_H;
        case XK_i:                  return NEKO_KEY_I;
        case XK_j:                  return NEKO_KEY_J;
        case XK_k:                  return NEKO_KEY_K;
        case XK_l:                  return NEKO_KEY_L;
        case XK_m:                  return NEKO_KEY_M;
        case XK_n:                  return NEKO_KEY_N;
        case XK_o:                  return NEKO_KEY_O;
        case XK_p:                  return NEKO_KEY_P;
        case XK_q:                  return NEKO_KEY_Q;
        case XK_r:                  return NEKO_KEY_R;
        case XK_s:                  return NEKO_KEY_S;
        case XK_t:                  return NEKO_KEY_T;
        case XK_u:                  return NEKO_KEY_U;
        case XK_v:                  return NEKO_KEY_V;
        case XK_w:                  return NEKO_KEY_W;
        case XK_x:                  return NEKO_KEY_X;
        case XK_y:                  return NEKO_KEY_Y;
        case XK_z:                  return NEKO_KEY_Z;

            // Function keys
        case XK_Escape:             return NEKO_KEY_ESCAPE;
        case XK_Return:             return NEKO_KEY_ENTER;
        case XK_Tab:                return NEKO_KEY_TAB;
        case XK_BackSpace:          return NEKO_KEY_BACKSPACE;
        case XK_Insert:             return NEKO_KEY_INSERT;
        case XK_Delete:             return NEKO_KEY_DELETE;
        case XK_Right:              return NEKO_KEY_RIGHT;
        case XK_Left:               return NEKO_KEY_LEFT;
        case XK_Down:               return NEKO_KEY_DOWN;
        case XK_Up:                 return NEKO_KEY_UP;
        case XK_Page_Up:            return NEKO_KEY_PAGE_UP;
        case XK_Page_Down:          return NEKO_KEY_PAGE_DOWN;
        case XK_Home:               return NEKO_KEY_HOME;
        case XK_End:                return NEKO_KEY_END;
        case XK_Caps_Lock:          return NEKO_KEY_CAPS_LOCK;
        case XK_Scroll_Lock:        return NEKO_KEY_SCROLL_LOCK;
        case XK_Num_Lock:           return NEKO_KEY_NUM_LOCK;
        case XK_Print:              return NEKO_KEY_PRINT_SCREEN;
        case XK_Pause:              return NEKO_KEY_PAUSE;
        case XK_F1:                 return NEKO_KEY_F1;
        case XK_F2:                 return NEKO_KEY_F2;
        case XK_F3:                 return NEKO_KEY_F3;
        case XK_F4:                 return NEKO_KEY_F4;
        case XK_F5:                 return NEKO_KEY_F5;
        case XK_F6:                 return NEKO_KEY_F6;
        case XK_F7:                 return NEKO_KEY_F7;
        case XK_F8:                 return NEKO_KEY_F8;
        case XK_F9:                 return NEKO_KEY_F9;
        case XK_F10:                return NEKO_KEY_F10;
        case XK_F11:                return NEKO_KEY_F11;
        case XK_F12:                return NEKO_KEY_F12;
        case XK_F13:                return NEKO_KEY_F13;
        case XK_F14:                return NEKO_KEY_F14;
        case XK_F15:                return NEKO_KEY_F15;
        case XK_F16:                return NEKO_KEY_F16;
        case XK_F17:                return NEKO_KEY_F17;
        case XK_F18:                return NEKO_KEY_F18;
        case XK_F19:                return NEKO_KEY_F19;
        case XK_F20:                return NEKO_KEY_F20;
        case XK_F21:                return NEKO_KEY_F21;
        case XK_F22:                return NEKO_KEY_F22;
        case XK_F23:                return NEKO_KEY_F23;
        case XK_F24:                return NEKO_KEY_F24;
        case XK_F25:                return NEKO_KEY_F25;
        case XK_KP_0:               return NEKO_KEY_NP_0;
        case XK_KP_1:               return NEKO_KEY_NP_1;
        case XK_KP_2:               return NEKO_KEY_NP_2;
        case XK_KP_3:               return NEKO_KEY_NP_3;
        case XK_KP_4:               return NEKO_KEY_NP_4;
        case XK_KP_5:               return NEKO_KEY_NP_5;
        case XK_KP_6:               return NEKO_KEY_NP_6;
        case XK_KP_7:               return NEKO_KEY_NP_7;
        case XK_KP_8:               return NEKO_KEY_NP_8;
        case XK_KP_9:               return NEKO_KEY_NP_9;
        case XK_KP_Decimal:         return NEKO_KEY_NP_DECIMAL;
        case XK_KP_Divide:          return NEKO_KEY_NP_DIVIDE;
        case XK_KP_Multiply:        return NEKO_KEY_NP_MULTIPLY;
        case XK_KP_Add:             return NEKO_KEY_NP_ADD;
        case XK_KP_Enter:           return NEKO_KEY_NP_ENTER;
        case XK_KP_Equal:           return NEKO_KEY_NP_EQUAL;
        case XK_Shift_L:            return NEKO_KEY_LEFT_SHIFT;
        case XK_Control_L:          return NEKO_KEY_LEFT_CTRL;
        case XK_Alt_L:              return NEKO_KEY_LEFT_ALT;
        case XK_Super_L:            return NEKO_KEY_LEFT_SUPER;
        case XK_Shift_R:            return NEKO_KEY_RIGHT_SHIFT;
        case XK_Control_R:          return NEKO_KEY_RIGHT_CTRL;
        case XK_Alt_R:              return NEKO_KEY_RIGHT_ALT;
        case XK_Super_R:            return NEKO_KEY_RIGHT_SUPER;
        case XK_Menu:               return NEKO_KEY_MENU;

        default:                    return NEKO_HID_UNKNOWN;
    }
}


neko_HidEvent neko_TranslateNativeMouseCode(uint16_t _code) {
    switch (button) {
        case Button1: return NEKO_MOUSE_BTN_1;
        case Button2: return NEKO_MOUSE_BTN_2;
        case Button3: return NEKO_MOUSE_BTN_3;
        case Button4: return NEKO_MOUSE_SCROLL_UP;
        case Button5: return NEKO_MOUSE_SCROLL_DOWN;
        
        default: return NEKO_HID_UNKNOWN;
    }
}
#endif

/// Register new input event to input table
/// This function is meant to be called only by DENG platform dependant surface instances
void _neko_RegisterInputEvent(neko_Input *_input, neko_HidEvent _event, bool _active) {
    if(_active) {
        _input->raw.active_table[_event] = true;
        _input->raw.released_table[_event] = false;
    } else if(_event < NEKO_MOUSE_SCROLL_DOWN || _event > NEKO_MOUSE_SCROLL_UP){
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
