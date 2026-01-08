#include "Server/Core/Chunk/ChunkSystem.hpp"
#include "Server/Core/Chunk/DebugChunkSyncSystem.hpp"
#include "Server/Core/GameWorld.hpp"
#include "Server/Core/GameWorldSyncSystem.hpp"

#include "Server/Debug/DebugSystem.hpp"

#include "Server/Entities/EnemiesManager.hpp"
#include "Server/Entities/Player.hpp"
#include "Server/Entities/Projectile.hpp"
#include "Server/Entities/SwordSlash.hpp"

#include "Server/Quests/QuestManager.hpp"
#include "Server/Quests/QuestSystem.hpp"

#include "Server/Network/NetworkServer.hpp"

#include "Server/Renderer/Renderer.hpp"

#include "Server/Systems/AI/EnemyAISystem.hpp"
#include "Server/Systems/Combat/CombatSystem.hpp"
#include "Server/Systems/Combat/WeaponSystem.hpp"
#include "Server/Systems/Input/InputManager.hpp"
#include "Server/Systems/Input/InputSystem.hpp"
#include "Server/Systems/Interest/InterestSystem.hpp"
#include "Server/Systems/Inventory/InventorySystem.hpp"
#include "Server/Systems/Inventory/InventorySyncSystem.hpp"
#include "Server/Systems/Log/LogSystem.hpp"
#include "Server/Systems/Physics/PhysicsSystem.hpp"
#include "Server/Systems/Physics/WorldCollision.hpp"
#include "Server/Systems/Physics/WorldCollisionSystem.hpp"
#include "Server/Systems/Physics/WorldCollisionSyncSystem.hpp"

#include "Server/Utils/Constants.hpp"
#include "Server/Utils/Random.hpp"
#include "Server/Utils/Font.hpp"
#include "Shared/TcpPacketType.hpp"

sf::Font Font::font;
std::deque<std::string> LogSystem::logs;

void syncGameWorldFromClients(GameWorld &gameWorld, InputManager &inputManager, InventorySystem &inventorySystem, InventorySyncSystem &inventorySyncSystem, NetworkServer &networkServer, WorldCollisionSyncSystem &worldCollisionSyncSystem) {
    std::vector<NetworkEvent> events;
    networkServer.fetchEvents(events);
    for (NetworkEvent &event : events) {
        switch (event.type) {
            case NetworkEventType::NewClient: {
                // Sửa lại sau, cần di chuyển thành file mới
                int entityId = gameWorld.addPlayer(event.clientId);
                sf::Packet packet; packet << static_cast<uint8_t>(TcpPacketType::Assign_EntityID) << entityId;
                networkServer.sendAsync(event.clientId, packet, false);
                
                inputManager.clearClientQueue(event.clientId);

                inventorySyncSystem.syncInventoryToClient(event.clientId);
                inventorySyncSystem.syncEquipmentToClient(event.clientId);

                worldCollisionSyncSystem.syncToClient(event.clientId);

                QuestSystem::acceptQuest(gameWorld.getPlayer(event.clientId), 0);
                QuestSystem::acceptQuest(gameWorld.getPlayer(event.clientId), 1);
            
                break;
            }
            case NetworkEventType::Input:
                inputManager.pushClientInput(event.clientId, event.input);
                
                break;
            case NetworkEventType::Disconnect:
                gameWorld.removePlayer(event.clientId);

                break;
            case NetworkEventType::MoveItem:
                
                if (Player *player = gameWorld.getPlayer(event.clientId);
                    player != nullptr && inventorySystem.moveItem(player, event.from, event.to)) {
                    inventorySyncSystem.syncInventoryToClient(event.clientId);
                }

                break;
            case NetworkEventType::EquipItem:
                if (Player *player = gameWorld.getPlayer(event.clientId);
                    player != nullptr && inventorySystem.equipItem(player, event.from, event.to)) {
                    inventorySyncSystem.syncInventoryToClient(event.clientId);
                    inventorySyncSystem.syncEquipmentToClient(event.clientId);
                }
            
                break;
            case NetworkEventType::MouseSelect:
                if (Player *player = gameWorld.getPlayer(event.clientId);
                    player != nullptr) {
                    int x = event.from, y = event.to;

                    player->setMousePosition(sf::Vector2f( x, y ));
                    player->toggleMouseSelected();
                }

                break;
            default:
                break;
        }
    }
}

int main() {
    NetworkServer networkServer;
    if (!networkServer.start(55001, 55002)) {
        return -1;
    }
    
    GameWorld       gameWorld;
    WorldCollision  worldCollision;
    InputManager    inputManager;
    
    EnemiesManager       enemiesManager(gameWorld);

    EnemyAISystem        enemyAISystem;
    CombatSystem         combatSystem;
    WeaponSystem         weaponSystem;
    InputSystem          inputSystem;
    InterestSystem       interestSystem;
    InventorySystem      inventorySystem;
    PhysicsSystem        physicsSystem(worldCollision);
    WorldCollisionSystem worldCollisionSystem(worldCollision);

    DebugChunkSyncSystem     debugChunkSyncSystem(gameWorld, networkServer);
    GameWorldSyncSystem      gameWorldSyncSystem(gameWorld, networkServer, interestSystem);
    InventorySyncSystem      inventorySyncSystem(gameWorld, networkServer);
    WorldCollisionSyncSystem worldCollisionSyncSystem(worldCollision, networkServer);

    uint64_t seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    Random::seed(seed);

    enemiesManager.loadStaticEnemies();
    worldCollisionSystem.loadStaticCollisions();

    sf::RenderWindow window({ WINDOW_WIDTH, WINDOW_HEIGHT }, "[Server]", sf::Style::Close);
    
    bool isFullscreen = false;
    Renderer renderer(window);
    Font::loadFromFile("Assets/Roboto_Mono.ttf");

    DebugSystem debugSystem;

    if (QuestManager::instance().loadFromFile("Assets/Quests/quests.json") == false) {
        QuestManager::instance().loadFromFile("../Assets/Quests/quests.json");
    }

    sf::Clock clock, sendClock;
    float accumulator = 0.0f;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        accumulator += dt;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                networkServer.close();
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    networkServer.close();
                    window.close();
                }
                else if (event.key.code == sf::Keyboard::F11) {
                    isFullscreen = !isFullscreen;

                    if (isFullscreen) {
                        window.create( sf::VideoMode::getDesktopMode(), "[Server]", sf::Style::None);
                        window.setVerticalSyncEnabled(true);
                        renderer.resize();
                    }
                    else {
                        window.create({ WINDOW_WIDTH, WINDOW_HEIGHT }, "[Server]", sf::Style::Close);
                        window.setVerticalSyncEnabled(true);
                        renderer.resize();
                    }
                }
            }
            else if (event.type == sf::Event::MouseWheelScrolled) {
                renderer.handleScroll(event);
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                renderer.handleMousePressed(event);
            }
            else if (event.type == sf::Event::MouseButtonReleased) {
                renderer.handleMouseReleased(event);
            }
            else if (event.type == sf::Event::MouseMoved) {
                renderer.handleMouseMoved(event);
            }
        }

        syncGameWorldFromClients(gameWorld, inputManager, inventorySystem, inventorySyncSystem, networkServer, worldCollisionSyncSystem);
        networkServer.cleanupDisconnectedClients();

        if (accumulator >= SERVER_TICK) {
            accumulator -= SERVER_TICK;

            enemyAISystem.update(SERVER_TICK, gameWorld, inputManager);

            inputSystem.processPlayerInputs(inputManager, gameWorld, physicsSystem, weaponSystem);
            inputSystem.processEnemyInputs(inputManager, gameWorld, physicsSystem, weaponSystem);

            physicsSystem.resolvePlayerCollisions(gameWorld.getPlayers());
            physicsSystem.resolveEnemyCollisions(gameWorld.getEnemies());
            physicsSystem.resolvePlayerWithEnemyCollisions(gameWorld.getPlayers(), gameWorld.getEnemies());

            combatSystem.handleCollision(gameWorld.getPlayers(), gameWorld.getEnemies(), gameWorld.getDamageEntities());
            combatSystem.handleKilledEvents(gameWorld);

            gameWorld.update(SERVER_TICK);
        }
        
        if (sendClock.getElapsedTime().asSeconds() >= SEND_INTEVAL) {
            sendClock.restart();

            gameWorldSyncSystem.syncToClients();

            debugChunkSyncSystem.syncToClients();
        }

        window.clear(sf::Color(30.0f, 30.0f, 30.0f));
        
        renderer.render(debugSystem.getSnapshot(gameWorld, networkServer));

        window.display();
    }

    return 0;
}