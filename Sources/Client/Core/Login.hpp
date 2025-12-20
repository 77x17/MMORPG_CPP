#pragma once

namespace sf {class RenderWindow; };
class NetworkClient;

class Login {
private:
    int clientId = -1;

public:
    void connecting(sf::RenderWindow &window, NetworkClient &networkClient);

    int loginScreen(sf::RenderWindow &window);
    
    int connectingScreen(sf::RenderWindow &window, NetworkClient &networkClient);
    
    void accountAlreadyLoggedIn(sf::RenderWindow &window);
};