#pragma once

#include <SFML/Network.hpp>

#include "../Shared/InterfaceEntity.hpp"

inline sf::Packet& operator << (sf::Packet &packet, const EntityState &state) {
    packet << state.id
           << state.position.x << state.position.y
           << state.velocity.x << state.velocity.y
           << state.hp
           << state.destroyed;
    return packet;
}

inline sf::Packet& operator >> (sf::Packet &packet, EntityState &state) {
    packet >> state.id
           >> state.position.x >> state.position.y
           >> state.velocity.x >> state.velocity.y
           >> state.hp
           >> state.destroyed;
    return packet;
}