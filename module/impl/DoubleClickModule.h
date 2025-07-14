#include "../BaseModule.h"
#include "../../event/impl/MouseEvent.h"
#include "../../event/EventBus.h"
#include <iostream>
#include "module/ModuleCategory.h"
#include "utils/InputUtil.h"

class DoubleClickModule : public BaseModule {
public:
    EventBus* bus;

    DoubleClickModule(EventBus* bus)
        : BaseModule("DoubleClick", ModuleCategory::MISC), bus(bus) {}

    void onEnable() override {
        std::cout << getName() << " (" << categoryToString(getCategory()) << ") enabled!\n";

        bus->subscribe<MouseEvent>([this](MouseEvent& e) {
            if (e.button == 1 && e.isDown) {
                if (this->enabled) {
                    for (int i = 0; i < 2; i++)
                    {
                        InputUtil::click(MouseButton::RIGHT);
                    }
                }
            }
        });
    }

    void whileEnabled() override {
        
    }
};