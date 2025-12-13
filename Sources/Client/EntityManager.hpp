#pragma once

#include <unordered_map>
#include <vector>

#include "../Shared/InputState.hpp"

#include "WorldSnapshot.hpp"
#include "RemotePlayer.hpp"
#include "RemoteProjectile.hpp"
#include "RemoteSwordSlash.hpp"

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

    RemotePlayer & getPlayer(int myId);
};