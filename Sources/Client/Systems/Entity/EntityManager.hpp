#pragma once

#include <unordered_map>
#include <vector>

#include "Shared/InputState.hpp"

class WorldSnapshot;

#include "Client/Entities/RemotePlayer.hpp"
#include "Client/Entities/RemoteProjectile.hpp"
#include "Client/Entities/RemoteSwordSlash.hpp"

class EntityManager {
private:
    std::unordered_map<int, RemotePlayer>     remotePlayers;
    std::unordered_map<int, RemoteProjectile> remoteProjectiles;
    std::unordered_map<int, RemoteSwordSlash> remoteSwordSlashs;

public:
    EntityManager() = default;

    void applySnapshot(const WorldSnapshot &snapshot, int myId, std::vector<InputState> &pendingInputs);

    void update(const float &dt, int myId);

    const std::unordered_map<int, RemotePlayer> & getPlayers() const;
    const std::unordered_map<int, RemoteProjectile> & getProjectiles() const;
    const std::unordered_map<int, RemoteSwordSlash> & getSwordSlashs() const;

    bool findPlayer(int myId) const;

    RemotePlayer & getPlayer(int myId);
    const RemotePlayer & getPlayer(int myId) const;
};