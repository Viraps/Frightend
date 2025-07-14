#pragma once
#include <string>

#include "ModuleCategory.h"

class BaseModule {
private:
    std::string name;
    ModuleCategory category;

public:
    bool enabled = false;

    BaseModule(std::string name, ModuleCategory category) 
        : name(std::move(name)), category(category) {}


    const std::string& getName() const { return name; }
    ModuleCategory getCategory() const { return category; }

    void setName(const std::string& newName) { name = newName; }
    void setCategory(ModuleCategory newCategory) { category = newCategory; }

    virtual void onEnable() {}
    virtual void onDisable() {}
    virtual void whileEnabled() {}

    void toggle() {
        enabled = !enabled;
        if (enabled) onEnable();
        else onDisable();
    }
};