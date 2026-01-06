#include "Server/Systems/Input/InputSystem.hpp"

#include "Server/Systems/Input/InputManager.hpp"
#include "Server/Core/GameWorld.hpp"
#include "Server/Systems/Physics/PhysicsSystem.hpp"
#include "Server/Systems/Combat/WeaponSystem.hpp"
#include "Server/Entities/Enemy.hpp"
#include "Server/Entities/Player.hpp"
#include "Server/Entities/DamageEntity.hpp"

#include "Server/Utils/Constants.hpp"

void InputSystem::processPlayerInputs(InputManager &inputManager, GameWorld &gameWorld, PhysicsSystem &physicsSystem, WeaponSystem &weaponSystem) {
    for (Player *player : gameWorld.getPlayers()) {
        if (player == nullptr) continue;
        if (player->isDestroyed()) continue;

        InputState latestInput;
        std::vector<InputState> &inputQueue = inputManager.getClientQueue(player->getId());

        if (!inputQueue.empty()) {
            latestInput = inputQueue.back();
            inputManager.clearClientQueue(player->getId());
            player->lastProcessedInput = latestInput.seq;
        }

        physicsSystem.updatePlayer(*player, latestInput, SERVER_TICK);

        if (DamageEntity *damageEntity = weaponSystem.tryFire(*player, latestInput)) {
            damageEntity->setId(++nextProjectileId);
            gameWorld.addDamageEntity(damageEntity);
        }
    }
}

void InputSystem::processEnemyInputs(InputManager &inputManager, GameWorld &gameWorld, PhysicsSystem &physicsSystem, WeaponSystem &weaponSystem) {
    for (Enemy *enemy : gameWorld.getEnemies()) {
        if (enemy == nullptr) continue;
        if (enemy->isDestroyed()) continue;

        InputState latestInput;
        std::vector<InputState> &inputQueue = inputManager.getEnemyQueue(enemy->getId());

        if (!inputQueue.empty()) {
            latestInput = inputQueue.back();
            inputManager.getEnemyQueue(enemy->getId());
            enemy->lastProcessedInput = latestInput.seq;
        }

        physicsSystem.updateEnemy(*enemy, latestInput, SERVER_TICK);

        if (DamageEntity *damageEntity = weaponSystem.tryFire(*enemy, latestInput)) {
            damageEntity->setId(++nextProjectileId);
            gameWorld.addDamageEntity(damageEntity);
        }
    }
}