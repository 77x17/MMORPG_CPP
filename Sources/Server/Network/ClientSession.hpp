#pragma once

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>

enum class ClientState {
    Connecting,
    Active,
    Disconnected
};

struct ClientSession {
    int            id;

    sf::TcpSocket *tcp = nullptr;
    sf::IpAddress  udpId;
    unsigned short udpPort = 0;

    ClientState state = ClientState::Connecting;

    bool operator == (const ClientSession &other) const {
        return id == other.id && udpPort == other.udpPort && udpId == other.udpId;
    }
};