#pragma once

// EnterClientId
//     ↓ Enter
// Connecting
//     ↓ success → DONE
//     ↓ fail → IdTaken
// IdTaken
//     ↓ any key
// EnterClientId

#include "Client/States/IClientState.hpp"

#include <string>
#include <SFML/System/Clock.hpp>

class NetworkClient;

class LoginState : public IClientState {
private:
    enum class Phase {
        EnterId,
        Connecting,
        IdTaken,
        Success
    };

    Phase phase = Phase::EnterId;

    NetworkClient &networkClient;

    int clientId = -1;
    std::string inputId;

    sf::Clock caretClock;
    bool caretVisible = true;

    bool loginSent = false;

public:
    LoginState(NetworkClient &network);

    void handleEvent(const sf::Event &event) override;
    void update(float dt) override;
    void render(sf::RenderWindow &window) override;

    ClientStateType tryChangeState() const override;
};