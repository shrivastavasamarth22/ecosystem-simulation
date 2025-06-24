#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <SFML/Graphics.hpp>
#include "core/World.h"
#include "common/AnimalTypes.h"
#include <map>

class UIManager {
public:
    UIManager();
    bool loadAssets();
    void drawUI(sf::RenderWindow& window, const World& world, bool is_paused);
    void drawSimulationEndedMessage(sf::RenderWindow& window);
    void drawCursor(sf::RenderWindow& window);

private:
    sf::Font m_font;
    sf::Texture m_cursor_texture;
    sf::Sprite m_cursor_sprite;
    std::map<AnimalType, sf::Texture> m_animal_textures;
};

#endif // UIMANAGER_H
