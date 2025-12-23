#include "Client/States/ClientStateManager.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "Client/Network/NetworkClient.hpp"

#include "Client/States/Login/LoginState.hpp"
#include "Client/States/InGame/InGameState.hpp"

void ClientStateManager::changeState(ClientStateType nextState) {
    switch (nextState) {
        case ClientStateType::Login:
            currentState = std::make_unique<LoginState>(networkClient);
            break;
            
        case ClientStateType::InGame:
            currentState = std::make_unique<InGameState>(window, networkClient);
            break;

        default:
            break;
    }
}

ClientStateManager::ClientStateManager(sf::RenderWindow &_window, NetworkClient &_networkClient) 
: window(_window), networkClient(_networkClient) {
    currentState = std::make_unique<LoginState>(networkClient);
}

void ClientStateManager::handleEvent(const sf::Event &event) {
    currentState->handleEvent(event);
}

void ClientStateManager::update(float dt) {
    currentState->update(dt);

    ClientStateType nextState = currentState->tryChangeState();
    if (nextState != ClientStateType::None) {
        changeState(nextState);
    }
}

void ClientStateManager::render() {
    currentState->render(window);
}
