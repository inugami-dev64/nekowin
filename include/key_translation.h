/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: key_definitions.h - keycode definitions' header
/// author: Karl-Mihkel Ott

#ifndef _KEY_TRANSLATION_H
#define _KEY_TRANSLATION_H

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
#define NEKO_HINT_API_OPENGL        0x0001
#define NEKO_HINT_API_VULKAN        0x0002
#define NEKO_HINT_FULL_SCREEN       0x0004
#define NEKO_HINT_FIXED_SIZE        0x0008
#define NEKO_HINT_RESIZEABLE        0x0010
#define NEKO_HINT_NO_FULL_SCREEN    0x0010


/// All size hints
#define NEKO_HINT_SIZE_HINTS        NEKO_HINT_FULL_SCREEN | NEKO_HINT_RESIZEABLE | NEKO_HINT_NO_FULL_SCREEN | NEKO_HINT_FIXED_SIZE


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


/// HID event numbers
typedef uint8_t neko_HidEvent;
#define NEKO_HID_UNKNOWN        0
#define NEKO_KEY_SPACE          1
#define NEKO_KEY_APOSTROPHE     2
#define NEKO_KEY_COMMA          3
#define NEKO_KEY_MINUS          4
#define NEKO_KEY_DOT            5
#define NEKO_KEY_SLASH          6
#define NEKO_KEY_0              7
#define NEKO_KEY_1              8
#define NEKO_KEY_2              9
#define NEKO_KEY_3              10
#define NEKO_KEY_4              11
#define NEKO_KEY_5              12
#define NEKO_KEY_6              13
#define NEKO_KEY_7              14
#define NEKO_KEY_8              15
#define NEKO_KEY_9              16
#define NEKO_KEY_SEMICOLON      17
#define NEKO_KEY_EQUAL          18
#define NEKO_KEY_A              19
#define NEKO_KEY_B              20
#define NEKO_KEY_C              21
#define NEKO_KEY_D              22
#define NEKO_KEY_E              23
#define NEKO_KEY_F              24
#define NEKO_KEY_G              25
#define NEKO_KEY_H              26
#define NEKO_KEY_I              27
#define NEKO_KEY_J              28
#define NEKO_KEY_K              29
#define NEKO_KEY_L              30
#define NEKO_KEY_M              31
#define NEKO_KEY_N              32
#define NEKO_KEY_O              33
#define NEKO_KEY_P              34
#define NEKO_KEY_Q              35
#define NEKO_KEY_R              36
#define NEKO_KEY_S              37
#define NEKO_KEY_T              38
#define NEKO_KEY_U              39
#define NEKO_KEY_V              40
#define NEKO_KEY_W              41
#define NEKO_KEY_X              42
#define NEKO_KEY_Y              43
#define NEKO_KEY_Z              44
#define NEKO_KEY_ESCAPE         45
#define NEKO_KEY_ENTER          46
#define NEKO_KEY_TAB            47
#define NEKO_KEY_BACKSPACE      48
#define NEKO_KEY_INSERT         49
#define NEKO_KEY_DELETE         50
#define NEKO_KEY_RIGHT          51
#define NEKO_KEY_LEFT           52
#define NEKO_KEY_DOWN           53
#define NEKO_KEY_UP             54
#define NEKO_KEY_PAGE_UP        55
#define NEKO_KEY_PAGE_DOWN      56
#define NEKO_KEY_HOME           57
#define NEKO_KEY_END            58
#define NEKO_KEY_CAPS_LOCK      59
#define NEKO_KEY_SCROLL_LOCK    60
#define NEKO_KEY_NUM_LOCK       61
#define NEKO_KEY_PRINT_SCREEN   62
#define NEKO_KEY_PAUSE          63
#define NEKO_KEY_F1             64
#define NEKO_KEY_F2             65
#define NEKO_KEY_F3             66
#define NEKO_KEY_F4             67
#define NEKO_KEY_F5             68
#define NEKO_KEY_F6             69
#define NEKO_KEY_F7             70
#define NEKO_KEY_F8             71
#define NEKO_KEY_F9             72
#define NEKO_KEY_F10            73
#define NEKO_KEY_F11            74
#define NEKO_KEY_F12            75
#define NEKO_KEY_F13            76
#define NEKO_KEY_F14            77
#define NEKO_KEY_F15            78
#define NEKO_KEY_F16            79
#define NEKO_KEY_F17            80
#define NEKO_KEY_F18            81
#define NEKO_KEY_F19            82
#define NEKO_KEY_F20            83
#define NEKO_KEY_F21            84
#define NEKO_KEY_F22            85
#define NEKO_KEY_F23            86
#define NEKO_KEY_F24            87
#define NEKO_KEY_F25            88
#define NEKO_KEY_NP_0           89
#define NEKO_KEY_NP_1           90
#define NEKO_KEY_NP_2           91
#define NEKO_KEY_NP_3           92
#define NEKO_KEY_NP_4           93
#define NEKO_KEY_NP_5           94
#define NEKO_KEY_NP_6           95
#define NEKO_KEY_NP_7           96
#define NEKO_KEY_NP_8           97
#define NEKO_KEY_NP_9           98
#define NEKO_KEY_NP_DECIMAL     99
#define NEKO_KEY_NP_DIVIDE      100
#define NEKO_KEY_NP_MULTIPLY    101
#define NEKO_KEY_NP_SUBTRACT    102
#define NEKO_KEY_NP_ADD         103
#define NEKO_KEY_NP_ENTER       104
#define NEKO_KEY_NP_EQUAL       105
#define NEKO_KEY_LEFT_SHIFT     106
#define NEKO_KEY_LEFT_CTRL      107
#define NEKO_KEY_LEFT_ALT       108
#define NEKO_KEY_LEFT_SUPER     109
#define NEKO_KEY_RIGHT_SHIFT    110
#define NEKO_KEY_RIGHT_CTRL     111
#define NEKO_KEY_RIGHT_ALT      112
#define NEKO_KEY_RIGHT_SUPER    113
#define NEKO_KEY_MENU           114

/// Mouse buttons
#define NEKO_MOUSE_BTN_1        116
#define NEKO_MOUSE_BTN_2        117
#define NEKO_MOUSE_BTN_3        118
#define NEKO_MOUSE_SCROLL_DOWN  119
#define NEKO_MOUSE_SCROLL_UP    120

/// Mouse delta movements
#define NEKO_MOUSE_DELTA_UNKNOWN    121
#define NEKO_MOUSE_DELTA_X          122
#define NEKO_MOUSE_DELTA_NX         123
#define NEKO_MOUSE_DELTA_Y          124
#define NEKO_MOUSE_DELTA_NY         125


/// Enumeral values for nekowin cursors
typedef enum neko_CursorMode {
    NEKO_CURSOR_MODE_STANDARD,
    NEKO_CURSOR_MODE_WAITING,
    NEKO_CURSOR_MODE_POINTER,
    NEKO_CURSOR_MODE_HIDDEN
} neko_CursorMode;


/// NOTE: In DENG all active input device events are masked into a single 64bit integer with the input event index being
/// 8 bits wide. This means that the 64 bit integer can store up to 8 different masked events, which allows to create
/// keybindings with up to 8 keys very efficiently.


/// Universal input event type for general purpose input devices
typedef uint64_t neko_InputBits;
#define MAX_KEY_COMBO   8


#if defined(__linux__) && defined(__NWIN_C)
    neko_HidEvent _neko_TranslateX11Key(KeySym keysym);
    neko_HidEvent _neko_TranslateX11Btn(uint32_t button);
#elif defined(_WIN32) && defined(__NWIN_C)
    neko_HidEvent translateWIN32Key(uint16_t key_code);
    neko_HidEvent translateWIN32Btn(UINT msg);
#endif

#ifdef __cplusplus
}
#endif

#endif
