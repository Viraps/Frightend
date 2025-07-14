#pragma once
#include "../event/Event.h"

struct KeyEvent : public Event {
    int keyCode;
    bool isDown;
};
