#pragma once

#include <memory>

#include "Client/States/IClientState.hpp"

namespace sf { class RenderWindow; }
namespace sf { class Event; }

class NetworkClient;

class ClientStateManager {
private:
    sf::RenderWindow &window;
    NetworkClient    &networkClient;

    std::unique_ptr<IClientState> currentState;

private:
    void changeState(ClientStateType nextState);

public:
    ClientStateManager(sf::RenderWindow &_window, NetworkClient &_networkClient);
    
    void handleEvent(const sf::Event &event);
    void update(float dt);
    void render();

};