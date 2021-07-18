/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: key_definitions.h - keycode definitions' header
/// author: Karl-Mihkel Ott

#ifndef NEKO_KEY_DEFINITIONS_H
#define NEKO_KEY_DEFINITIONS_H

#define NEKO_INPUT_EV_COUNT 126

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __KEY_TRANSLATION_C
    #include <stdint.h>
#endif

#ifdef __linux__
    #include <X11/XKBlib.h>
    #include <X11/keysym.h>
#endif

#ifdef _WIN32 
    #include <windows.h>
#endif


typedef uint16_t neko_Hint;
#define NEKO_HINT_API_OPENGL    0x0001
#define NEKO_HINT_API_VULKAN    0x0002
#define NEKO_HINT_FULL_SCREEN   0x0004
#define NEKO_HINT_FIXED_SIZE    0x0008
#define NEKO_HINT_RESIZEABLE    0x0010


/// Specify input event type for lookup
typedef enum neko_InputEventType {
    NEKO_INPUT_EVENT_TYPE_ACTIVE    = 0,
    NEKO_INPUT_EVENT_TYPE_RELEASED  = 1,
    NEKO_INPUT_EVENT_TYPE_FIRST     = 0,
    NEKO_INPUT_EVENT_TYPE_LAST      = 1
} neko_InputEventType;


/// Specify input device type for action lookup
typedef enum neko_InputType {
    NEKO_INPUT_TYPE_MOUSE   = 0,
    NEKO_INPUT_TYPE_KB      = 1,
    NEKO_INPUT_TYPE_FIRST   = 0,
    NEKO_INPUT_TYPE_LAST    = 1
} neko_InputType;


/// Specify what should happen if virtual cursor achieves 
/// its maximum or minimum position 
typedef enum neko_VCPOverflowAction {
    NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION   = 0,
    NEKO_VCP_OVERFLOW_ACTION_BLOCK_POSITION         = 1,
    NEKO_VCP_OVERFLOW_ACTION_NONE                   = 2,
    NEKO_VCP_OVERFLOW_FIRST                         = NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION,
    NEKO_VCP_OVERFLOW_LAST                          = NEKO_VCP_OVERFLOW_ACTION_BLOCK_POSITION
} neko_VCPOverflowAction;


/// Mouse mode specifier enum
typedef enum neko_MouseMode {
    NEKO_MOUSE_MODE_UNKNOWN         = -1,
    NEKO_MOUSE_MODE_INVISIBLE       = 0,
    NEKO_MOUSE_MODE_CURSOR_VISIBLE  = 1,
    NEKO_MOUSE_MODE_FIRST           = -1,
    NEKO_MOUSE_MODE_LAST            = 1
} neko_MouseMode;


/// Keyboard key event specifier enum
typedef enum neko_Key {
    NEKO_KEY_UNKNOWN        =   0,
    NEKO_KEY_SPACE          =   1,
    NEKO_KEY_APOSTROPHE     =   2,
    NEKO_KEY_COMMA          =   3,
    NEKO_KEY_MINUS          =   4,
    NEKO_KEY_DOT            =   5,
    NEKO_KEY_SLASH          =   6,
    NEKO_KEY_0              =   7,
    NEKO_KEY_1              =   8,
    NEKO_KEY_2              =   9,
    NEKO_KEY_3              =   10,
    NEKO_KEY_4              =   11,
    NEKO_KEY_5              =   12,
    NEKO_KEY_6              =   13,
    NEKO_KEY_7              =   14,
    NEKO_KEY_8              =   15,
    NEKO_KEY_9              =   16,
    NEKO_KEY_SEMICOLON      =   17,
    NEKO_KEY_EQUAL          =   18,
    NEKO_KEY_A              =   19,
    NEKO_KEY_B              =   20,
    NEKO_KEY_C              =   21,
    NEKO_KEY_D              =   22,
    NEKO_KEY_E              =   23,
    NEKO_KEY_F              =   24,
    NEKO_KEY_G              =   25,
    NEKO_KEY_H              =   26,
    NEKO_KEY_I              =   27,
    NEKO_KEY_J              =   28,
    NEKO_KEY_K              =   29,
    NEKO_KEY_L              =   30,
    NEKO_KEY_M              =   31,
    NEKO_KEY_N              =   32,
    NEKO_KEY_O              =   33,
    NEKO_KEY_P              =   34,
    NEKO_KEY_Q              =   35,
    NEKO_KEY_R              =   36,
    NEKO_KEY_S              =   37,
    NEKO_KEY_T              =   38,
    NEKO_KEY_U              =   39,
    NEKO_KEY_V              =   40,
    NEKO_KEY_W              =   41,
    NEKO_KEY_X              =   42,
    NEKO_KEY_Y              =   43,
    NEKO_KEY_Z              =   44,
    NEKO_KEY_ESCAPE         =   45,
    NEKO_KEY_ENTER          =   46,
    NEKO_KEY_TAB            =   47,
    NEKO_KEY_BACKSPACE      =   48,
    NEKO_KEY_INSERT         =   49,
    NEKO_KEY_DELETE         =   50,
    NEKO_KEY_RIGHT          =   51,
    NEKO_KEY_LEFT           =   52,
    NEKO_KEY_DOWN           =   53,
    NEKO_KEY_UP             =   54,
    NEKO_KEY_PAGE_UP        =   55,
    NEKO_KEY_PAGE_DOWN      =   56,
    NEKO_KEY_HOME           =   57,
    NEKO_KEY_END            =   58,
    NEKO_KEY_CAPS_LOCK      =   59,
    NEKO_KEY_SCROLL_LOCK    =   60,
    NEKO_KEY_NUM_LOCK       =   61,
    NEKO_KEY_PRINT_SCREEN   =   62,
    NEKO_KEY_PAUSE          =   63,
    NEKO_KEY_F1             =   64,
    NEKO_KEY_F2             =   65,
    NEKO_KEY_F3             =   66,
    NEKO_KEY_F4             =   67,
    NEKO_KEY_F5             =   68,
    NEKO_KEY_F6             =   69,
    NEKO_KEY_F7             =   70,
    NEKO_KEY_F8             =   71,
    NEKO_KEY_F9             =   72,
    NEKO_KEY_F10            =   73,
    NEKO_KEY_F11            =   74,
    NEKO_KEY_F12            =   75,
    NEKO_KEY_F13            =   76,
    NEKO_KEY_F14            =   77,
    NEKO_KEY_F15            =   78,
    NEKO_KEY_F16            =   79,
    NEKO_KEY_F17            =   80,
    NEKO_KEY_F18            =   81,
    NEKO_KEY_F19            =   82,
    NEKO_KEY_F20            =   83,
    NEKO_KEY_F21            =   84,
    NEKO_KEY_F22            =   85,
    NEKO_KEY_F23            =   86,
    NEKO_KEY_F24            =   87,
    NEKO_KEY_F25            =   88,
    NEKO_KEY_NP_0           =   89,
    NEKO_KEY_NP_1           =   90,
    NEKO_KEY_NP_2           =   91,
    NEKO_KEY_NP_3           =   92,
    NEKO_KEY_NP_4           =   93,
    NEKO_KEY_NP_5           =   94,
    NEKO_KEY_NP_6           =   95,
    NEKO_KEY_NP_7           =   96,
    NEKO_KEY_NP_8           =   97,
    NEKO_KEY_NP_9           =   98,
    NEKO_KEY_NP_DECIMAL     =   99,
    NEKO_KEY_NP_DIVIDE      =   100,
    NEKO_KEY_NP_MULTIPLY    =   101,
    NEKO_KEY_NP_SUBTRACT    =   102,
    NEKO_KEY_NP_ADD         =   103,
    NEKO_KEY_NP_ENTER       =   104,
    NEKO_KEY_NP_EQUAL       =   105,
    NEKO_KEY_LEFT_SHIFT     =   106,
    NEKO_KEY_LEFT_CTRL      =   107,
    NEKO_KEY_LEFT_ALT       =   108,
    NEKO_KEY_LEFT_SUPER     =   109,
    NEKO_KEY_RIGHT_SHIFT    =   110,
    NEKO_KEY_RIGHT_CTRL     =   111,
    NEKO_KEY_RIGHT_ALT      =   112,
    NEKO_KEY_RIGHT_SUPER    =   113,
    NEKO_KEY_MENU           =   114,
    NEKO_KEY_FIRST          =   NEKO_KEY_UNKNOWN,
    NEKO_KEY_LAST           =   NEKO_KEY_MENU
} neko_Key;


/// Mouse button event specifier enum
typedef enum neko_MouseButton {
    NEKO_MOUSE_BTN_UNKNOWN  = 115,
    NEKO_MOUSE_BTN_1        = 116,
    NEKO_MOUSE_BTN_2        = 117,
    NEKO_MOUSE_BTN_3        = 118,
    NEKO_MOUSE_SCROLL_DOWN  = 119,
    NEKO_MOUSE_SCROLL_UP    = 120,
    NEKO_MOUSE_BTN_FIRST    = NEKO_MOUSE_BTN_UNKNOWN,
    NEKO_MOUSE_BTN_LAST     = NEKO_MOUSE_SCROLL_UP
} neko_MouseButton;


/// These values are used in case delta mouse movement is detected
typedef enum neko_MouseDeltaMovement {
    NEKO_MOUSE_DELTA_UNKNOWN    = 121,
    NEKO_MOUSE_DELTA_X          = 122,
    NEKO_MOUSE_DELTA_NX         = 123,
    NEKO_MOUSE_DELTA_Y          = 124,
    NEKO_MOUSE_DELTA_NY         = 125,
    NEKO_MOUSE_DELTA_FIRST      = NEKO_MOUSE_DELTA_UNKNOWN,
    NEKO_MOUSE_DELTA_LAST       = NEKO_MOUSE_DELTA_NY
} neko_MouseDeltaMovement;


/// NOTE: In DENG all active input device events are masked into a single 64bit integer with the input event index being
/// 8 bits wide. This means that the 64 bit integer can store up to 8 different masked events, which allows to create
/// keybindings with up to 8 keys very efficiently.


/// Universal union for specifying input information
typedef union {
    neko_Key key;
    neko_MouseButton btn;
    neko_MouseDeltaMovement md_mov;
} neko_InputEv;


/// Universal input event type for general purpose input devices
typedef uint64_t neko_InputBits;
#define MAX_KEY_COMBO   8


#ifdef __linux__
    neko_Key translateX11Key(KeySym keysym);
    neko_MouseButton translateX11Btn(uint32_t button);
#endif

#ifdef WIN32
    neko_Key translateWIN32Key(uint32_t key_code);
    neko_MouseButton translateWIN32Btn(UINT msg);
#endif

#ifdef __cplusplus
}
#endif

#endif
