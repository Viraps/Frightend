#include "InputUtil.h"
#include <thread>
#include <chrono>


void InputUtil::pressKey(int virtualKey) {
    sendKeyInput(virtualKey, KeyState::PRESSED);
}

void InputUtil::releaseKey(int virtualKey) {
    sendKeyInput(virtualKey, KeyState::RELEASED);
}

void InputUtil::tapKey(int virtualKey, int holdTimeMs) {
    pressKey(virtualKey);
    sleep(holdTimeMs);
    releaseKey(virtualKey);
}

void InputUtil::typeText(const std::string& text) {
    for (char c : text) {
        int vk = VkKeyScanA(c);
        if (vk != -1) {
            bool needShift = (vk & 0x0100) != 0;
            vk &= 0xFF;
            
            if (needShift) {
                pressKey(VK_SHIFT);
            }
            
            tapKey(vk, 50);
            
            if (needShift) {
                releaseKey(VK_SHIFT);
            }
            
            sleep(10); 
        }
    }
}

void InputUtil::sendKeyCombo(const std::vector<int>& keys) {
    for (int key : keys) {
        pressKey(key);
        sleep(10);
    }
    
    sleep(50); 
    
    for (auto it = keys.rbegin(); it != keys.rend(); ++it) {
        releaseKey(*it);
        sleep(10);
    }
}

// Mouse Methods
void InputUtil::click(MouseButton button) {
    mouseDown(button);
    sleep(50);
    mouseUp(button);
}

void InputUtil::doubleClick(MouseButton button) {
    click(button);
    sleep(50);
    click(button);
}

void InputUtil::mouseDown(MouseButton button) {
    DWORD flags = getMouseButtonFlags(button, true);
    sendMouseInput(flags);
}

void InputUtil::mouseUp(MouseButton button) {
    DWORD flags = getMouseButtonFlags(button, false);
    sendMouseInput(flags);
}

void InputUtil::moveMouse(int x, int y, bool absolute) {
    if (absolute) {
        POINT resolution = getScreenResolution();
        x = (x * 65535) / resolution.x;
        y = (y * 65535) / resolution.y;
        sendMouseInput(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, x, y);
    } else {
        sendMouseInput(MOUSEEVENTF_MOVE, x, y);
    }
}

bool InputUtil::isHeldDown(int virtualKey) {
    return isKeyPressed(virtualKey);
}

bool InputUtil::isHeldDown(MouseButton button) {
    return isMouseButtonPressed(button);
}



void InputUtil::scroll(int delta) {
    sendMouseInput(MOUSEEVENTF_WHEEL, 0, 0, delta);
}

void InputUtil::dragMouse(int startX, int startY, int endX, int endY, int durationMs) {
    moveMouse(startX, startY, true);
    sleep(50);
    mouseDown(MouseButton::LEFT);
    
    int steps = durationMs / 10; 
    if (steps < 1) steps = 1;
    
    for (int i = 0; i <= steps; i++) {
        float progress = (float)i / steps;
        int currentX = startX + (int)((endX - startX) * progress);
        int currentY = startY + (int)((endY - startY) * progress);
        moveMouse(currentX, currentY, true);
        sleep(10);
    }
    
    mouseUp(MouseButton::LEFT);
}

bool InputUtil::isKeyPressed(int virtualKey) {
    return (GetAsyncKeyState(virtualKey) & 0x8000) != 0;
}

bool InputUtil::isMouseButtonPressed(MouseButton button) {
    int vk = getMouseButtonVK(button);
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

POINT InputUtil::getMousePosition() {
    POINT pos;
    GetCursorPos(&pos);
    return pos;
}

POINT InputUtil::getScreenResolution() {
    POINT resolution;
    resolution.x = GetSystemMetrics(SM_CXSCREEN);
    resolution.y = GetSystemMetrics(SM_CYSCREEN);
    return resolution;
}


void InputUtil::sleep(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void InputUtil::blockInput(bool block) {
    BlockInput(block);
}

void InputUtil::setMouseSensitivity(int sensitivity) {
    SystemParametersInfo(SPI_SETMOUSESPEED, 0, (PVOID)sensitivity, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
}

void InputUtil::sendRawInput(RAWINPUT* rawInput) {

}

void InputUtil::simulateKeySequence(const std::vector<int>& keys, int delayMs) {
    for (int key : keys) {
        tapKey(key, 50);
        sleep(delayMs);
    }
}

void InputUtil::holdKeyFor(int virtualKey, int durationMs) {
    pressKey(virtualKey);
    sleep(durationMs);
    releaseKey(virtualKey);
}

DWORD InputUtil::getMouseButtonFlags(MouseButton button, bool down) {
    switch (button) {
        case MouseButton::LEFT:
            return down ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
        case MouseButton::RIGHT:
            return down ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
        case MouseButton::MIDDLE:
            return down ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
        case MouseButton::X1:
            return down ? MOUSEEVENTF_XDOWN : MOUSEEVENTF_XUP;
        case MouseButton::X2:
            return down ? MOUSEEVENTF_XDOWN : MOUSEEVENTF_XUP;
        default:
            return 0;
    }
}

int InputUtil::getMouseButtonVK(MouseButton button) {
    switch (button) {
        case MouseButton::LEFT: return VK_LBUTTON;
        case MouseButton::RIGHT: return VK_RBUTTON;
        case MouseButton::MIDDLE: return VK_MBUTTON;
        case MouseButton::X1: return VK_XBUTTON1;
        case MouseButton::X2: return VK_XBUTTON2;
        default: return 0;
    }
}

void InputUtil::sendKeyInput(int virtualKey, KeyState state) {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = virtualKey;
    input.ki.dwFlags = (state == KeyState::RELEASED) ? KEYEVENTF_KEYUP : 0;
    
    SendInput(1, &input, sizeof(INPUT));
}

void InputUtil::sendMouseInput(DWORD flags, int x, int y, DWORD mouseData) {
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = flags;
    input.mi.dx = x;
    input.mi.dy = y;
    input.mi.mouseData = mouseData;
    
    SendInput(1, &input, sizeof(INPUT));
}