#include "Client/Entities/EntityManager.hpp"
#include "Client/Prediction/Interpolation.hpp"

#include "Shared/Utils.hpp"
#include "Shared/Constants.hpp"
#include "Server/Utils/Constants.hpp"

#include "Client/Snapshots/WorldSnapshot.hpp"

void EntityManager::applySnapshot(const WorldSnapshot &snapshot, int myEntityId, std::vector<InputState> &pendingInputs) {
    std::unordered_map<int, RemotePlayer> newRemotePlayers;
    for (const PlayerSnapshot &playerSnapshot : snapshot.players) {
        RemotePlayer remotePlayer;
        remotePlayer.id             = playerSnapshot.id;
        remotePlayer.serverPosition = { playerSnapshot.x, playerSnapshot.y };
        remotePlayer.serverVelocity = { 0.0f, 0.0f };
        remotePlayer.hp             = playerSnapshot.hp;
        remotePlayer.lastAck        = playerSnapshot.lastProcessed;
        remotePlayer.name           = playerSnapshot.name;

        if (remotePlayers.count(remotePlayer.id)) {
            remotePlayer.localPosition = remotePlayers[remotePlayer.id].localPosition;
        }
        else {
            remotePlayer.localPosition = remotePlayer.serverPosition;
        }

        if (remotePlayer.id == myEntityId) {
            // reconcile pending inputs: remove acknowledged
            while (!pendingInputs.empty() && pendingInputs.front().seq <= remotePlayer.lastAck) {
                pendingInputs.erase(pendingInputs.begin());
            }

            // recompute local predicted position using remaining pendingInputs (left as before)
            sf::Vector2f reconciledPosition = remotePlayer.serverPosition;
            for (InputState &input : pendingInputs) {
                reconciledPosition += normalize(input.movementDir) * PLAYER_SPEED * SERVER_TICK;
            }

            float distanceError = distance(remotePlayer.localPosition, reconciledPosition);
            if (distanceError > 100.0f) {
                remotePlayer.localPosition = lerp(remotePlayer.localPosition, reconciledPosition, 0.6f);
            }
            else {
                remotePlayer.localPosition = lerp(remotePlayer.localPosition, reconciledPosition, 0.35f);
            }
        }

        newRemotePlayers[remotePlayer.id] = remotePlayer;
    }
    remotePlayers.swap(newRemotePlayers);


    std::unordered_map<int, RemoteProjectile> newRemoteProjectiles;
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

        newRemoteProjectiles[remoteProjectile.id] = remoteProjectile;
    }

    for (auto &[id, remoteProjectile] : newRemoteProjectiles) {
        remoteProjectiles[id] = remoteProjectile;
    }

    for (auto it = remoteProjectiles.begin(); it != remoteProjectiles.end(); ) {
        it = ((newRemoteProjectiles.count(it->first) == 0) ? remoteProjectiles.erase(it) : ++it);
    }


    std::unordered_map<int, RemoteSwordSlash> newRemoteSwordSlashs;
    for (const SwordSlashSnapshot &swordSlashSnapshot : snapshot.swordSlashs) {
        RemoteSwordSlash remoteSwordSlash;
        remoteSwordSlash.id             = swordSlashSnapshot.id;
        remoteSwordSlash.serverPosition = { swordSlashSnapshot.left, swordSlashSnapshot.top };
        remoteSwordSlash.size           = { swordSlashSnapshot.width, swordSlashSnapshot.height };
        remoteSwordSlash.ownerId        = swordSlashSnapshot.ownerId;
        remoteSwordSlash.authoritative  = true;

        remoteSwordSlash.localPosition = remoteSwordSlash.serverPosition;

        newRemoteSwordSlashs[remoteSwordSlash.id] = remoteSwordSlash;
    }

    for (auto &[id, remoteSwordSlash] : newRemoteSwordSlashs) {
        remoteSwordSlashs[id] = remoteSwordSlash;
    }

    for (auto it = remoteSwordSlashs.begin(); it != remoteSwordSlashs.end(); ) {
        it = ((newRemoteSwordSlashs.count(it->first) == 0) ? remoteSwordSlashs.erase(it) : ++it);
    }


    std::unordered_map<int, RemoteEnemy> newRemoteEnemies;
    for (const EnemySnapshot &enemySnapshot : snapshot.enemies) {
        RemoteEnemy remoteEnemy;
        remoteEnemy.entityId       = enemySnapshot.entityId;
        remoteEnemy.enemyId        = enemySnapshot.enemyId;
        remoteEnemy.serverPosition = { enemySnapshot.x, enemySnapshot.y };
        remoteEnemy.serverVelocity = { 0.0f, 0.0f };
        remoteEnemy.hp             = enemySnapshot.hp;

        if (remoteEnemies.count(remoteEnemy.entityId)) {
            remoteEnemy.localPosition = remoteEnemies[remoteEnemy.entityId].localPosition;
        }
        else {
            remoteEnemy.localPosition = remoteEnemy.serverPosition;
        }

        newRemoteEnemies[remoteEnemy.entityId] = remoteEnemy;
    }
    remoteEnemies.swap(newRemoteEnemies);
}

void EntityManager::update(const float &dt, int myEntityId) {
    for (auto &[id, remotePlayer] : remotePlayers) {
        if (remotePlayer.id == myEntityId) continue;
        remotePlayer.localPosition = lerp(remotePlayer.localPosition, remotePlayer.serverPosition, 0.2f);
    }

    for (auto &[id, remoteProjectile] : remoteProjectiles) {
        remoteProjectile.localPosition += remoteProjectile.velocity * dt;
        remoteProjectile.localPosition = lerp(remoteProjectile.localPosition, remoteProjectile.serverPosition, 0.5f);
    }

    for (auto &[id, remoteEnemy] : remoteEnemies) {
        remoteEnemy.localPosition = lerp(remoteEnemy.localPosition, remoteEnemy.serverPosition, 0.2f);
    }
}

const std::unordered_map<int, RemoteEnemy> & EntityManager::getEnemies() const {
    return remoteEnemies;
}

const std::unordered_map<int, RemotePlayer> & EntityManager::getPlayers() const {
    return remotePlayers;
}

const std::unordered_map<int, RemoteProjectile> & EntityManager::getProjectiles() const {
    return remoteProjectiles;
}

const std::unordered_map<int, RemoteSwordSlash> & EntityManager::getSwordSlashs() const {
    return remoteSwordSlashs;
}

RemotePlayer * EntityManager::getPlayer(int myEntityId) {
    auto it = remotePlayers.find(myEntityId);
    if (it == remotePlayers.end()) {
        return nullptr;
    }
    return &it->second;
}

const RemotePlayer * EntityManager::getPlayer(int myEntityId) const {
    const auto it = remotePlayers.find(myEntityId);
    if (it == remotePlayers.end()) {
        return nullptr;
    }
    return &it->second;
}

int EntityManager::getDamageEntitiesSize() const {
    return remoteProjectiles.size() + remoteSwordSlashs.size();
}