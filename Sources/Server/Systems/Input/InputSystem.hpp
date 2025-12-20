#pragma once

class InputManager;
class GameWorld;
class PhysicsSystem;
class WeaponSystem;
class DamageEntity;

class InputSystem {
private:
    int nextProjectileId = 0;

public:
    InputSystem() = default;

    void processPlayerInputs(InputManager &inputManager, GameWorld &gameWorld, PhysicsSystem &physicsSystem, WeaponSystem &weaponSystem);
    void processEnemyInputs(InputManager &inputManager, GameWorld &gameWorld, PhysicsSystem &physicsSystem, WeaponSystem &weaponSystem);
};