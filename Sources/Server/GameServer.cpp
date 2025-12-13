#include "GameServer.hpp"

#include <SFML/System/Clock.hpp>

#include "Projectile.hpp"
#include "SwordSlash.hpp"

bool GameServer::start(unsigned short tcpPort, unsigned short udpPort) {
    if (!networkServer.start(tcpPort, udpPort)) {
        return false;
    }

    return true;
}

void GameServer::run() {
    sf::Clock clock, sendClock;
    while (true) {
        float dt = clock.restart().asSeconds();
        accumulator += dt;

        networkServer.poll();

        std::vector<NewClientEvent> newClientEvents = networkServer.fetchNewClients();
        for (NewClientEvent &event : newClientEvents) {
            gameWorld.addPlayer(event.clientId);
            inputSystem.clearQueue(event.clientId);
        }

        std::vector<NewInputEvent> newInputEvents = networkServer.fetchInputs();
        for (NewInputEvent &event : newInputEvents) {
            inputSystem.pushInput(event.clientId, event.input);
        }

        std::vector<DeleteClientEvent> deleteClientEvents = networkServer.fetchDeleteClients();
        for (DeleteClientEvent &event : deleteClientEvents) {
            gameWorld.removePlayer(event.clientId);
        }

        if (accumulator >= SERVER_TICK) {
            for (Player *player : gameWorld.getPlayers()) {
                std::vector<InputState> &inputQueue = inputSystem.getQueue(player->getId());
                if (!inputQueue.empty()) {
                    InputState latest = inputQueue.back();
                    inputSystem.clearQueue(player->getId());

                    DamageEntity *damageEntity = player->updatePlayer(SERVER_TICK, latest);
                    if (damageEntity) {
                        damageEntity->setId(++nextProjectileId);
                        gameWorld.addDamageEntity(damageEntity);
                    }

                    player->lastProcessedInput = latest.seq;
                }
                else {
                    InputState input;
                    player->updatePlayer(SERVER_TICK, input);
                }
            }

            combatSystem.handleCollision(gameWorld);

            gameWorld.update(SERVER_TICK);

            accumulator -= SERVER_TICK;
        }

        if (sendClock.getElapsedTime().asSeconds() >= SEND_INTEVAL) {
            for (ClientSession &client : networkServer.getClients()) {
                if (client.udpPort == 0) continue;

                Player *currentPlayer = nullptr;
                for (Player *player : gameWorld.getPlayers()) {
                    if (player->getId() == client.id) currentPlayer = player;
                }
                sf::Packet worldStatePacket;
                worldStatePacket << "WorldState";
                
                std::vector<Player *> visiblePlayers = interestSytem.getVisiblePlayers(currentPlayer, gameWorld.getPlayers());
                worldStatePacket << (int)visiblePlayers.size();
                for (Player *player : visiblePlayers) {
                    worldStatePacket << player->getId() 
                                     << player->getPosition().x 
                                     << player->getPosition().y 
                                     << player->getHealth() 
                                     << player->lastProcessedInput;
                }

                std::vector<DamageEntity *> visibleDamageEntities = interestSytem.getVisibleDamageEntities(currentPlayer, gameWorld.getDamageEntities());
                worldStatePacket << (int)visibleDamageEntities.size();
                for (DamageEntity *damageEntity : visibleDamageEntities) {
                    Projectile *projectile = dynamic_cast<Projectile *>(damageEntity);
                    if (projectile) {
                        worldStatePacket << "Projectile"
                                         << projectile->getId() 
                                         << projectile->getPosition().x 
                                         << projectile->getPosition().y 
                                         << projectile->getVelocity().x
                                         << projectile->getVelocity().y
                                         << projectile->getOwnerId();
                    }

                    SwordSlash *swordSlash = dynamic_cast<SwordSlash *>(damageEntity);
                    if (swordSlash) {
                        worldStatePacket << "SwordSlash"
                                         << swordSlash->getId()
                                         << swordSlash->getBounds().left
                                         << swordSlash->getBounds().top
                                         << swordSlash->getBounds().width
                                         << swordSlash->getBounds().height
                                         << swordSlash->getOwnerId();
                    }
                }

                networkServer.sendToClient(client, worldStatePacket);
            }

            sendClock.restart();
        }

        sf::sleep(sf::milliseconds(1));
    }
}