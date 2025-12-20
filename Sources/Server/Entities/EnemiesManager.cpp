#include "Server/Entities/EnemiesManager.hpp"

#include "Server/Core/GameWorld.hpp"

EnemiesManager::EnemiesManager(GameWorld &_gameWorld) 
: gameWorld(_gameWorld) {

}

void EnemiesManager::loadStaticEnemies() {
    gameWorld.addEnemy(1);
    gameWorld.addEnemy(2);
    gameWorld.addEnemy(3);
    gameWorld.addEnemy(4);
}