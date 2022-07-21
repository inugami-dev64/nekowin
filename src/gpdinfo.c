#include <stdio.h>
#include <nwin.h>

#if defined(_WIN32)
	#include <windows.h>
#elif defined(__linux__)
	#include <unistd.h>
#endif


uint32_t PollGamepads() {
	printf("Waiting for some controllers to be connected...\n");
	uint32_t count;
	while(!(count = neko_GetConnectedControllerCount())) {
#if defined(_WIN32)
		Sleep(1000);
#elif defined(__linux__)
		sleep(1);
#endif
	}

	printf("Select one of the following gamepad devices:\n");
	for (uint32_t i = 0; i < neko_GetConnectedControllerCount(); i++) {
		printf("%u) %s\n", i + 1, neko_GetControllerName(i));
	}

	uint32_t id;
	scanf("%u", &id);
	id--;
	return id;
}


int main(void) {
	uint32_t id = PollGamepads();

	neko_Gamepad old = { 0 };
	neko_Gamepad new = { 0 };

	while (true) {
		if(neko_CheckGamepadDisconnect(id)) {
			printf("Gamepad %u disconnected, please use another connected device\n", id);
			id = PollGamepads();
		}
		neko_UpdateController(id, &new);

		// A, B, X, Y
		if ((new.buttons & NEKO_GAMEPAD_BTN_A) && !(old.buttons & NEKO_GAMEPAD_BTN_A))
			printf("A pressed\n");
		if ((new.buttons & NEKO_GAMEPAD_BTN_B) && !(old.buttons & NEKO_GAMEPAD_BTN_B))
			printf("B pressed\n");
		if ((new.buttons & NEKO_GAMEPAD_BTN_X) && !(old.buttons & NEKO_GAMEPAD_BTN_X))
			printf("X pressed\n");
		if ((new.buttons & NEKO_GAMEPAD_BTN_Y) && !(old.buttons & NEKO_GAMEPAD_BTN_Y))
			printf("Y pressed\n");

		// dpad buttons
		if ((new.buttons & NEKO_GAMEPAD_BTN_DPAD_UP) && !(old.buttons & NEKO_GAMEPAD_BTN_DPAD_UP))
			printf("DPAD up pressed\n");
		if ((new.buttons & NEKO_GAMEPAD_BTN_DPAD_DOWN) && !(old.buttons & NEKO_GAMEPAD_BTN_DPAD_DOWN))
			printf("DPAD down pressed\n");
		if ((new.buttons & NEKO_GAMEPAD_BTN_DPAD_LEFT) && !(old.buttons & NEKO_GAMEPAD_BTN_DPAD_LEFT))
			printf("DPAD left pressed\n");
		if ((new.buttons & NEKO_GAMEPAD_BTN_DPAD_RIGHT) && !(old.buttons & NEKO_GAMEPAD_BTN_DPAD_RIGHT))
			printf("DPAD right pressed\n");

		// back and start buttons
		if ((new.buttons & NEKO_GAMEPAD_BTN_START) && !(old.buttons & NEKO_GAMEPAD_BTN_START))
			printf("Start pressed\n");
		if ((new.buttons & NEKO_GAMEPAD_BTN_BACK) && !(old.buttons & NEKO_GAMEPAD_BTN_BACK))
			printf("Back pressed\n");

		// bumper and stick keys
		if ((new.buttons & NEKO_GAMEPAD_BTN_LEFT_BUMPER) && !(old.buttons & NEKO_GAMEPAD_BTN_LEFT_BUMPER))
			printf("Left bumper pressed\n");
		if ((new.buttons & NEKO_GAMEPAD_BTN_RIGHT_BUMPER) && !(old.buttons & NEKO_GAMEPAD_BTN_RIGHT_BUMPER))
			printf("Right bumper pressed\n");
		if ((new.buttons & NEKO_GAMEPAD_BTN_LEFT_STICK) && !(old.buttons & NEKO_GAMEPAD_BTN_LEFT_STICK))
			printf("Left joystick pressed\n");
		if ((new.buttons & NEKO_GAMEPAD_BTN_RIGHT_STICK) && !(old.buttons & NEKO_GAMEPAD_BTN_RIGHT_STICK))
			printf("Right joystick pressed\n");

		// analog values
		if (new.left_trigger != old.left_trigger)
			printf("Left trigger: %u\n", new.left_trigger);
		if (new.right_trigger != old.right_trigger)
			printf("Right trigger: %u\n", new.right_trigger);
		if (new.left_thumb.x != old.left_thumb.x || new.left_thumb.y != old.left_thumb.y)
			printf("Left joystick (x; y): (%d; %d)\n", new.left_thumb.x, new.left_thumb.y);
		if (new.right_thumb.x != old.right_thumb.x || new.right_thumb.y != old.right_thumb.y)
			printf("Right joystick (x; y): (%d; %d)\n", new.right_thumb.x, new.right_thumb.y);

		old = new;
	}
	return 0;
}