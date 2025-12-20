#include "Client/Core/Login.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Text.hpp>

#include "Client/Network/NetworkClient.hpp"
#include "Client/Utils/Font.hpp"

int Login::loginScreen(sf::RenderWindow &window) {
    bool loginDone = false;
    std::string inputClientId;

    sf::Clock caretClock;
    bool caretVisible = true;
    while (window.isOpen() && not loginDone) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                exit(0);
            }

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 8) { // Backspace
                    if (!inputClientId.empty()) {
                        inputClientId.pop_back();
                    }
                }
                else if (event.text.unicode == 13) { // Enter
                    if (!inputClientId.empty()) {
                        loginDone = true;
                    }
                }
                else if (isdigit(event.text.unicode) && inputClientId.size() < 16) {
                    inputClientId += static_cast<char>(event.text.unicode);
                }
            }
        }

        if (caretClock.getElapsedTime().asSeconds() > 0.3f) {
            caretVisible = !caretVisible;
            caretClock.restart();
        }

        window.clear(sf::Color(30, 30, 30));

        sf::Text inputText;
        inputText.setFont(Font::getFont());
        inputText.setCharacterSize(24);
        inputText.setPosition(100, 200);
        inputText.setString("Client ID: " + inputClientId + (caretVisible ? '|' : ' '));
        window.draw(inputText);

        window.display();
    }

    return std::stoi(inputClientId);
}

int Login::connectingScreen(sf::RenderWindow &window, NetworkClient &networkClient) {
    sf::Clock dotClock;
    int dotCount = 0;
    while (window.isOpen() && networkClient.connectAll() == false) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                exit(0);
            }
        }

        if (dotClock.getElapsedTime().asSeconds() > 1.0f) {
            dotClock.restart();
            
            dotCount = (dotCount + 1) % 4; // 0 -> 3
        }

        window.clear(sf::Color(30, 30, 30));

        sf::Text connectingText;
        connectingText.setFont(Font::getFont());
        connectingText.setCharacterSize(24);
        connectingText.setFillColor(sf::Color::White);
        connectingText.setPosition(100, 200);
        std::string dots(dotCount, '.');
        connectingText.setString("Connecting to server " + dots);
        window.draw(connectingText);

        window.display();
    }
    
    networkClient.sendLogin(clientId);

    int status = -1;
    while (status == -1) {
        status = networkClient.pollLogin();
    }
    return status;
}

void Login::accountAlreadyLoggedIn(sf::RenderWindow &window) {
    bool pressAnyKey = false;
    sf::Clock dotClock;
    int dotCount = 0;
    while (window.isOpen() && not pressAnyKey) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                exit(0);
            }

            if (event.type == sf::Event::KeyPressed) {
                pressAnyKey = true;
            }
        }

        if (dotClock.getElapsedTime().asSeconds() > 1.0f) {
            dotClock.restart();
            
            dotCount = (dotCount + 1) % 4; // 0 -> 3
        }

        window.clear(sf::Color(30, 30, 30));

        sf::Text idTakenText;
        idTakenText.setFont(Font::getFont());
        idTakenText.setCharacterSize(24);
        idTakenText.setFillColor(sf::Color::White);
        idTakenText.setPosition(100, 200);
        idTakenText.setString("Account ID: " + std::to_string(clientId) + " already logged in!\n");
        window.draw(idTakenText);

        sf::Text pressAnyKeyText;
        pressAnyKeyText.setFont(Font::getFont());
        pressAnyKeyText.setCharacterSize(24);
        pressAnyKeyText.setFillColor(sf::Color::White);
        pressAnyKeyText.setPosition(100, 230);
        std::string dots(dotCount, '.');
        pressAnyKeyText.setString("Press any key to login again " + dots);
        window.draw(pressAnyKeyText);

        window.display();
    }
}

void Login::connecting(sf::RenderWindow &window, NetworkClient &networkClient) {
    while (true) {
        clientId = loginScreen(window);

        int status = connectingScreen(window, networkClient);

        if (status >= 0) {
            break;
        }
        else {
            accountAlreadyLoggedIn(window);
        }
    }
}
