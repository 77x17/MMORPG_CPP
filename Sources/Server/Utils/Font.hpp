#pragma once

#include <SFML/Graphics/Font.hpp>

class Font {
private:
    static sf::Font font;

public:
    static void loadFromFile(const std::string &fileName);
    static const sf::Font & getFont();
};