#include "Server/Systems/Input/InputSystem.hpp"

#include "Server/Systems/Input/InputManager.hpp"
#include "Server/Core/GameWorld.hpp"
#include "Server/Systems/Physics/PhysicsSystem.hpp"
#include "Server/Systems/Combat/WeaponSystem.hpp"
#include "Server/Entities/Player.hpp"
#include "Server/Entities/DamageEntity.hpp"

#include "Server/Utils/Constants.hpp"

void InputSystem::processPlayerInputs(InputManager &inputManager, GameWorld &gameWorld, PhysicsSystem &physicsSystem, WeaponSystem &weaponSystem) {
    for (Player *player : gameWorld.getPlayers()) if (player != nullptr) {
        InputState latestInput;
        std::vector<InputState> &inputQueue = inputManager.getQueue(player->getId());

        if (!inputQueue.empty()) {
            latestInput = inputQueue.back();
            inputManager.clearQueue(player->getId());
            player->lastProcessedInput = latestInput.seq;
        }

        physicsSystem.updatePlayer(*player, latestInput, SERVER_TICK);

        if (DamageEntity *damageEntity = weaponSystem.tryFire(*player, latestInput)) {
            damageEntity->setId(++nextProjectileId);
            gameWorld.addDamageEntity(damageEntity);
        }
    }
}