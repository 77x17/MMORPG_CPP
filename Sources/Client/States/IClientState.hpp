#pragma once

// Client.cpp
//  └── while (running)
//        ├── input (handleEvent)
//        ├── update
//        ├── render

namespace sf { class Event; }
namespace sf { class RenderWindow; }

#include "Client/States/ClientStateType.hpp"

class IClientState {
public:
    virtual ~IClientState() = default;

    virtual void handleEvent(const sf::Event &event) = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow &window) = 0;

    virtual ClientStateType tryChangeState() const { 
        return ClientStateType::None;
    }
};