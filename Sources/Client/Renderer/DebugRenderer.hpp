#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#include "Shared/AABB.hpp"

class EntityManager;
class WorldCollision;
class ChunkSnapshot;

class DebugRenderer {
private:
    sf::RenderWindow &window;
    bool enabled = false;

    std::vector<AABB> chunks;

private:
    void drawWorldCollision(WorldCollision &worldCollision);
    void drawChunks();

public:
    DebugRenderer(sf::RenderWindow &_window);

    void applySnapshot(ChunkSnapshot & chunkSnapshot);

    void toggle();
    bool isEnabled() const;

    void render(const EntityManager &entityManager, WorldCollision &worldCollision);
};