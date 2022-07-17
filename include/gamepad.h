/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: gamepad.h - platform independent gamepad input management header
/// author: Karl-Mihkel Ott

#ifndef GAMEPAD_H
#define GAMEPAD_H

// universal includes
#if defined(WIN32_GAMEPAD_C) || defined(X11_GAMEPAD_C)
	#include <stdint.h>
	#include <stdbool.h>
	#include "nekodll.h"
#endif

// platform specific includes and functions
#if defined(WIN32_GAMEPAD_C)
	#include <Windows.h>
	#include <Xinput.h>
#elif defined(X11_GAMEPAD_C)
#endif

// bitwise values for gamepad buttons
typedef uint16_t neko_GamepadButtonMask;
#define NEKO_GAMEPAD_BTN_DPAD_UP		0x0001
#define NEKO_GAMEPAD_BTN_DPAD_DOWN		0x0002
#define NEKO_GAMEPAD_BTN_DPAD_LEFT		0x0004
#define NEKO_GAMEPAD_BTN_DPAD_RIGHT		0x0008
#define NEKO_GAMEPAD_BTN_BACK			0x0010
#define NEKO_GAMEPAD_BTN_START			0x0020
#define NEKO_GAMEPAD_BTN_A				0x0040
#define NEKO_GAMEPAD_BTN_B				0x0080
#define NEKO_GAMEPAD_BTN_X				0x0100
#define NEKO_GAMEPAD_BTN_Y				0x0200
#define NEKO_GAMEPAD_BTN_LEFT_BUMPER	0x0400
#define NEKO_GAMEPAD_BTN_RIGHT_BUMPER	0x0800
#define NEKO_GAMEPAD_BTN_LEFT_STICK		0x1000
#define NEKO_GAMEPAD_BTN_RIGHT_STICK	0x2000

typedef struct _neko_Gamepad {
	neko_GamepadButtonMask buttons;
	uint8_t left_trigger;
	uint8_t right_trigger;
	struct {
		int16_t x;
		int16_t y;
	} left_thumb;
	struct {
		int16_t x;
		int16_t y; 
	} right_thumb;
	
	uint16_t vibration_left;
	uint16_t vibration_right;
} neko_Gamepad;

typedef struct _neko_GamepadInfo {
	const char *description;

} neko_GamepadInfo;

LIBNWIN_API uint32_t neko_GetConnectedControllerCount();
LIBNWIN_API const char* neko_GetControllerName(uint32_t _id);
LIBNWIN_API void neko_UpdateController(uint32_t _id, neko_Gamepad *_gamepad);


#endif