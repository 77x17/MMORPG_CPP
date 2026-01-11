#pragma once

#include <cstdint>

enum class TcpPacketType : std::uint8_t {
    Login,
    Login_Fail,
    Assign_ClientID,
    Assign_EntityID,
    MoveItem,
    EquipItem,
    TcpPing,
    MouseSelect,
    Inventory,
    Equipment,
    WorldCollision,
    Chunk,
    QuestFullSync,
    QuestUpdate
};