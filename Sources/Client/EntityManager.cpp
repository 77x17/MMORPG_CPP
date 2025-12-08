#include "EntityManager.hpp"

#include "../Shared/Utils.hpp"
#include "../Shared/Constants.hpp"

void EntityManager::applySnapshot(const WorldSnapshot &snapshot, int myId, std::vector<InputState> &pendingInputs) {
    std::unordered_map<int, RemotePlayer> newPlayers;
    for (const PlayerSnapshot &playerSnapshot : snapshot.players) {
        RemotePlayer remotePlayer;
        remotePlayer.id             = playerSnapshot.id;
        remotePlayer.serverPosition = { playerSnapshot.x, playerSnapshot.y };
        remotePlayer.serverVelocity = { 0.0f, 0.0f };
        remotePlayer.hp             = playerSnapshot.hp;
        remotePlayer.lastAck        = playerSnapshot.lastProcessed;

        if (remotePlayers.count(remotePlayer.id)) {
            remotePlayer.localPosition = remotePlayers[remotePlayer.id].localPosition;
        }
        else {
            remotePlayer.localPosition = remotePlayer.serverPosition;
        }

        if (remotePlayer.id == myId) {
            // reconcile pending inputs: remove acknowledged
            while (!pendingInputs.empty() && pendingInputs.front().seq <= remotePlayer.lastAck) {
                pendingInputs.erase(pendingInputs.begin());
            }

            // recompute local predicted position using remaining pendingInputs (left as before)
            sf::Vector2f reconciled = remotePlayer.serverPosition;
            for (InputState &input : pendingInputs) {
                reconciled += input.movementDir * PLAYER_SPEED * SERVER_TICK;
            }

            remotePlayer.localPosition = lerp(remotePlayer.localPosition, reconciled, 0.6f);
        }

        newPlayers[remotePlayer.id] = remotePlayer;
    }
    remotePlayers.swap(newPlayers);

    std::unordered_map<int, RemoteProjectile> newProjectiles;
    for (const ProjectileSnapshot &projectilesSnapshot : snapshot.projectiles) {
        RemoteProjectile remoteProjectile;
        remoteProjectile.id             = projectilesSnapshot.id;
        remoteProjectile.serverPosition = { projectilesSnapshot.x, projectilesSnapshot.y };
        remoteProjectile.velocity       = { projectilesSnapshot.vx, projectilesSnapshot.vy };
        remoteProjectile.ownerId        = projectilesSnapshot.ownerId;
        remoteProjectile.authoritative  = true;

        if (remoteProjectiles.count(remoteProjectile.id)) {
            remoteProjectile.localPosition = remoteProjectiles[remoteProjectile.id].localPosition;
        }
        else {
            remoteProjectile.localPosition = remoteProjectile.serverPosition;
        }

        newProjectiles[remoteProjectile.id] = remoteProjectile;
    }

    for (auto it = localPredictedProjectiles.begin(); it != localPredictedProjectiles.end(); ) {
        bool matched = false;
        for (auto &[id, remoteProjectile] : newProjectiles) {
            if (remoteProjectile.ownerId == it->ownerId && distance(remoteProjectile.serverPosition, it->localPosition) < 40.0f) {
                remoteProjectile.localPosition = it->localPosition;
                
                matched = true;
                break;
            }
        }
        it = (matched ? localPredictedProjectiles.erase(it) : ++it);
    }

    for (auto &[id, remoteProjectile] : newProjectiles) {
        remoteProjectiles[id] = remoteProjectile;
    }

    for (auto it = remoteProjectiles.begin(); it != remoteProjectiles.end(); ) {
        it = ((newProjectiles.count(it->first) == 0) ? remoteProjectiles.erase(it) : ++it);
    }
}

void EntityManager::update(const float &dt, int myId) {
    for (auto &[id, remotePlayer] : remotePlayers) {
        if (remotePlayer.id == myId) continue;
        remotePlayer.localPosition = lerp(remotePlayer.localPosition, remotePlayer.serverPosition, 0.2f);
    }

    for (auto &[id, remoteProjectile] : remoteProjectiles) {
        remoteProjectile.localPosition += remoteProjectile.velocity * dt;
        if (distance(remoteProjectile.localPosition, remoteProjectile.serverPosition) > 50.0f) {
            remoteProjectile.localPosition = lerp(remoteProjectile.localPosition, remoteProjectile.serverPosition, 0.3f);
        }
    }

    for (RemoteProjectile &predictedProjectile : localPredictedProjectiles) {
        predictedProjectile.localPosition += predictedProjectile.velocity * dt;
    }
}

RemoteProjectile EntityManager::spawnPredictedProjectile(int ownerId, const sf::Vector2f &position, const sf::Vector2f &velocity) {
    RemoteProjectile remoteProjectile;
    remoteProjectile.id             = --tempProjectileCounter;
    remoteProjectile.ownerId        = ownerId;
    remoteProjectile.localPosition  = position;
    remoteProjectile.serverPosition = position;
    remoteProjectile.velocity       = velocity;
    remoteProjectile.authoritative  = false;

    localPredictedProjectiles.push_back(remoteProjectile);
    return remoteProjectile;
}

const std::unordered_map<int, RemotePlayer> & EntityManager::getPlayers() const {
    return remotePlayers;
}

const std::unordered_map<int, RemoteProjectile> & EntityManager::getProjectiles() const {
    return remoteProjectiles;
}

const std::vector<RemoteProjectile> & EntityManager::getPredictedProjectiles() const {
    return localPredictedProjectiles;
}