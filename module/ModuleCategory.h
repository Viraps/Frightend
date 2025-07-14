#pragma once
#include <string>

enum class ModuleCategory {
    COMBAT,
    MOVEMENT,
    RENDER,
    PLAYER,
    MISC
};


inline std::string categoryToString(ModuleCategory category) {
    switch (category) {
    case ModuleCategory::COMBAT: return "Combat";
    case ModuleCategory::MOVEMENT: return "Movement";
    case ModuleCategory::RENDER: return "Render";
    case ModuleCategory::PLAYER: return "Player";
    case ModuleCategory::MISC: return "Misc";
    default: return "Unknown";
    }
}
