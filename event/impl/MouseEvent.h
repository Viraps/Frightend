#pragma once
#include "../Event.h"

struct MouseEvent : public Event {
    int button;
    bool isDown;
    MouseEvent() = default;
    MouseEvent(int i, bool is_mouse_down);
};

