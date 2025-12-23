#include "Client/States/Login/LoginState.hpp"

#include "Client/Network/NetworkClient.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>

#include "Client/Utils/Font.hpp"

#include "Client/States/InGame/InGameState.hpp"

#include <iostream>

LoginState::LoginState(NetworkClient &network) 
: networkClient(network) {

}

void LoginState::handleEvent(const sf::Event &event) {
    if (phase == Phase::EnterId && event.type == sf::Event::TextEntered) {
        if (event.text.unicode == 8 && !inputId.empty()) {
            inputId.pop_back();
        }
        else if (event.text.unicode == 13 && !inputId.empty()) {
            clientId = std::stoi(inputId);
            inputId.clear();

            phase = Phase::Connecting;
        }
        else if (isdigit(event.text.unicode) && static_cast<int>(inputId.size()) < 8) {
            inputId += static_cast<char>(event.text.unicode);
        }
    }

    if (phase == Phase::IdTaken && event.type == sf::Event::KeyPressed) {
        inputId.clear();

        phase = Phase::EnterId;
    }
}

void LoginState::update(float dt) {
    if (phase == Phase::EnterId) {
        if (caretClock.getElapsedTime().asSeconds() > 0.3f) {
            caretVisible = !caretVisible;
            caretClock.restart();
        }
    }
    else if (phase == Phase::Connecting) {
        if (networkClient.connectAll()) {
            if (!loginSent) {
                loginSent = true;
                networkClient.resetLoginStatus();
                networkClient.sendLogin(clientId);
            }

            networkClient.pollTCP();
            
            if (networkClient.getLoginStatus() == LoginStatus::Success) {
                phase = Phase::Success;
            }
            else if (networkClient.getLoginStatus() == LoginStatus::Fail) {
                phase = Phase::IdTaken;

                loginSent = false;
            }
        }
    }
    else if (phase == Phase::IdTaken) {

    }
}

void LoginState::render(sf::RenderWindow &window) {
    window.clear(sf::Color(30, 30, 30));

    sf::Text text;
    text.setFont(Font::getFont());
    text.setCharacterSize(24);
    text.setPosition(100, 200);

    switch (phase) {
        case Phase::EnterId:
            text.setString("Client ID: " + inputId + (caretVisible ? "|" : ""));
            break;
        case Phase::Connecting:
            text.setString("Connecting...");
            break;
        case Phase::IdTaken:
            text.setString("ID already logged in!\nPress any key to continue...");
            break;
        default:
            break;
    }

    window.draw(text);
    window.display();
}

ClientStateType LoginState::tryChangeState() const {
    if (phase == Phase::Success) {
        return ClientStateType::InGame;
    }
    return ClientStateType::None;
}