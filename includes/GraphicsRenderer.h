#ifndef GRAPHICS_RENDERER_H
#define GRAPHICS_RENDERER_H

#include <SFML/Graphics.hpp>
#include <string>
#include "World.h" // Needs World definition for draw functions later

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

    // --- Drawing Methods (Implemented in later phases) ---
    // void drawWorld(const World& world); // Phase 2
    // void drawEntities(const EntityManager& entityManager); // Phase 3
    // void drawUI(const World& world); // Phase 5

    private:
    sf::RenderWindow m_window; // The SFML window object
    int m_tile_size;
};

#endif // GRAPHICS_RENDERER_H