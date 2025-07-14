#pragma once
#include "BaseModule.h"
#include <vector>
#include <memory>
#include <algorithm>

class ModuleManager {
public:
    std::vector<std::shared_ptr<BaseModule>> modules;

    void registerModule(const std::shared_ptr<BaseModule>& module) {
        modules.push_back(module);
    }

    void tickModules() {
        for (auto& mod : modules) {
            if (mod->enabled) mod->whileEnabled();
        }
    }

    
    std::vector<std::shared_ptr<BaseModule>> getModulesByCategory(ModuleCategory category) {
        std::vector<std::shared_ptr<BaseModule>> result;
        for (auto& mod : modules) {
            if (mod->getCategory() == category) {
                result.push_back(mod);
            }
        }
        return result;
    }

    std::shared_ptr<BaseModule> getModuleByName(const std::string& name) {
        auto it = std::find_if(modules.begin(), modules.end(),
            [&name](const std::shared_ptr<BaseModule>& mod) {
                return mod->getName() == name;
            });
        return (it != modules.end()) ? *it : nullptr;
    }
    
    bool toggleModule(const std::string& name) {
        auto module = getModuleByName(name);
        if (module) {
            module->toggle();
            return true;
        }
        return false;
    }
};