#include "Server/Utils/Font.hpp"

#include <iostream>
#include "Server/Systems/Log/LogSystem.hpp"

void Font::loadFromFile(const std::string &fileName) {
    if (!font.loadFromFile(fileName)) {
        if (!font.loadFromFile("../" + fileName)) {
            std::cout << "[Font] - Cannot load from " << fileName << '\n';
            LogSystem::addMessage("[Font] - Cannot load from " + fileName);
        }
    }
}

const sf::Font & Font::getFont() {
    return font;
}