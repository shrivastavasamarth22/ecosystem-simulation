#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <SFML/Graphics.hpp>
#include "core/World.h"
#include "core/EntityManager.h"
#include "graphics/Camera.h"
#include "common/AnimalTypes.h"
#include <map>

class UIManager {
public:
    UIManager();
    bool loadAssets();
    void drawUI(sf::RenderWindow& window, const World& world, bool is_paused);
    void drawEntityDetailPanel(sf::RenderWindow& window, const EntityManager& entityManager, const Camera& camera);
    void drawSimulationEndedMessage(sf::RenderWindow& window);
    void drawCursor(sf::RenderWindow& window);

private:
    sf::Font m_font;
    sf::Texture m_cursor_texture;
    sf::Sprite m_cursor_sprite;
    std::map<AnimalType, sf::Texture> m_animal_textures;
    
    // Performance optimization: cache for entity detail panel
    mutable size_t m_last_selected_entity;
    mutable std::string m_cached_panel_content;
    
    // Helper methods for entity detail panel
    std::string getAnimalTypeString(AnimalType type);
    std::string getAIStateString(AIState state);
    std::string getHealthCondition(float health, float max_health);
    std::string getEnergyCondition(float energy, float max_energy);
    std::string getAgeStage(int age, int prime_age);
};

#endif // UIMANAGER_H
