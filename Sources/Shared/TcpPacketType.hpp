#pragma once

#include <cstdint>

enum class TcpPacketType : std::uint8_t {
    Login,
    Login_Fail,
    Assign_ID,
    MoveItem,
    EquipItem,
    TcpPing,
    MouseSelect,
    Inventory,
    Equipment,
    WorldCollision,
    Chunk
};