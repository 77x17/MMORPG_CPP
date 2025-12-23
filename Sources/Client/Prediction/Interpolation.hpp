#pragma once

#include "Shared/Utils.hpp"

#include "Client/Entities/RemotePlayer.hpp"
#include "Client/Entities/RemoteProjectile.hpp"

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