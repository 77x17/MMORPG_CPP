#include "Server/Core/GameWorldSyncSystem.hpp"

#include "Server/Core/GameWorld.hpp"
#include "Server/Network/NetworkServer.hpp"
#include "Server/Systems/Interest/InterestSystem.hpp"

#include "Server/Entities/Enemy.hpp"
#include "Server/Entities/Player.hpp"
#include "Server/Entities/Projectile.hpp"
#include "Server/Entities/SwordSlash.hpp"

#include "Server/Systems/Log/LogSystem.hpp"

#include "Shared/UdpPacketType.hpp"

GameWorldSyncSystem::GameWorldSyncSystem(GameWorld &_gameWorld, NetworkServer &_networkServer, InterestSystem &_interestSystem)
: gameWorld(_gameWorld), networkServer(_networkServer), interestSystem(_interestSystem) {

}

void GameWorldSyncSystem::syncToClients() {
    for (ClientSession &client : networkServer.getClients()) {
        if (client.udpPort == 0) continue;

        Player *currentPlayer = gameWorld.getPlayer(client.id);
        if (currentPlayer == nullptr) continue;

        sf::Packet worldStatePacket; worldStatePacket << static_cast<uint8_t>(UdpPacketType::WorldState);
        
        std::vector<Player *> visiblePlayers = interestSystem.getVisiblePlayers(currentPlayer, gameWorld.getPlayersInChunk(currentPlayer->getPosition()));
        worldStatePacket << static_cast<int>(visiblePlayers.size());
        for (Player *player : visiblePlayers) {
            worldStatePacket << player->getId() 
                             << player->getPosition().x 
                             << player->getPosition().y 
                             << player->getHealth() 
                             << player->lastProcessedInput;
        }

        std::vector<DamageEntity *> visibleDamageEntities = interestSystem.getVisibleDamageEntities(currentPlayer, gameWorld.getDamageEntitiesInChunk(currentPlayer->getPosition()));
        worldStatePacket << static_cast<int>(visibleDamageEntities.size());
        for (DamageEntity *damageEntity : visibleDamageEntities) {
            Projectile *projectile = dynamic_cast<Projectile *>(damageEntity);
            if (projectile) {
                worldStatePacket << static_cast<uint8_t>(UdpPacketType::Projectile)
                                 << projectile->getId() 
                                 << projectile->getPosition().x 
                                 << projectile->getPosition().y 
                                 << projectile->getVelocity().x
                                 << projectile->getVelocity().y
                                 << projectile->getOwnerId();
            }

            SwordSlash *swordSlash = dynamic_cast<SwordSlash *>(damageEntity);
            if (swordSlash) {
                worldStatePacket << static_cast<uint8_t>(UdpPacketType::SwordSlash)
                                 << swordSlash->getId()
                                 << swordSlash->getBounds().left
                                 << swordSlash->getBounds().top
                                 << swordSlash->getBounds().width
                                 << swordSlash->getBounds().height
                                 << swordSlash->getOwnerId();
            }
        }
        
        std::vector<Enemy *> visibleEnemy = interestSystem.getVisibleEnemies(currentPlayer, gameWorld.getEnemiesInChunk(currentPlayer->getPosition()));
        worldStatePacket << static_cast<int>(visibleEnemy.size());
        for (Enemy *enemy : visibleEnemy) {
            worldStatePacket << enemy->getId()
                             << enemy->getPosition().x
                             << enemy->getPosition().y
                             << enemy->getHealth();
        }

        if (currentPlayer->getMouseSelected()) {
            worldStatePacket << true;
            bool find = false;
            for (const Enemy *enemy : visibleEnemy) {
                if (enemy->getBounds().contains(currentPlayer->getMousePosition())) {
                    worldStatePacket << enemy->getId()
                                     << enemy->getHealth()
                                     << enemy->getMaxHealth();

                    currentPlayer->setEntitySelectedId(enemy->getId());
                    
                    find = true;
                    break;
                }
            }
            if (!find) {
                currentPlayer->setEntitySelectedId(-1);
                worldStatePacket << -1 << -1 << -1;
            }
            currentPlayer->toggleMouseSelected();
        }
        else if (currentPlayer->getEntitySelectedId() != -1) {
            worldStatePacket << true;
            for (const Enemy *enemy : visibleEnemy) {
                if (enemy->getId() == currentPlayer->getEntitySelectedId()) {
                    worldStatePacket << enemy->getId()
                                     << enemy->getHealth()
                                     << enemy->getMaxHealth();
                    break;
                }
            }
        }
        else {
            worldStatePacket << false;
        }

        networkServer.sendAsync(client.id, worldStatePacket, true);
    }
}