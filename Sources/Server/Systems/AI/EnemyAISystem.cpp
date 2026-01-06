#include "Server/Systems/AI/EnemyAISystem.hpp"

#include "Shared/Utils.hpp"

#include "Server/Core/GameWorld.hpp"
#include "Server/Entities/Enemy.hpp"
#include "Server/Entities/Player.hpp"
#include "Server/Systems/Input/InputManager.hpp"
#include "Server/Utils/Random.hpp"

void EnemyAISystem::update(const float &dt, GameWorld &gameWorld, InputManager &inputManager) {
    for (Enemy *enemy : gameWorld.getEnemies()) {
        if (enemy == nullptr) continue;
        if (enemy->isDestroyed()) continue;

        InputState input;

        Player *target = gameWorld.findNearestPlayer(enemy->getPosition());
        if (target == nullptr || distance(target->getPosition(), enemy->getPosition()) > enemy->getDetectRange()) {
            enemy->unChase();
            if (enemy->getRandomCooldownTimer() <= 0.0f) {
                enemy->resetRandomCooldownTimer();

                if (Random::chance(0.7)) {
                    input.movementDir = { Random::rangeFloat(-1.0f, 1.0f), Random::rangeFloat(-1.0f, 1.0f) };
                }
                else {
                    input.movementDir = { 0.0f, 0.0f };
                }

                enemy->setPreviousMovement(input.movementDir);
            }
            else {
                input.movementDir = enemy->getPreviousMovement();
            }
        }
        else {
            enemy->toggleChase();

            float nearestPlayerDistance = distance(target->getPosition(), enemy->getPosition());
            
            sf::Vector2f direction = normalize(target->getPosition() - enemy->getPosition());
            input.movementDir = direction;
            
            // if (nearestPlayerDistance < 200.0f) {
            if (nearestPlayerDistance < 70.0f) {
                input.isShooting = true;
            }
        }
        
        input.seq = ++enemy->inputSeq;
        inputManager.pushEnemyInput(enemy->getId(), input);
    }
}