#pragma once

#include <SFML/Network/Packet.hpp>

struct OutgoingPacket {
    int clientId;
    bool useUdp;
    sf::Packet packet;
};