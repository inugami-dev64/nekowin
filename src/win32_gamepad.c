/// nekowin: OpenGL and Vulkan compatible library for context / surface window generation 
/// licence: Apache, see LICENCE.md
/// file: win32_gamepad.c - win32 gamepad input management source file
/// author: Karl-Mihkel Ott

#define WIN32_GAMEPAD_C
#include "gamepad.h"


uint32_t neko_GetConnectedControllerCount() {
	uint32_t count = 0;
	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++) {
		XINPUT_STATE state = { 0 };
		DWORD result = XInputGetState(i, &state);

		if (result == ERROR_SUCCESS) {
			count++;
		}
	}

	return count;
}


const char* neko_GetControllerName(uint32_t _id) {
	static JOYCAPS joy = { 0 };
	joyGetDevCapsA(_id, &joy, sizeof(JOYCAPS));

	return joy.szPname;
}


void neko_UpdateController(uint32_t _id, neko_Gamepad *_gamepad) {
	memset(_gamepad, 0, offsetof(neko_Gamepad, vibration_left));
	XINPUT_STATE state = { 0 };
	const DWORD result = XInputGetState((DWORD) _id, &state);

	// set controller vibration
	XINPUT_VIBRATION vib = {
		_gamepad->vibration_left,
		_gamepad->vibration_right
	};
	XInputSetState((DWORD)_id, &vib);

	// dpad states
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) == XINPUT_GAMEPAD_DPAD_UP)
		_gamepad->buttons |= NEKO_GAMEPAD_BTN_DPAD_UP;
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) == XINPUT_GAMEPAD_DPAD_DOWN)
		_gamepad->buttons |= NEKO_GAMEPAD_BTN_DPAD_DOWN;
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) == XINPUT_GAMEPAD_DPAD_LEFT)
		_gamepad->buttons |= NEKO_GAMEPAD_BTN_DPAD_LEFT;
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) == XINPUT_GAMEPAD_DPAD_RIGHT)
		_gamepad->buttons |= NEKO_GAMEPAD_BTN_DPAD_RIGHT;
	
	// A, B, X, Y states
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) == XINPUT_GAMEPAD_A)
		_gamepad->buttons |= NEKO_GAMEPAD_BTN_A;
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) == XINPUT_GAMEPAD_B)
		_gamepad->buttons |= NEKO_GAMEPAD_BTN_B;
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_X) == XINPUT_GAMEPAD_X)
		_gamepad->buttons |= NEKO_GAMEPAD_BTN_X;
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) == XINPUT_GAMEPAD_Y)
		_gamepad->buttons |= NEKO_GAMEPAD_BTN_Y;

	// start and back buttons
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) == XINPUT_GAMEPAD_START)
		_gamepad->buttons |= NEKO_GAMEPAD_BTN_START;
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) == XINPUT_GAMEPAD_BACK)
		_gamepad->buttons |= NEKO_GAMEPAD_BTN_BACK;

	// Bumper and stick buttons
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) == XINPUT_GAMEPAD_LEFT_SHOULDER)
		_gamepad->buttons |= NEKO_GAMEPAD_BTN_LEFT_BUMPER;
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) == XINPUT_GAMEPAD_RIGHT_SHOULDER)
		_gamepad->buttons |= NEKO_GAMEPAD_BTN_RIGHT_BUMPER;
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) == XINPUT_GAMEPAD_LEFT_THUMB)
		_gamepad->buttons |= NEKO_GAMEPAD_BTN_LEFT_STICK;
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) == XINPUT_GAMEPAD_RIGHT_THUMB)
		_gamepad->buttons |= NEKO_GAMEPAD_BTN_RIGHT_STICK;

	// trigger and thumb analog values
	_gamepad->left_trigger = (uint8_t) state.Gamepad.bLeftTrigger;
	_gamepad->right_trigger = (uint8_t) state.Gamepad.bRightTrigger;
	_gamepad->left_thumb.x = (int16_t) state.Gamepad.sThumbLX;
	_gamepad->left_thumb.y = (int16_t) state.Gamepad.sThumbLY;
	_gamepad->right_thumb.x = (int16_t) state.Gamepad.sThumbRX;
	_gamepad->right_thumb.y = (int16_t) state.Gamepad.sThumbRY;
}