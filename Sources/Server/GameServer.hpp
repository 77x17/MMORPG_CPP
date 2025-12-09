#pragma once

#include "NetworkServer.hpp"
#include "InputSystem.hpp"
#include "CombatSystem.hpp"
#include "InterestSystem.hpp"
#include "GameWorld.hpp"


constexpr float SEND_HZ      = 30.0f;
constexpr float SEND_INTEVAL = 1.0f / SEND_HZ;
constexpr float SERVER_HZ    = 60.0f;
constexpr float SERVER_TICK  = 1.0f / SERVER_HZ;

class GameServer {    
private:
    NetworkServer  networkServer;
    GameWorld      gameWorld;
    
    InputSystem    inputSystem;
    CombatSystem   combatSystem;
    InterestSystem interestSytem;
    
    float accumulator = 0.0f;
    int nextProjectileId = 0;

public:
    bool start(unsigned short port);
    void run();
};