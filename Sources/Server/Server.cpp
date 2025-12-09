#include "GameServer.hpp"

int main() {
    GameServer gameServer;
    
    if (!gameServer.start(55001)) {
        return 0;
    }
    
    gameServer.run();
    
    return 0;
}