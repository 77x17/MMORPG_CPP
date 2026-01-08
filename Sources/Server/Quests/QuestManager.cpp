#include "Server/Quests/QuestManager.hpp"

#include <fstream>

#include "Server/Quests/KillObjective.hpp"
#include "Server/Quests/ReachLocationObjective.hpp"

#include "Server/Utils/json.hpp"

using json = nlohmann::json;

QuestManager & QuestManager::instance() {
    static QuestManager instance;
    return instance;
}

const QuestDefinition * QuestManager::getQuest(QuestID id) const {
    auto it = quests.find(id);
    
    if (it == quests.end()) return nullptr;
    return &it->second;
}

bool QuestManager::loadFromFile(const std::string &path) {
    std::ifstream f(path);
    if (!f.is_open()) return false;
    
    json data;
    f >> data;

    for (auto &q : data["quests"]) {
        QuestDefinition def;
        def.id          = q["id"];
        def.name        = q["name"];
        def.description = q["description"];

        for (auto &obj : q["objectives"]) {
            std::string type = obj["type"];

            if (type == "kill") {
                def.objectives.push_back(
                    std::make_unique<KillObjective>(
                        obj["enemyId"],
                        obj["count"]
                    )
                );
            }
            else if (type == "reach") {
                def.objectives.push_back(
                    std::make_unique<ReachLocationObjective>(
                        obj["x"],
                        obj["y"],
                        obj["radius"]
                    )
                );
            }
        }

        quests.emplace(def.id, std::move(def));
    }

    return true;
}