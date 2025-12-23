#pragma once

#include "Client/States/IClientState.hpp"

namespace sf { class RenderWindow; }

class NetworkClient;

#include "Client/Entities/EntityManager.hpp"

#include "Client/Inventory/Inventory.hpp"
#include "Client/Inventory/Equipment.hpp"

#include "Client/Renderer/Renderer.hpp"
#include "Client/Renderer/DebugRenderer.hpp"

#include "Client/World/WorldCollision.hpp"

class InGameState : public IClientState {
private:
    sf::RenderWindow &window;
    NetworkClient    &networkClient;

    // === Game Data ===
    EntityManager  entityManager;
    Inventory      inventory;
    Equipment      equipment;
    WorldCollision worldCollision;

    // === Render ===
    Renderer      renderer;
    DebugRenderer debugRenderer;

    // === Input Prediction ===
    int inputSeq = 0;
    std::vector<InputState> pendingInputs;

    float clientAccumulator = 0.0f;

public:
    InGameState(sf::RenderWindow &_window, NetworkClient &_networkClient);

    void handleEvent(const sf::Event &event) override;
    void update(float dt) override;
    void render(sf::RenderWindow &window) override;
};