#include "Server/Debug/DebugSystem.hpp"

#include <sstream>
#include <format>

#include "Server/Core/GameWorld.hpp"

#include "Server/Entities/DamageEntity.hpp"
#include "Server/Entities/Enemy.hpp"
#include "Server/Entities/Player.hpp"

#include "Server/Network/NetworkServer.hpp"

#include "Server/Systems/Log/LogSystem.hpp"

DebugSystem::DebugSystem() {}

void DebugSystem::rebuildPlayers(const GameWorld &gameWorld, const NetworkServer &networkServer) {
    std::ostringstream oss;
    for (const ClientSession & client : networkServer.getClients()) {
        oss << std::format(
            "ID: {:>2} | TCP: {}:{} | UDP: {}:{}\n",
            client.id,
            client.tcp ? client.tcp->getRemoteAddress().toString() : 0,
            client.tcp ? client.tcp->getRemotePort() : 0,
            client.udpId.toString(),
            client.udpPort
        );
            
        if (client.id != -1) {
            if (const Player *player = gameWorld.getPlayer(client.id)) {
                oss << std::format(
                    "       | HP: {:>3} | POS: ({:>7.2f}, {:>7.2f})\n",
                    player->getHealth(),
                    player->getPosition().x,
                    player->getPosition().y
                );
            }
            else {
                oss << "       | Not spawned\n";
            }
        }
        else {
            oss << "       | Not logged in\n";
        }
    }
    snapshot.players = oss.str();
}

void DebugSystem::rebuildEnemies(const GameWorld &gameWorld) {
    std::ostringstream oss;
    for (const Enemy *enemy : gameWorld.getEnemies()) {
        oss << std::format(
            "ID: {:>2} | HP: {:>3} | POS: ({:>7.2f}, {:>7.2f})\n",
            enemy->getEntityId(),
            enemy->getHealth(),
            enemy->getPosition().x,
            enemy->getPosition().y
        );
    }
    snapshot.enemies = oss.str();
}

void DebugSystem::rebuildEntities(const GameWorld &gameWorld) {
    std::ostringstream oss;
    for (const DamageEntity *entity : gameWorld.getDamageEntities()) {
        oss << std::format(
            "{} | ID: {:>2} | OId: {:>2} | POS: ({:>7.2f}, {:>7.2f})\n",
            entity->getName(),
            entity->getEntityId(),
            entity->getOwnerEntityId(),
            entity->getPosition().x,
            entity->getPosition().y
        );
    }
    snapshot.entities = oss.str();
}

void DebugSystem::rebuildLogs() {
    std::ostringstream oss;
    for (const std::string & message : LogSystem::getLogs()) {
        oss << message + "\n";
    }
    snapshot.logs = oss.str();
}

void DebugSystem::markPlayersDirty() {
    playersDirty = true;
}

void DebugSystem::markEnemiesDirty() {
    enemiesDirty = true;
}

void DebugSystem::markEntitiesDirty() {
    entitiesDirty = true;
}

void DebugSystem::markLogsDirty() {
    logsDirty = true;
}

const DebugSnapshot & DebugSystem::getSnapshot(const GameWorld &gameWorld, const NetworkServer &networkServer) {
    // if (playersDirty) {
    //     playersDirty = false;
    //     rebuildPlayers(gameWorld, networkServer);
    // }
    // if (enemiesDirty) {
    //     enemiesDirty = false;
    //     rebuildEnemies(gameWorld);
    // }
    // if (entitiesDirty) {
    //     entitiesDirty = false;
    //     rebuildEntities(gameWorld);
    // }
    // if (logsDirty) {
    //     logsDirty = false;
    //     rebuildLogs();
    // }
    rebuildPlayers(gameWorld, networkServer);
    rebuildEnemies(gameWorld);
    rebuildEntities(gameWorld);
    rebuildLogs();
    
    return snapshot;
}