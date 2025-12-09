#pragma once

#include <unordered_map>
#include <vector>

#include "../Shared/InputState.hpp"

#include "WorldSnapshot.hpp"
#include "RemotePlayer.hpp"
#include "RemoteProjectile.hpp"

class EntityManager {
private:
    std::unordered_map<int, RemotePlayer>     remotePlayers;
    std::unordered_map<int, RemoteProjectile> remoteProjectiles;
    std::vector<RemoteProjectile>             localPredictedProjectiles;

    int tempProjectileCounter = 0;

public:
    EntityManager() = default;

    void applySnapshot(const WorldSnapshot &snapshot, int myId, std::vector<InputState> &pendingInputs);

    void update(const float &dt, int myId);

    RemoteProjectile spawnPredictedProjectile(int ownerId, const sf::Vector2f &position, const sf::Vector2f &velocity);

    const std::unordered_map<int, RemotePlayer> & getPlayers() const;
    const std::unordered_map<int, RemoteProjectile> & getProjectiles() const;
    const std::vector<RemoteProjectile> & getPredictedProjectiles() const;
};