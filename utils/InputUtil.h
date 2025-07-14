#pragma once
#include <Windows.h>
#include <vector>
#include <string>

enum class MouseButton {
    LEFT,
    RIGHT,
    MIDDLE,
    X1,
    X2
};

enum class KeyState {
    PRESSED,
    RELEASED
};

class InputUtil {
public:
    // Keyboard Methods
    static void pressKey(int virtualKey);
    static void releaseKey(int virtualKey);
    static void tapKey(int virtualKey, int holdTimeMs = 50);
    static void typeText(const std::string& text);
    static void sendKeyCombo(const std::vector<int>& keys);
    
    // Mouse Methods
    static void click(MouseButton button);
    static void doubleClick(MouseButton button);
    static void mouseDown(MouseButton button);
    static void mouseUp(MouseButton button);
    static void moveMouse(int x, int y, bool absolute = true);
    static void scroll(int delta);
    static void dragMouse(int startX, int startY, int endX, int endY, int durationMs = 100);
    static bool isHeldDown(int virtualKey);
    static bool isHeldDown(MouseButton button);
    
    // State Query Methods
    static bool isKeyPressed(int virtualKey);
    static bool isMouseButtonPressed(MouseButton button);
    static POINT getMousePosition();
    static POINT getScreenResolution();
    
    // Utility Methods
    static void sleep(int milliseconds);
    static void blockInput(bool block);
    static void setMouseSensitivity(int sensitivity);
    
    // Advanced Methods
    static void sendRawInput(RAWINPUT* rawInput);
    static void simulateKeySequence(const std::vector<int>& keys, int delayMs = 50);
    static void holdKeyFor(int virtualKey, int durationMs);
    
private:
    static DWORD getMouseButtonFlags(MouseButton button, bool down);
    static int getMouseButtonVK(MouseButton button);
    static void sendKeyInput(int virtualKey, KeyState state);
    static void sendMouseInput(DWORD flags, int x = 0, int y = 0, DWORD mouseData = 0);
};