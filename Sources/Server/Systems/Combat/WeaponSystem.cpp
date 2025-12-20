#include "Server/Systems/Combat/WeaponSystem.hpp"

#include "Shared/Constants.hpp"
#include "Shared/InputState.hpp"

#include "Server/Entities/DamageEntity.hpp"
#include "Server/Entities/Enemy.hpp"
#include "Server/Entities/Player.hpp"
#include "Server/Entities/Projectile.hpp"
#include "Server/Entities/SwordSlash.hpp"

DamageEntity * WeaponSystem::tryFire(Player &player, const InputState &inputState) {
    if (not inputState.isShooting) return nullptr;
    if (player.getProjectileCooldownTimer() > 0) return nullptr;

    player.resetProjectileCooldownTimer();

    sf::Vector2f direction = inputState.movementDir;
    if (direction.x == 0.0f && direction.y == 0.0f) {
        direction = player.getOldShootDir();
    }

    int weapon = player.getEquipment().getWeapon().id;

    if (weapon == 0) {
        return new Projectile(player.getId(), player.getPosition(), direction);
    }
    else if (weapon == 1) {
        return new SwordSlash(player.getId(), player.getPosition(), direction);
    }
    
    return nullptr;
}

DamageEntity * WeaponSystem::tryFire(Enemy &enemy, const InputState &inputState) {
    if (not inputState.isShooting) return nullptr;
    if (enemy.getProjectileCooldownTimer() > 0) return nullptr;

    enemy.resetProjectileCooldownTimer();

    sf::Vector2f direction = inputState.movementDir;
    if (direction.x == 0.0f && direction.y == 0.0f) {
        direction = enemy.getOldShootDir();
    }

    // int weapon = enemy.getEquipment().getWeapon().id;
    int weapon = 1;

    if (weapon == 0) {
        return new Projectile(enemy.getId(), enemy.getPosition(), direction);
    }
    else if (weapon == 1) {
        return new SwordSlash(enemy.getId(), enemy.getPosition(), direction);
    }
    
    return nullptr;
}