#ifndef GRAPHICS_RENDERER_H
#define GRAPHICS_RENDERER_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <map>
#include "core/World.h" // Needs World definition for draw functions later
#include "core/EntityManager.h" // Needs EntityManager definition for draw functions later
#include "resources/Resource.h" // Needs Resource definition for draw functions later
#include "common/AnimalTypes.h"

class GraphicsRenderer {
    public:
    GraphicsRenderer();
    ~GraphicsRenderer();

    // Initialize the SFML window
    void init(int world_width, int world_height, int tile_size, const std::string& title);

    // Check if the window is open
    bool isOpen() const;

    // Handle SFML events
    void handleEvents();

    // Clear the window with a background color
    void clear(const sf::Color& color = sf::Color::Black);

    // Display the contents of the window
    void display();

    // --- Drawing Methods ---
    void drawWorld(const World& world); 
    void drawEntities(const EntityManager& entityManager);
    void drawUI(const World& world, bool is_paused); 
    void drawSimulationEndedMessage(); // NEW: Draw "Simulation Ended" message
    private:
    sf::RenderWindow m_window;
    int m_tile_size;

    // --- NEW: Textures for Tiles ---
    sf::Texture m_empty_tile_texture; // Texture for empty tiles
    // We'll use a map for resource types as it's extensible
    std::map<const ResourceType*, sf::Texture> m_resource_tile_textures;


    // Add animal textures/font here in later phases
    std::map<AnimalType, sf::Texture> m_animal_textures;

    sf::Font m_font;

    sf::Music m_background_music; // Background music for the game
};

#endif // GRAPHICS_RENDERER_H
