#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "Client/Network/NetworkClient.hpp"

#include "Client/States/ClientStateManager.hpp"

#include "Client/Utils/Constants.hpp"
#include "Client/Utils/Font.hpp"

sf::Font Font::font;

int main() {
    Font::loadFromFile("Assets/Roboto_Mono.ttf");

    bool isFullscreen = false;
    sf::RenderWindow window({ WINDOW_WIDTH, WINDOW_HEIGHT }, "[Client]", sf::Style::Close);
    window.setVerticalSyncEnabled(true);

    NetworkClient networkClient("127.0.0.1", 55001, 55002);
    
    ClientStateManager stateManager(window, networkClient);

    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                networkClient.close();
                window.close();
            } 
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    networkClient.close();
                    window.close();
                }
                else if (event.key.code == sf::Keyboard::F11) {
                    isFullscreen = !isFullscreen;

                    if (isFullscreen) {
                        window.create(
                            sf::VideoMode::getDesktopMode(),
                            "[Client] - ID: " + std::to_string(networkClient.getClientId()),
                            sf::Style::None
                        );
                        window.setVerticalSyncEnabled(true);
                    }
                    else {
                        window.create(
                            sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
                            "[Client] - ID: " + std::to_string(networkClient.getClientId()),
                            sf::Style::Close
                        );
                        window.setVerticalSyncEnabled(true);
                    }
                }
            }   

            stateManager.handleEvent(event);
        }

        stateManager.update(dt);
        stateManager.render();
    }

    return 0;
}