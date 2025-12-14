#include "NetworkServer.hpp"
#include "InputSystem.hpp"
#include "CombatSystem.hpp"
#include "InterestSystem.hpp"
#include "GameWorld.hpp"
#include "Projectile.hpp"
#include "SwordSlash.hpp"

constexpr float SEND_HZ      = 30.0f;
constexpr float SEND_INTEVAL = 1.0f / SEND_HZ;
constexpr float SERVER_HZ    = 60.0f;
constexpr float SERVER_TICK  = 1.0f / SERVER_HZ;

#include <iostream>

int main() {
    NetworkServer  networkServer;
    if (!networkServer.start(55001, 55002)) {
        return -1;
    }
   
    GameWorld      gameWorld;
    
    InputSystem    inputSystem;
    CombatSystem   combatSystem;
    InterestSystem interestSytem;
    
    float accumulator = 0.0f;
    int nextProjectileId = 0;
   
    sf::Clock clock, sendClock;
    while (true) {
        float dt = clock.restart().asSeconds();
        accumulator += dt;

        networkServer.poll();

        std::vector<NewClientEvent> & newClientEvents = networkServer.fetchNewClients();
        if (newClientEvents.size()) {
            for (NewClientEvent &event : newClientEvents) {
                gameWorld.addPlayer(event.clientId);
                inputSystem.clearQueue(event.clientId);

                Inventory &inventory = gameWorld.getPlayer(event.clientId)->getInventory();
                sf::Packet inventoryPacket;
                inventoryPacket << "Inventory" << (int)inventory.getSlots().size();
                for (const ItemSlot &slot : inventory.getSlots()) {
                    inventoryPacket << slot.id;
                }
                networkServer.sendToClientTcp(event.clientId, inventoryPacket);
            }
            newClientEvents.clear();
        }

        std::vector<NewInputEvent> & newInputEvents = networkServer.fetchInputs();
        if (newInputEvents.size()) {
            for (NewInputEvent &event : newInputEvents) {
                inputSystem.pushInput(event.clientId, event.input);
            }
            newInputEvents.clear();
        }

        std::vector<DeleteClientEvent> & deleteClientEvents = networkServer.fetchDeleteClients();
        if (deleteClientEvents.size()) {
            for (DeleteClientEvent &event : deleteClientEvents) {
                gameWorld.removePlayer(event.clientId);
            }
            deleteClientEvents.clear();
        }

        std::vector<MoveItemEvent> & moveItemEvents = networkServer.fetchMoveItems();
        if (moveItemEvents.size()) {
            for (MoveItemEvent &event : moveItemEvents) {
                if (gameWorld.moveItem(event.clientId, event.from, event.to)) {
                    Inventory &inventory = gameWorld.getPlayer(event.clientId)->getInventory();
                    sf::Packet inventoryPacket;
                    inventoryPacket << "Inventory" << (int)inventory.getSlots().size();
                    for (const ItemSlot &slot : inventory.getSlots()) {
                        inventoryPacket << slot.id;
                    }
                    networkServer.sendToClientTcp(event.clientId, inventoryPacket);
                }
            }
            moveItemEvents.clear();
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

            combatSystem.handleCollision(gameWorld.getPlayers(), gameWorld.getDamageEntities());

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

                networkServer.sendToClientUdp(client, worldStatePacket);
            }

            sendClock.restart();
        }

        sf::sleep(sf::milliseconds(1));
    }

    return 0;
}