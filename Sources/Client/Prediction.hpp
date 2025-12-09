#pragma once

#include <vector>
#include <unordered_map>

#include "RemotePlayer.hpp"
#include "RemoteProjectile.hpp"

#include "../Shared/Utils.hpp"

namespace ClientPrediction {
    inline void predictLocalPlayer(RemotePlayer &localPlayer, const float &dt) {
        localPlayer.localPosition += localPlayer.serverVelocity * dt;
    }

    struct PredictedProjectile {
        int id;
        sf::Vector2f position;
        sf::Vector2f velocity;
        float life = 0.0f;
    };

    inline void updatePredictedProjectiles(std::vector<PredictedProjectile> &projectiles, const float &dt) {
        for (PredictedProjectile &predictedProjectile : projectiles) {
            predictedProjectile.position += predictedProjectile.velocity * dt;
            predictedProjectile.life     += dt;
        }
    }

    inline void reconcileProjectiles(std::vector<PredictedProjectile> &projectiles, std::unordered_map<int, RemoteProjectile> &remoteMap) {
        for (auto it = projectiles.begin(); it != projectiles.end();) {
            bool matched = false;
            for (auto &kv : remoteMap) {
                auto &rp = kv.second;
                if (distance(rp.serverPosition, it->position) < 16.f && rp.ownerId == it->id) {
                    matched = true;
                    break;
                }
            }
            if (matched) it = projectiles.erase(it);
            else ++it;
        }
    }
}