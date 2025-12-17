#pragma once

class DamageEntity;
class Player;
class InputState;

class WeaponSystem {
public:
    DamageEntity * tryFire(Player &player, const InputState &inputState);
};