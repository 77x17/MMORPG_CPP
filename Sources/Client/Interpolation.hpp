#pragma once

// #include <SFML/System.hpp>
// #include <unordered_map>

#include "RemotePlayer.hpp"
#include "RemoteProjectile.hpp"
#include "../Shared/Utils.hpp"

namespace ClientInterpolation {
    inline void interpolatePlayer(RemotePlayer &player, const float &alpha) {
        player.localPosition = lerp(player.localPosition, player.serverPosition, alpha);
    }

    inline void extrapolateProjectile(RemoteProjectile &projectile, const float &dt) {
        projectile.localPosition += projectile.velocity * dt;
    }

    inline void snapProjectileToServer(RemoteProjectile &projectile) {
        projectile.localPosition = projectile.serverPosition;
    }
}