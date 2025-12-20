#include "Client/Utils/Font.hpp"

#include <iostream>

void Font::loadFromFile(const std::string &fileName) {
    if (!font.loadFromFile(fileName)) {
        if (!font.loadFromFile("../" + fileName)) {
            std::cout << "[Font] - Cannot load from " << fileName << '\n';
        }
    }
}

const sf::Font & Font::getFont() {
    return font;
}