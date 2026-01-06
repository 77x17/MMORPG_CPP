#include "Server/Entities/EnemiesManager.hpp"

#include "Server/Core/GameWorld.hpp"

EnemiesManager::EnemiesManager(GameWorld &_gameWorld) 
: gameWorld(_gameWorld) {

}

void EnemiesManager::loadStaticEnemies() {
    for (int i = 0; i < 16; ++i) {
        gameWorld.addEnemy();
    }
}