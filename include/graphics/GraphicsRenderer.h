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
    void clear(const sf::Color& color = sf::Color::Black);    // Display the contents of the window
    void display();

    // Update camera system (call this each frame)
    void updateCamera(float delta_time);

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

    sf::Music m_background_music; // Background music for the game    // --- Camera System ---
    sf::View m_camera_view;          // Main camera view for world rendering
    float m_zoom_level;              // Current zoom factor (1.0 = normal)
    float m_target_zoom_level;       // Target zoom for smooth zooming
    sf::Vector2f m_camera_center;    // Current camera center position
    bool m_is_dragging;              // Track if mouse is currently dragging
    sf::Vector2i m_last_mouse_pos;   // Store last mouse position for drag calculations
    
    // Camera bounds and constraints
    sf::FloatRect m_world_bounds;    // World boundaries for camera constraint
    float m_min_zoom;                // Minimum zoom level
    float m_max_zoom;                // Maximum zoom level
    
    // Camera helper methods
    void updateCameraView();
    void initializeCamera(int world_width, int world_height);
    void constrainCamera();          // Keep camera within world bounds
    void resetCamera();              // Reset camera to default position
    void smoothZoom(float delta_time); // Smooth zoom transitions
};

#endif // GRAPHICS_RENDERER_H
