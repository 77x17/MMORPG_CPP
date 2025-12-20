#pragma once

class DamageEntity;
class Enemy;
class Player;
class InputState;

class WeaponSystem {
public:
    DamageEntity * tryFire(Player &player, const InputState &inputState);
    DamageEntity * tryFire(Enemy  &enemy, const InputState &inputState);
};