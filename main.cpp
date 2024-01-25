#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <d3dx10.h>
#include <string>
#pragma comment(lib, "D3D10.lib")
#pragma comment(lib, "d3dx10d.lib")

LPCTSTR WndClassName = L"firstwindow";
HWND hwnd = NULL;

const int Width = 800;
const int Height = 600;

bool InitializeWindow(HINSTANCE hInstance,
  int ShowWnd,
  int width, int height,
  bool windowed);

ID3D10Device* d3dDevice;
IDXGISwapChain* SwapChain;
ID3D10RenderTargetView* RenderTargetView;

bool InitializeDirect3dApp(HINSTANCE hInstance);
//////////////////new////////////////////
bool InitScene();
ID3DX10Font* Font;
//////////////////new////////////////////

int messageloop();

LRESULT CALLBACK WndProc(HWND hWnd,
  UINT msg,
  WPARAM wParam,
  LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,    //Main windows function
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nShowCmd) {

  if (!InitializeWindow(hInstance, nShowCmd, Width, Height, true)) {
    MessageBox(0, L"Window Initialization - Failed",
      L"Error", MB_OK);
    return 0;
  }
  if (!InitializeDirect3dApp(hInstance)) {
    MessageBox(0, L"Direct3D Initialization - Failed",
      L"Error", MB_OK);
    return 0;
  }
  //////////////////new////////////////////
  if (!InitScene()) {
    MessageBox(0, L"Scene Initialization - Failed",
      L"Error", MB_OK);
    return 0;
  }
  //////////////////new////////////////////
  messageloop();

  return 0;
}

bool InitializeWindow(HINSTANCE hInstance,
  int ShowWnd,
  int width, int height,
  bool windowed) {
  typedef struct _WNDCLASS {
    UINT cbSize;
    UINT style;
    WNDPROC lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    HANDLE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    LPCTSTR lpszMenuName;
    LPCTSTR lpszClassName;
  } WNDCLASS;

  WNDCLASSEX wc;

  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = NULL;
  wc.cbWndExtra = NULL;
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = WndClassName;
  wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

  if (!RegisterClassEx(&wc)) {
    MessageBox(NULL, L"Error registering class",
      L"Error", MB_OK | MB_ICONERROR);
    return 1;
  }

  hwnd = CreateWindowEx(
    NULL,
    WndClassName,
    L"Window Title",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,
    width, height,
    NULL,
    NULL,
    hInstance,
    NULL
  );

  if (!hwnd) {
    MessageBox(NULL, L"Error creating window",
      L"Error", MB_OK | MB_ICONERROR);
    return 1;
  }

  ShowWindow(hwnd, ShowWnd);
  UpdateWindow(hwnd);

  return true;
}

bool InitializeDirect3dApp(HINSTANCE hInstance) {
  HRESULT hr;

  DXGI_SWAP_CHAIN_DESC scd;
  scd.BufferDesc.Width = Width;
  scd.BufferDesc.Height = Height;
  scd.BufferDesc.RefreshRate.Numerator = 60;
  scd.BufferDesc.RefreshRate.Denominator = 1;
  scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

  //no multisampling
  scd.SampleDesc.Count = 1;
  scd.SampleDesc.Quality = 0;

  scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  scd.BufferCount = 1;
  scd.OutputWindow = hwnd;
  scd.Windowed = true;
  scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  scd.Flags = 0;

  D3D10CreateDeviceAndSwapChain(0, D3D10_DRIVER_TYPE_HARDWARE, 0, 0, D3D10_SDK_VERSION, &scd, &SwapChain, &d3dDevice);


  ID3D10Texture2D* backBuffer;
  SwapChain->GetBuffer(0, _uuidof(ID3D10Texture2D), reinterpret_cast<void**>(&backBuffer));
  d3dDevice->CreateRenderTargetView(backBuffer, 0, &RenderTargetView);
  backBuffer->Release();

  D3D10_TEXTURE2D_DESC depthStencilDesc;
  depthStencilDesc.Width = Width;
  depthStencilDesc.Height = Height;
  depthStencilDesc.MipLevels = 1;
  depthStencilDesc.ArraySize = 1;
  depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthStencilDesc.SampleDesc.Count = 1;
  depthStencilDesc.SampleDesc.Quality = 0;
  depthStencilDesc.Usage = D3D10_USAGE_DEFAULT;
  depthStencilDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
  depthStencilDesc.CPUAccessFlags = 0;
  depthStencilDesc.MiscFlags = 0;

  ID3D10Texture2D* DepthStencilBuffer;
  ID3D10DepthStencilView* DepthStencilView;

  hr = (d3dDevice->CreateTexture2D(&depthStencilDesc, 0, &DepthStencilBuffer));
  if (FAILED(hr)) {
    MessageBox(NULL, L"Error creating Texture2D",
      L"Error", MB_OK | MB_ICONERROR);
    return 1;
  }
  hr = (d3dDevice->CreateDepthStencilView(DepthStencilBuffer, 0, &DepthStencilView));
  if (FAILED(hr)) {
    MessageBox(NULL, L"Error creating Depth Stencil Buffer",
      L"Error", MB_OK | MB_ICONERROR);
    return 1;
  }

  d3dDevice->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

  D3D10_VIEWPORT vp;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  vp.Width = Width;
  vp.Height = Height;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;

  d3dDevice->RSSetViewports(1, &vp);

  D3DXCOLOR bgColor(0.0f, 0.0f, 1.0f, 1.0f);
  d3dDevice->ClearRenderTargetView(RenderTargetView, bgColor);

  SwapChain->Present(0, 0);

  return true;
}
//////////////////new////////////////////
bool InitScene() {
  D3DX10_FONT_DESC fd;
  fd.Height = 64;
  fd.Width = 0;
  fd.Weight = 0;
  fd.MipLevels = 1;
  fd.Italic = false;
  fd.CharSet = OUT_DEFAULT_PRECIS;
  fd.Quality = DEFAULT_QUALITY;
  fd.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
  wcscpy(fd.FaceName, L"Impact");

  D3DX10CreateFontIndirect(d3dDevice, &fd, &Font);

  return true;
}
//////////////////new////////////////////
int messageloop() {
  MSG msg;
  ZeroMemory(&msg, sizeof(MSG));
  while (true) {
    BOOL PeekMessageL(
      LPMSG lpMsg,
      HWND hWnd,
      UINT wMsgFilterMin,
      UINT wMsgFilterMax,
      UINT wRemoveMsg
    );

    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT)
        break;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      // run game code
          //////////////////new////////////////////
      D3DXCOLOR fontColor(1.0f, 1.0f, 1.0f, 1.0f);
      RECT rectangle = { 0, 0, 0, 0 };
      Font->DrawText(0, L"Hello this is Jude", -1, &rectangle, DT_NOCLIP, fontColor);
      SwapChain->Present(0, 0);
      //////////////////new////////////////////
    }
  }
  return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd,
  UINT msg,
  WPARAM wParam,
  LPARAM lParam) {
  switch (msg) {
  case WM_KEYDOWN:
    if (wParam == VK_ESCAPE) {
      SwapChain->SetFullscreenState(false, NULL);
    }
    if (wParam == VK_F11) {
      SwapChain->SetFullscreenState(true, NULL);
    }
    return 0;

  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hwnd,
    msg,
    wParam,
    lParam);
}
