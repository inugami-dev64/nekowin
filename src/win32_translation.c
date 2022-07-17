/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: win32_translation.c - win32 event translation source file
/// author: Karl-Mihkel Ott

#define WIN32_TRANSLATION_C
#include "win32_translation.h"

neko_HidEvent _neko_TranslateWin32Key(WPARAM _key) {
    switch (_key) {
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


neko_HidEvent _neko_TranslateWin32Btn(UINT _btn) {
    switch (_btn) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:          return NEKO_MOUSE_BTN_1;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:          return NEKO_MOUSE_BTN_2;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:          return NEKO_MOUSE_BTN_3;
        default:                    return NEKO_HID_UNKNOWN;
    }
}