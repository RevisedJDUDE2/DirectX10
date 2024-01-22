#pragma comment(lib, "d3d10.lib")

#include <windows.h>
#include <d3dx10.h>
#include <stdio.h>

LRESULT CALLBACK lpfnWndProcExWin(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hThis, HINSTANCE hPrev, LPSTR Args, int NCMD) {
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

	//Direct X Stuff

	ID3D10Device* Device;
	IDXGISwapChain* SwapChain;
	ID3D10RenderTargetView* Renderer;

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
