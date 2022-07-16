/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: x11_translation.h - Xlib input symbol to neko_HidEvent translator source file
/// author: Karl-Mihkel Ott

#define X11_TRANSLATION_C
#include <x11_translation.h>

neko_HidEvent _neko_TranslateKeyX11(KeySym _sym) {
    switch (_sym) {   
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


neko_HidEvent _neko_TranslateMouseBtnX11(uint32_t button) {
    switch (button) {
        case Button1: return NEKO_MOUSE_BTN_1;
        case Button2: return NEKO_MOUSE_BTN_2;
        case Button3: return NEKO_MOUSE_BTN_3;
        case Button4: return NEKO_MOUSE_SCROLL_UP;
        case Button5: return NEKO_MOUSE_SCROLL_DOWN;
        
        default: return NEKO_HID_UNKNOWN;
    }
}