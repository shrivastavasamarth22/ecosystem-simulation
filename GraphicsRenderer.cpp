#include "includes/GraphicsRenderer.h"
#include "includes/AnimalTypes.h"
#include "includes/Resource.h"
#include "includes/Tile.h"
#include "includes/EntityManager.h"
#include <iostream>

// Define texture file paths
const std::string ASSETS_PATH = "assets/";
const std::string EMPTY_TILE_TEXTURE_PATH = ASSETS_PATH + "empty_tile.png";
const std::string GRASS_TILE_TEXTURE_PATH = ASSETS_PATH + "grass_tile.png";
const std::string BERRY_TILE_TEXTURE_PATH = ASSETS_PATH + "berry_tile.png"; 

GraphicsRenderer::GraphicsRenderer() : m_window(), m_tile_size(0) {
    // Constructor doesn't create the window yet, init does.
}

GraphicsRenderer::~GraphicsRenderer() {
    // Destructor is called when the object goes out of scope,
    // automatically cleans up SFML resources (like the window).
}

void GraphicsRenderer::init(int world_width, int world_height, int tile_size, const std::string& title) {
    m_tile_size = tile_size;
    unsigned int window_width = world_width * m_tile_size;
    unsigned int window_height = world_height * m_tile_size;

    m_window.create(sf::VideoMode(window_width, window_height), title);
    m_window.setVerticalSyncEnabled(true);

    // --- NEW: Load Textures ---
    // Load the empty tile texture
    if (!m_empty_tile_texture.loadFromFile(EMPTY_TILE_TEXTURE_PATH)) {
        // Handle error - e.g., print to console, throw exception
        std::cerr << "Error loading empty tile texture: " << EMPTY_TILE_TEXTURE_PATH << std::endl;
        // Keep running with default white texture, or exit? Let's continue.
    }

    // Load resource tile textures and map them to ResourceType pointers
    // Note: Ensure the texture files exist and paths are correct.
    // Note: Need to access the global RESOURCE_GRASS and RESOURCE_BERRIES defined in Resource.cpp.
    //      They should be externed in Resource.h. We already did that.

    if (!m_resource_tile_textures[&RESOURCE_GRASS].loadFromFile(GRASS_TILE_TEXTURE_PATH)) {
         std::cerr << "Error loading grass texture: " << GRASS_TILE_TEXTURE_PATH << std::endl;
    }
     if (!m_resource_tile_textures[&RESOURCE_BERRIES].loadFromFile(BERRY_TILE_TEXTURE_PATH)) {
         std::cerr << "Error loading berry texture: " << BERRY_TILE_TEXTURE_PATH << std::endl;
    }

    // Texture loading for animals and font will go here in later phases.
}

bool GraphicsRenderer::isOpen() const {
    return m_window.isOpen();
}

void GraphicsRenderer::handleEvents() {
    sf::Event event;
    // Process events in a loop
    while (m_window.pollEvent(event)) {
        // Close window: exit
        if (event.type == sf::Event::Closed)
            m_window.close();

        // Handle keyboard input here later (Phase 6)
        // if (event.type == sf::Event::KeyPressed) { ... }
    }
}

void GraphicsRenderer::clear(const sf::Color& color) {
    m_window.clear(color); // Clear the window with the specified color
}

void GraphicsRenderer::display() {
    m_window.display(); // Display what has been drawn to the window
}

// --- NEW: drawWorld Implementation ---
    void GraphicsRenderer::drawWorld(const World& world) {
    // Iterate through each tile in the world grid
    for (int y = 0; y < world.getHeight(); ++y) {
        for (int x = 0; x < world.getWidth(); ++x) {
            const Tile& tile = world.getTile(x, y);
            sf::Sprite tile_sprite;

            // Determine which texture to use
            if (tile.resource_type == nullptr || tile.resource_amount <= 0) {
                // Use the empty tile texture
                tile_sprite.setTexture(m_empty_tile_texture);
            } else {
                // Use the texture mapped to the specific resource type
                // Find the texture in the map
                auto it = m_resource_tile_textures.find(tile.resource_type);
                if (it != m_resource_tile_textures.end()) {
                    tile_sprite.setTexture(it->second);
                } else {
                    // Fallback: use empty texture if resource type not mapped
                    tile_sprite.setTexture(m_empty_tile_texture);
                        std::cerr << "Warning: No texture mapped for resource type: " << tile.resource_type->name << std::endl;
                }
            }

            // Set the position of the sprite (world grid coords -> screen pixel coords)
            tile_sprite.setPosition(x * m_tile_size, y * m_tile_size);

            // Optionally scale the sprite if the texture size doesn't match tile_size
            sf::Vector2u texture_size = tile_sprite.getTexture()->getSize();
            tile_sprite.setScale(
                static_cast<float>(m_tile_size) / texture_size.x,
                static_cast<float>(m_tile_size) / texture_size.y
            );

            // Draw the sprite to the window
            m_window.draw(tile_sprite);
        }
    }
    }