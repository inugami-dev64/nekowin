/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: input.h - neko cross-platform input API header file 
/// author: Karl-Mihkel Ott

#ifndef INPUT_H
#define INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(INPUT_C)
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

    #include "nwin/nekodll.h"
#endif


/// HID event enum
typedef enum _neko_HidEvent {
	NEKO_HID_UNKNOWN,
	NEKO_KEY_SPACE,
	NEKO_KEY_APOSTROPHE,
	NEKO_KEY_COMMA,
	NEKO_KEY_MINUS,
	NEKO_KEY_DOT,
	NEKO_KEY_SLASH,
	NEKO_KEY_0,
	NEKO_KEY_1,
	NEKO_KEY_2,
	NEKO_KEY_3,
	NEKO_KEY_4,
	NEKO_KEY_5,
	NEKO_KEY_6,
	NEKO_KEY_7,
	NEKO_KEY_8,
	NEKO_KEY_9,
	NEKO_KEY_SEMICOLON,
	NEKO_KEY_EQUAL,
	NEKO_KEY_A,
	NEKO_KEY_B,
	NEKO_KEY_C,
	NEKO_KEY_D,
	NEKO_KEY_E,
	NEKO_KEY_F,
	NEKO_KEY_G,
	NEKO_KEY_H,
	NEKO_KEY_I,
	NEKO_KEY_J,
	NEKO_KEY_K,
	NEKO_KEY_L,
	NEKO_KEY_M,
	NEKO_KEY_N,
	NEKO_KEY_O,
	NEKO_KEY_P,
	NEKO_KEY_Q,
	NEKO_KEY_R,
	NEKO_KEY_S,
	NEKO_KEY_T,
	NEKO_KEY_U,
	NEKO_KEY_V,
	NEKO_KEY_W,
	NEKO_KEY_X,
	NEKO_KEY_Y,
	NEKO_KEY_Z,
    NEKO_KEY_LEFT_BRACKET,      // '['
    NEKO_KEY_RIGHT_BRACKER,     // ']'
    NEKO_KEY_BACKSLASH,         // '\'
    NEKO_KEY_GRAVE_ACCENT,      // '`'
	NEKO_KEY_ESCAPE,
	NEKO_KEY_ENTER,
	NEKO_KEY_TAB,
	NEKO_KEY_BACKSPACE,
	NEKO_KEY_INSERT,
	NEKO_KEY_DELETE,
	NEKO_KEY_RIGHT,
	NEKO_KEY_LEFT,
	NEKO_KEY_DOWN,
	NEKO_KEY_UP,
	NEKO_KEY_PAGE_UP,
	NEKO_KEY_PAGE_DOWN,
	NEKO_KEY_HOME,
	NEKO_KEY_END,
	NEKO_KEY_CAPS_LOCK,
	NEKO_KEY_SCROLL_LOCK,
	NEKO_KEY_NUM_LOCK,
	NEKO_KEY_PRINT_SCREEN,
	NEKO_KEY_PAUSE,
	NEKO_KEY_F1,
	NEKO_KEY_F2,
	NEKO_KEY_F3,
	NEKO_KEY_F4,
	NEKO_KEY_F5,
	NEKO_KEY_F6,
	NEKO_KEY_F7,
	NEKO_KEY_F8,
	NEKO_KEY_F9,
	NEKO_KEY_F10,
	NEKO_KEY_F11,
	NEKO_KEY_F12,
	NEKO_KEY_F13,
	NEKO_KEY_F14,
	NEKO_KEY_F15,
	NEKO_KEY_F16,
	NEKO_KEY_F17,
	NEKO_KEY_F18,
	NEKO_KEY_F19,
	NEKO_KEY_F20,
	NEKO_KEY_F21,
	NEKO_KEY_F22,
	NEKO_KEY_F23,
	NEKO_KEY_F24,
	NEKO_KEY_F25,
	NEKO_KEY_NP_0,
	NEKO_KEY_NP_1,
	NEKO_KEY_NP_2,
	NEKO_KEY_NP_3,
	NEKO_KEY_NP_4,
	NEKO_KEY_NP_5,
	NEKO_KEY_NP_6,
	NEKO_KEY_NP_7,
	NEKO_KEY_NP_8,
	NEKO_KEY_NP_9,
	NEKO_KEY_NP_DECIMAL,
	NEKO_KEY_NP_DIVIDE,
	NEKO_KEY_NP_MULTIPLY,
	NEKO_KEY_NP_SUBTRACT,
	NEKO_KEY_NP_ADD,
	NEKO_KEY_NP_ENTER,
	NEKO_KEY_NP_EQUAL,
	NEKO_KEY_LEFT_SHIFT,
	NEKO_KEY_LEFT_CTRL,
	NEKO_KEY_LEFT_ALT,
	NEKO_KEY_LEFT_SUPER,
	NEKO_KEY_RIGHT_SHIFT,
	NEKO_KEY_RIGHT_CTRL,
	NEKO_KEY_RIGHT_ALT,
	NEKO_KEY_RIGHT_SUPER,
	NEKO_KEY_MENU,
    NEKO_KEY_MAX,

    /// Mouse buttons
	NEKO_MOUSE_BTN_1,
	NEKO_MOUSE_BTN_2,
	NEKO_MOUSE_BTN_3,
	NEKO_MOUSE_SCROLL_DOWN,
	NEKO_MOUSE_SCROLL_UP,
    NEKO_MOUSE_MAX,

    /// Mouse delta movements
	NEKO_MOUSE_DELTA_X,
	NEKO_MOUSE_DELTA_NX,
	NEKO_MOUSE_DELTA_Y,
	NEKO_MOUSE_DELTA_NY,
    NEKO_MOUSE_DELTA_MAX,
    
    /// Traditional gamepad layout (reserved for future)
    NEKO_GAMEPAD_BUTTON_A,
    NEKO_GAMEPAD_BUTTON_B,
    NEKO_GAMEPAD_BUTTON_X,
    NEKO_GAMEPAD_BUTTON_Y,
    NEKO_GAMEPAD_BUTTON_LB,
    NEKO_GAMEPAD_BUTTON_RB,
    NEKO_GAMEPAD_BUTTON_LT,
    NEKO_GAMEPAD_BUTTON_RT,
    NEKO_GAMEPAD_BUTTON_DPAD_UP,
    NEKO_GAMEPAD_BUTTON_DPAD_RIGHT,
    NEKO_GAMEPAD_BUTTON_DPAD_LEFT,
    NEKO_GAMEPAD_BUTTON_DPAD_DOWN,
    NEKO_GAMEPAD_JOYSTICK_1,
    NEKO_GAMEPAD_JOYSTICK_2,
    NEKO_GAMEPAD_MAX,
    NEKO_HID_EVENT_COUNT,
} neko_HidEvent;


/// Enumeral values for nekowin cursors
typedef enum _neko_CursorMode {
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
#define NEKO_MAX_INPUT_BIT_COMBO     8

typedef struct _EventQueue {
    neko_HidEvent events[NEKO_HID_EVENT_COUNT];
    uint32_t size;
} EventQueue;


typedef enum _VirtualCursorOverflowAction {
	NEKO_VIRTUAL_CURSOR_OVERFLOW_ACTION_BLOCK,
	NEKO_VIRTUAL_CURSOR_OVERFLOW_ACTION_OVERWRITE
} VirtualCursorOverflowAction;


/// Input event data structure for storing information about retrieved inputs
typedef struct _neko_Input {
	struct {
		int64_t x;
		int64_t y;
		int64_t orig_x;
		int64_t orig_y;

		// Virtual cursor boundaries
		int64_t max_vc_x;
		int64_t max_vc_y;
		int64_t min_vc_x;
		int64_t min_vc_y;
		VirtualCursorOverflowAction x_overflow;
		VirtualCursorOverflowAction y_overflow;
		
		// Flag
		bool is_virtual;
	} cursor;
	
	// Keyboard events are categorized into raw and text modes 
	struct {
        bool active_table[NEKO_HID_EVENT_COUNT];
        bool released_table[NEKO_HID_EVENT_COUNT];
        EventQueue active_queue;
        EventQueue released_queue;
    } raw;

    struct {
        uint16_t ucs[NEKO_HID_EVENT_COUNT];
		uint32_t size;
    } text;

	bool use_text_mode;
} neko_Input;


#if defined(X11_WINDOW_C) || defined(WIN32_WINDOW_C)
    /// Register new keyevent to key register
    /// This function is meant to be called only by DENG platform dependant surface instances
    void _neko_RegisterInputEvent(neko_Input *_input, neko_HidEvent event, bool _active);


    /// Clean released key and mouse button array 
    void _neko_ClearReleasedInputs();
#endif


/// Create new input bitmask from multiple input events
LIBNWIN_API neko_InputBits neko_CreateInputMask(neko_HidEvent evs[8]);


/// Unmask neko_InputBits instance and return a static array of neko_HidEvent, whose size is exactly 8.
/// NOTE: Array elements that have no event attached use NEKO_KEY_UNKNOWN as a value
LIBNWIN_API neko_HidEvent *neko_UnmaskInput(neko_InputBits _bits);
#ifdef __cplusplus
}
#endif

#endif

