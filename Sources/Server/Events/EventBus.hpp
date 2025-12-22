#pragma once

#include <unordered_map>
#include <vector>
#include <functional>
#include <typeindex>

#include "Server/Events/Event.hpp"

class EventBus {
private:
    std::unordered_map<
        std::type_index, 
        std::vector<std::function<void(const Event &)>>
    > subscribers;

public:
    template<typename EventT>
    using Handle = std::function<void(const EventT &)>;

    template<typename EventT>
    void subscribe(Handle<EventT> handler) {
        auto &handlers = subscribers[typeid(EventT)];
        handlers.push_back(
            [h = std::move(handler)](const Event &event) {
                h(static_cast<const EventT &>(event));
            }
        );
    }

    template<typename EventT>
    void publish(const EventT &event) const {
        auto it = subscribers.find(typeid(EventT));
        if (it == subscribers.end()) return;

        for (const auto& function : it->second) {
            function(event);
        }
    }
};