#pragma once

#include <cstdint>

enum class UdpPacketType : std::uint8_t {
    UdpPing,
    Assign_UDP,
    Input,
    WorldState,
    Projectile,
    SwordSlash
};