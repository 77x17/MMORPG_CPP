#pragma once

class GameWorld;

class EnemiesManager {
private:
    GameWorld &gameWorld;

public:
    EnemiesManager(GameWorld &_gameWorld);
    
    void loadStaticEnemies();
};