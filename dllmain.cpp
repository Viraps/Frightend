#include <Windows.h>
#include <thread>
#include <cstdio>
#include "event/EventBus.h"
#include "module/BaseModule.h"
#include "module/ModuleManager.h"
#include "module/impl/DoubleClickModule.h"
#include "hooks/InputHook.h"
#include "gui/GUI.h"

EventBus g_EventBus;
ModuleManager g_ModuleManager;

bool g_RightShiftPressed = false;

void MainThread(HMODULE module) {
    AllocConsole();
    static FILE* p_file = nullptr;
    freopen_s(&p_file, "CONOUT$", "w", stdout);
    
    if (!InputHook::Init()) {
        printf("Failed to initialize InputHook\n");
        MessageBoxA(0, "ERROR", "Failed to initialize InputHook", MB_ICONERROR);
        FreeLibrary(module);
        return;
    }
    
    if (!GUI::Initialize()) {
        printf("Failed to initialize GUI\n");
        MessageBoxA(0, "ERROR", "Failed to initialize GUI", MB_ICONERROR);
        InputHook::Shutdown();
        FreeLibrary(module);
        return;
    }
    
    
    g_ModuleManager.registerModule(std::make_shared<DoubleClickModule>(&g_EventBus));

    
    GUI::SetModules(&g_ModuleManager.modules);
    
    for (auto& mod : g_ModuleManager.modules) {
        for (auto& mod : g_ModuleManager.modules) {
            std::cout << "[MainThread] Registered: " << mod->getName() << std::endl;
        }

    }

    printf("Main loop started. Press END to exit, Right Shift to toggle GUI.\n");
    
    while (!GetAsyncKeyState(VK_END)) {
        InputHook::Poll(g_EventBus);
        
        bool rightShiftCurrent = GetAsyncKeyState(VK_RSHIFT) & 0x8000;
        if (rightShiftCurrent && !g_RightShiftPressed) {
            GUI::Toggle();
        }
        g_RightShiftPressed = rightShiftCurrent;

        GUI::ProcessMessages();
        GUI::Render();

        g_ModuleManager.tickModules();

        Sleep(1);
    }

    printf("Shutting down...\n");
    
    InputHook::Shutdown();
    GUI::Shutdown();
    
    if (p_file) {
        fclose(p_file);
    }
    FreeConsole();
    
    FreeLibraryAndExitThread(module, 0);
}

bool __stdcall DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(instance);
        std::thread([=] { MainThread(instance); }).detach();
    }

    return true;
}