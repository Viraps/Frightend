#include "GUI.h"
#include <d3d11.h>
#include <dxgi.h>
#include "../module/BaseModule.h"
#include "../Imgui/imgui.h"
#include "../Imgui/imgui_impl_win32.h"
#include "../Imgui/imgui_impl_dx11.h"
#include <map>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")


static bool g_ShowGui = false;
static std::vector<std::shared_ptr<BaseModule>>* g_Modules = nullptr;

static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

static HWND g_hWnd = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool GUI::CreateDeviceD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void GUI::CleanupDeviceD3D() {
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void GUI::CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void GUI::CleanupRenderTarget() {
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

LRESULT WINAPI GUI::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED) {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) 
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

bool GUI::Initialize() {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassEx(&wc);
    
    g_hWnd = ::CreateWindow(wc.lpszClassName, L"Module Manager", WS_OVERLAPPEDWINDOW, 100, 100, 450, 400, nullptr, nullptr, wc.hInstance, nullptr);

    
    if (!CreateDeviceD3D(g_hWnd)) {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return false;
    }

    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    return true;
}

void GUI::Shutdown() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(g_hWnd);
    ::UnregisterClass(L"ImGui Example", GetModuleHandle(nullptr));
}

void GUI::Render() {
    if (!g_ShowGui) return;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Create the main window
    ImGui::Begin("Module Manager", &g_ShowGui, ImGuiWindowFlags_AlwaysAutoResize);
    
    ImGui::Text("Module Management System");
    ImGui::Text("Press Right Shift to toggle this window.");
    ImGui::Separator();
   
    if (g_Modules && !g_Modules->empty()) {
        std::map<ModuleCategory, std::vector<std::shared_ptr<BaseModule>>> categorizedModules;
        for (const auto& module : *g_Modules) {
            categorizedModules[module->getCategory()].push_back(module);
        }

        for (const auto& categoryPair : categorizedModules) {
            ModuleCategory category = categoryPair.first;
            const auto& modules = categoryPair.second;
            
            std::string categoryName = categoryToString(category);
            
            int enabledCount = 0;
            for (const auto& module : modules) {
                if (module->enabled) enabledCount++;
            }
            
          
            std::string headerText = categoryName + " (" + std::to_string(enabledCount) + "/" + std::to_string(modules.size()) + ")";
            
            if (ImGui::TreeNode(headerText.c_str())) {
                for (const auto& module : modules) {
                    ImGui::PushID(module.get()); 
                    
                    if (module->enabled) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green for enabled
                    } else {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f)); // Gray for disabled
                    }
                    
                    if (ImGui::Selectable(module->getName().c_str(), false, ImGuiSelectableFlags_None)) {
                        module->toggle();
                    }
                    
                    ImGui::PopStyleColor();
                    
                    if (ImGui::IsItemHovered()) {
                        ImGui::BeginTooltip();
                        ImGui::Text("Module: %s", module->getName().c_str());
                        ImGui::Text("Category: %s", categoryName.c_str());
                        ImGui::Text("Status: %s", module->enabled ? "Enabled" : "Disabled");
                        ImGui::Text("Click to toggle");
                        ImGui::EndTooltip();
                    }
                    
                    ImGui::PopID();
                }
                ImGui::TreePop();
            }
        }
    } else {
        ImGui::Text("No modules registered.");
    }
    
    ImGui::Separator();
    
    if (g_Modules && !g_Modules->empty()) {
        if (ImGui::Button("Enable All")) {
            for (auto& module : *g_Modules) {
                if (!module->enabled) {
                    module->toggle();
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Disable All")) {
            for (auto& module : *g_Modules) {
                if (module->enabled) {
                    module->toggle();
                }
            }
        }
    }
    
    if (ImGui::Button("Close")) {
        g_ShowGui = false;
        Hide();
    }
    
    ImGui::End();
    
    ImGui::Render();
    const float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    g_pSwapChain->Present(1, 0);
}

void GUI::Show() {
    g_ShowGui = true;
    ::ShowWindow(g_hWnd, SW_SHOWDEFAULT);
    ::UpdateWindow(g_hWnd);
    ::SetForegroundWindow(g_hWnd);
}

void GUI::Hide() {
    g_ShowGui = false;
    ::ShowWindow(g_hWnd, SW_HIDE);
}

void GUI::Toggle() {
    if (g_ShowGui) {
        Hide();
    } else {
        Show();
    }
}

bool GUI::IsVisible() {
    return g_ShowGui;
}

void GUI::SetModules(std::vector<std::shared_ptr<BaseModule>>* modules) {
    g_Modules = modules;
}

void GUI::ProcessMessages() {
    if (!g_ShowGui) return;

    MSG msg;
    while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
        if (msg.message == WM_QUIT) {
            Hide();
        }
    }
}
