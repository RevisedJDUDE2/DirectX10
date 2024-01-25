#pragma comment(lib, "d3d10.lib")

#include <windows.h>
#include <d3dx10.h>
#include <stdio.h>

LRESULT CALLBACK lpfnWndProcExWin(HWND, UINT, WPARAM, LPARAM);

//				GLOBALS				//
float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;

int colormodr = 1;
int colormodg = 1;
int colormodb = 1;

int WINAPI WinMain(HINSTANCE hThis, HINSTANCE hPrev, LPSTR Args, int NCMD) {
	const int window_height = 800;
	const int window_width = 600;
	HWND handle;
	WNDCLASSEXW windowclassexw = { sizeof(WNDCLASSEXW) };
	windowclassexw.lpszClassName = L"Dx10";
	windowclassexw.lpfnWndProc = lpfnWndProcExWin;
	windowclassexw.hInstance = hThis;
	windowclassexw.style = CS_VREDRAW | CS_HREDRAW;

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

	DXGI_SWAP_CHAIN_DESC scd;
	scd.BufferDesc.Height = window_height;
	scd.BufferDesc.Width = window_width;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = handle;
	scd.Windowed = false;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = 0;

	D3D10CreateDeviceAndSwapChain(0,
		D3D10_DRIVER_TYPE_HARDWARE, 0, 0, D3D10_SDK_VERSION, &scd,
		&SwapChain, &Device);

	ID3D10Texture2D* BackBuffer;
	SwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D),
		reinterpret_cast<void**>(&BackBuffer));

	Device->CreateRenderTargetView(BackBuffer, 0, &Renderer);

	BackBuffer->Release();

	Device->OMSetRenderTargets(1, &Renderer, NULL);

	D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Height = window_height;
	vp.Width = window_width;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	Device->RSSetViewports(1, &vp);

	D3DXCOLOR bgColor(0.0f, 0.0f, 0.0f, 1.0f);

	Device->ClearRenderTargetView(Renderer, bgColor);

	SwapChain->Present(0, 0);

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
			red += colormodr * 0.00005f;
			green += colormodg * 0.00002f;
			blue += colormodb * 0.00001f;

			if (red >= 1.0f || red <= 0.0f)
				colormodr *= -1;
			if (green >= 1.0f || green <= 0.0f)
				colormodg *= -1;
			if (blue >= 1.0f || blue <= 0.0f)
				colormodb *= -1;

			D3DXCOLOR bgColor(red, green, blue, 1.0f);
			Device->ClearRenderTargetView(Renderer, bgColor);
			SwapChain->Present(0, 0);
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
