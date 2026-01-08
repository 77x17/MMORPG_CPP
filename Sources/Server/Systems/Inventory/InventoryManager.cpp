#include "Server/Systems/Inventory/InventoryManager.hpp"

#include <fstream>

#include "Server/Systems/Inventory/Inventory.hpp"
#include "Server/Systems/Inventory/Equipment.hpp"

#include "Server/Utils/json.hpp"

using json = nlohmann::json;

static json loadPlayerJson(int playerId) {
    std::ifstream file("Assets/Data/Players/player_" + std::to_string(playerId) + ".json");
    if (!file.is_open())
        return json::object(); 

    json j;
    file >> j;
    return j;
}

bool InventoryManager::loadInventory(int playerId, Inventory &inventory) {
    std::ifstream file("Assets/Data/Players/player_" + std::to_string(playerId) + ".json");
    if (not file.is_open()) {
        return false;
    }

    json j;
    file >> j;

    auto& _inventory = j["inventory"];
    for (size_t i = 0; i < 36; ++i) {
        if (i < _inventory.size()) {
            inventory[i].id = _inventory[i].value("itemId", -1);
        }
        else {
            inventory[i].id = -1;
        }
    }
    return true;
}

bool InventoryManager::saveInventory(int playerId, const Inventory &inventory) {
    json j = loadPlayerJson(playerId);
    j["inventory"].clear();
    for (size_t i = 0; i < 36; ++i) {
        j["inventory"].push_back({ 
            { "itemId", inventory[i].id } 
        });
    }

    std::ofstream file("Assets/Data/Players/player_" + std::to_string(playerId) + ".json");
    if (not file.is_open()) {
        return false;
    }

    file << j.dump(4);
    return true;
}

bool InventoryManager::loadEquipment(int playerId, Equipment &equipment) {
    std::ifstream file("Assets/Data/Players/player_" + std::to_string(playerId) + ".json");
    if (not file.is_open()) {
        return false;
    }

    json j;
    file >> j;

    if (not j.contains("equipment")) {
        return false;
    }

    auto& _equipment = j["equipment"];
    for (size_t i = 0; i < 8; ++i) {
        if (i < _equipment.size()) {
            equipment[i].id = _equipment[i].value("itemId", -1);
        }
        else {
            equipment[i].id = -1;
        }
    }
    return true;
}

bool InventoryManager::saveEquipment(int playerId, const Equipment &equipment) {
    json j = loadPlayerJson(playerId);
    j["equipment"].clear();
    for (size_t i = 0; i < 8; ++i) {
        j["equipment"].push_back({ 
            { "itemId", equipment[i].id } 
        });
    }

    std::ofstream file("Assets/Data/Players/player_" + std::to_string(playerId) + ".json");
    if (not file.is_open()) {
        return false;
    }

    file << j.dump(4);
    return true;
}
