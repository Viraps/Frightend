#include "MouseEvent.h"

MouseEvent::MouseEvent(int i, bool is_mouse_down) 
    : button(i), isDown(is_mouse_down) {
}