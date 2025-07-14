#pragma once
#include <Windows.h>
#include <memory>
#include <vector>

class BaseModule;
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

class GUI {
public:
    static bool Initialize();
    static void Shutdown();
    static void Render();
    static void Show();
    static void Hide();
    static void Toggle();
    static bool IsVisible();
    static void SetModules(std::vector<std::shared_ptr<BaseModule>>* modules);
    static void ProcessMessages();

private:
    static bool CreateDeviceD3D(HWND hWnd);
    static void CleanupDeviceD3D();
    static void CreateRenderTarget();
    static void CleanupRenderTarget();
    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};