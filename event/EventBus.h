#pragma once
#include "Event.h"
#include <functional>
#include <vector>
#include <typeindex>
#include <unordered_map>

class EventBus {
public:
    template<typename T>
    using Listener = std::function<void(T&)>;

    template<typename T>
    void subscribe(Listener<T> listener) {
        auto& vec = listeners[typeid(T)];
        vec.push_back([listener](Event& e) {
            listener(static_cast<T&>(e));
        });
    }

    void post(Event& event) {
        auto it = listeners.find(typeid(event));
        if (it != listeners.end()) {
            for (auto& listener : it->second) {
                listener(event);
            }
        }
    }

private:
    std::unordered_map<std::type_index, std::vector<std::function<void(Event&)>>> listeners;
};
