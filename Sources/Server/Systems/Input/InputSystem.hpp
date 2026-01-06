#pragma once

class InputManager;
class GameWorld;
class PhysicsSystem;
class WeaponSystem;
class DamageEntity;

class InputSystem {
public:
    InputSystem() = default;

    void processPlayerInputs(InputManager &inputManager, GameWorld &gameWorld, PhysicsSystem &physicsSystem, WeaponSystem &weaponSystem);
    void processEnemyInputs(InputManager &inputManager, GameWorld &gameWorld, PhysicsSystem &physicsSystem, WeaponSystem &weaponSystem);
};