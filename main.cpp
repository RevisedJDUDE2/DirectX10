#include <windows.h>
#include <stdio.h>

/** @brief Hello Bitch
*
*/
LRESULT CALLBACK lpfnWndProcExWin(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hThis, HINSTANCE hPrev, LPSTR Args, int NCMD) {
	MessageBoxW(nullptr, L"Init Successfully", L"", MB_OK);
	HWND handle;
	WNDCLASSEXW windowclassexw = { sizeof(WNDCLASSEXW) };
	windowclassexw.lpszClassName = L"Dx10";
	windowclassexw.lpfnWndProc = lpfnWndProcExWin;
	windowclassexw.hInstance = hThis;
	windowclassexw.hbrBackground = CreateSolidBrush(RGB(255, 0, 0));

	if (!RegisterClassExW(&windowclassexw)) {
		MessageBoxW(nullptr, L"cannot register window class", L"", MB_OK);
	}

	handle = CreateWindowExW(0, windowclassexw.lpszClassName, L"Game",
		WS_VISIBLE | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		nullptr, nullptr, windowclassexw.hInstance, nullptr);

	if (handle == INVALID_HANDLE_VALUE) {
		MessageBoxW(nullptr, L"cannot create window!", L"", MB_OK);
	}

	MSG message = {};
	bool isOpen = true;
	while (isOpen) {
		if (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE) > 0) {
			TranslateMessage(&message);
			DispatchMessage(&message);
			if (message.message == WM_QUIT) {
				isOpen = false;
			}
		} else {
			//RENDER HERE
		}
	}
	UnregisterClassW(windowclassexw.lpszClassName, windowclassexw.hInstance);
	return 0;
}
LRESULT CALLBACK lpfnWndProcExWin(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	if (msg == WM_DESTROY) {
		PostQuitMessage(0);
	}
	return DefWindowProcW(hwnd, msg, wp, lp);
}
