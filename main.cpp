#include <Windows.h>
#include <XInput.h>
#include <iostream>

#pragma comment(lib, "Xinput.lib")
#pragma comment(lib, "Xinput9_1_0.lib")

#define DEADZONE(x) \
-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE <= x && x <= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE \


int main() {
	DWORD dwResult, controllerID, sensitivity = 5000;
	float LX, LY, magnitude, nmagnitude = 0, nLX, nLY, dx, dy;
	bool newPress = true;
	POINT cursorPos;
	XINPUT_STATE state;
	INPUT input;
	ZeroMemory(&input, sizeof(INPUT));
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	input.type = INPUT_MOUSE;

	for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) {
		dwResult = XInputGetState(i, &state);
		if (dwResult == ERROR_SUCCESS) { controllerID = i; std::cout << "Found controller number: " << i << ", using it." << std::endl; break; }
		else std::cout << "Controller number: " << i << ", not found, checking next.." << std::endl;
	}

	while (true) {
		// SimpLY get the state of the controller from XInput.
		dwResult = XInputGetState(controllerID, &state);
		if (GetAsyncKeyState(VK_OEM_PLUS) & 1) sensitivity -= 1000;
		if (GetAsyncKeyState(VK_OEM_MINUS) & 1) sensitivity += 1000;

		if (dwResult == ERROR_SUCCESS)
		{
			auto gamePad = state.Gamepad;
			LX = gamePad.sThumbLX;
			LY = gamePad.sThumbLY;

			magnitude = sqrt(LX * LX + LY * LY);
			//nLX = LX / magnitude;
			//nLY = LY / magnitude;

			if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
				//std::cout << "x: " << LX << "\nY: " << LY << std::endl;
				//if (magnitude > 32767) magnitude = 32767;
				//magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

				//nmagnitude = magnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

				GetCursorPos(&cursorPos);

				dx = cursorPos.x + (LX / sensitivity);
				dy = cursorPos.y - (LY / sensitivity);

				SetCursorPos(dx, dy);
			}
			/*else {
				magnitude = 0.0;
				nmagnitude = 0.0;
			}*/

			if ((gamePad.wButtons & 0x1000) && newPress) {
				newPress = false;
				input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
				SendInput(1, &input, sizeof(INPUT));
				input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
				SendInput(1, &input, sizeof(INPUT));
			}
			else if (!(gamePad.wButtons & 0x1000)) {
				newPress = true;
			}
		}
		Sleep(1);
	}
}