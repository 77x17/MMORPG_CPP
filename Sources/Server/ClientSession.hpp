#pragma once

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>

#include "../Shared/Player.hpp"

struct ClientSession {
    int            id;

    sf::TcpSocket *tcp;
    sf::IpAddress  udpId;
    unsigned short udpPort = 0;
};