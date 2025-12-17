#include "Client/Systems/Entity/EntityManager.hpp"
#include "Client/Systems/Prediction/Interpolation.hpp"

#include "Shared/Utils.hpp"
#include "Shared/Constants.hpp"

#include "Client/Snapshots/WorldSnapshot.hpp"

void EntityManager::applySnapshot(const WorldSnapshot &snapshot, int clientId, std::vector<InputState> &pendingInputs) {
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

        if (remotePlayer.id == clientId) {
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

    for (auto &[id, remoteProjectile] : newProjectiles) {
        remoteProjectiles[id] = remoteProjectile;
    }

    for (auto it = remoteProjectiles.begin(); it != remoteProjectiles.end(); ) {
        it = ((newProjectiles.count(it->first) == 0) ? remoteProjectiles.erase(it) : ++it);
    }

    std::unordered_map<int, RemoteSwordSlash> newSwordSlashs;
    for (const SwordSlashSnapshot &swordSlashSnapshot : snapshot.swordSlashs) {
        RemoteSwordSlash remoteSwordSlash;
        remoteSwordSlash.id             = swordSlashSnapshot.id;
        remoteSwordSlash.serverPosition = { swordSlashSnapshot.left, swordSlashSnapshot.top };
        remoteSwordSlash.size           = { swordSlashSnapshot.width, swordSlashSnapshot.height };
        remoteSwordSlash.ownerId        = swordSlashSnapshot.ownerId;
        remoteSwordSlash.authoritative  = true;

        remoteSwordSlash.localPosition = remoteSwordSlash.serverPosition;

        newSwordSlashs[remoteSwordSlash.id] = remoteSwordSlash;
    }

    for (auto &[id, remoteSwordSlash] : newSwordSlashs) {
        remoteSwordSlashs[id] = remoteSwordSlash;
    }

    for (auto it = remoteSwordSlashs.begin(); it != remoteSwordSlashs.end(); ) {
        it = ((newSwordSlashs.count(it->first) == 0) ? remoteSwordSlashs.erase(it) : ++it);
    }
}

void EntityManager::update(const float &dt, int clientId) {
    for (auto &[id, remotePlayer] : remotePlayers) {
        if (remotePlayer.id == clientId) continue;
        remotePlayer.localPosition = lerp(remotePlayer.localPosition, remotePlayer.serverPosition, 0.2f);
    }

    for (auto &[id, remoteProjectile] : remoteProjectiles) {
        remoteProjectile.localPosition += remoteProjectile.velocity * dt;
        remoteProjectile.localPosition = lerp(remoteProjectile.localPosition, remoteProjectile.serverPosition, 0.5f);
    }
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

bool EntityManager::findPlayer(int myId) const {
    return remotePlayers.count(myId) > 0;
}

RemotePlayer & EntityManager::getPlayer(int myId) {
    return remotePlayers.at(myId);
}

const RemotePlayer & EntityManager::getPlayer(int myId) const {
    return remotePlayers.at(myId);
}