#ifndef GRAPHICS_RENDERER_H
#define GRAPHICS_RENDERER_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <map>
#include <memory>
#include "core/World.h"
#include "core/EntityManager.h"
#include "resources/Resource.h"
#include "resources/Biome.h" // <-- NEW: Include Biome for texture mapping
#include "common/AnimalTypes.h"
#include "graphics/Camera.h"
#include "graphics/UIManager.h"

class GraphicsRenderer {
public:
    GraphicsRenderer();
    ~GraphicsRenderer();

    void init(unsigned int window_width, unsigned int window_height, int world_width, int world_height, int tile_size, const std::string& title);
    bool isOpen() const;
    void handleEvents();
    void clear(const sf::Color& color = sf::Color::Black);
    void display();
    void update(float delta_time);

    void drawWorld(const World& world);
    void drawEntities(const EntityManager& entityManager);
    void drawUI(const World& world, bool is_paused);
    void drawSimulationEndedMessage();
    void drawCursor();

private:
    // Helper method for view frustum culling
    sf::FloatRect getVisibleBounds() const;

    sf::RenderWindow m_window;
    int m_tile_size;
    sf::Texture m_empty_tile_texture;
    std::map<const ResourceType*, sf::Texture> m_resource_tile_textures;
    std::map<const BiomeType*, sf::Texture> m_biome_tile_textures; // <-- NEW: For biome background colors
    std::map<AnimalType, sf::Texture> m_animal_textures;
    sf::Music m_background_music;

    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<UIManager> m_ui_manager;
};

#endif // GRAPHICS_RENDERER_H
