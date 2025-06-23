#include "includes/GraphicsRenderer.h"
#include "includes/AnimalTypes.h"
#include "includes/Resource.h"
#include "includes/Tile.h"
#include "includes/EntityManager.h"
#include <iostream>
#include <sstream>

// Define texture file paths
const std::string ASSETS_PATH = "assets/";
const std::string EMPTY_TILE_TEXTURE_PATH = ASSETS_PATH + "empty_tile.png";
const std::string GRASS_TILE_TEXTURE_PATH = ASSETS_PATH + "grass_tile.png";
const std::string BERRY_TILE_TEXTURE_PATH = ASSETS_PATH + "berry_tile.png"; 

// --- Animal Texture Paths ---
const std::string HERBIVORE_TEXTURE_PATH = ASSETS_PATH + "herbivore.png";
const std::string CARNIVORE_TEXTURE_PATH = ASSETS_PATH + "carnivore.png";
const std::string OMNIVORE_TEXTURE_PATH = ASSETS_PATH + "omnivore.png";

// --- Font File Path ---
const std::string FONT_PATH = ASSETS_PATH + "daydream.ttf";

// Backround music file path
const std::string BACKGROUND_MUSIC_PATH = ASSETS_PATH + "background_music.mp3";

GraphicsRenderer::GraphicsRenderer() : m_window(), m_tile_size(0), m_background_music() {
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


    // --- Load Textures ---
    if (!m_empty_tile_texture.loadFromFile(EMPTY_TILE_TEXTURE_PATH)) {
        std::cerr << "Error loading empty tile texture: " << EMPTY_TILE_TEXTURE_PATH << std::endl;
    }
    // Ensure RESOURCE_GRASS and RESOURCE_BERRIES are defined in Resource.cpp and externed in Resource.h
    if (!m_resource_tile_textures[&RESOURCE_GRASS].loadFromFile(GRASS_TILE_TEXTURE_PATH)) {
         std::cerr << "Error loading grass texture: " << GRASS_TILE_TEXTURE_PATH << std::endl;
    }
    if (!m_resource_tile_textures[&RESOURCE_BERRIES].loadFromFile(BERRY_TILE_TEXTURE_PATH)) {
         std::cerr << "Error loading berry texture: " << BERRY_TILE_TEXTURE_PATH << std::endl;
    }

    // --- Load Animal Textures ---
    if (!m_animal_textures[AnimalType::HERBIVORE].loadFromFile(HERBIVORE_TEXTURE_PATH)) {
        std::cerr << "Error loading herbivore texture: " << HERBIVORE_TEXTURE_PATH << std::endl;
    }
    if (!m_animal_textures[AnimalType::CARNIVORE].loadFromFile(CARNIVORE_TEXTURE_PATH)) {
        std::cerr << "Error loading carnivore texture: " << CARNIVORE_TEXTURE_PATH << std::endl;
    }
    if (!m_animal_textures[AnimalType::OMNIVORE].loadFromFile(OMNIVORE_TEXTURE_PATH)) {
        std::cerr << "Error loading omnivore texture: " << OMNIVORE_TEXTURE_PATH << std::endl;
    }

     if (!m_font.loadFromFile(FONT_PATH)) {
        std::cerr << "Error loading font: " << FONT_PATH << std::endl;
        // The program can still run without a font, text won't be displayed.
    }

    // --- Load and Play Background Music ---
    // Open the music file
    if (!m_background_music.openFromFile(BACKGROUND_MUSIC_PATH)) {
        std::cerr << "Error loading background music: " << BACKGROUND_MUSIC_PATH << std::endl;
        // Handle error: Music will just not play.
    } else {
        // Set the music to loop
        m_background_music.setLoop(true);
        // Start playing the music
        m_background_music.play();
    }
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

// --- drawEntities Implementation ---
void GraphicsRenderer::drawEntities(const EntityManager& entityManager) {
    size_t num_entities = entityManager.getEntityCount();

    for (size_t i = 0; i < num_entities; ++i) {
        // --- This check prevents drawing dead entities ---
        if (!entityManager.is_alive[i]) {
                continue; // Skip to the next entity if this one is not alive
        }

        AnimalType entity_type = entityManager.type[i];

        // Find the correct animal texture
        auto it = m_animal_textures.find(entity_type);
        if (it != m_animal_textures.end()) {
            sf::Sprite entity_sprite;
            entity_sprite.setTexture(it->second);

            // Set position based on entity's world coordinates
            entity_sprite.setPosition(entityManager.x[i] * m_tile_size, entityManager.y[i] * m_tile_size);

            // Apply scaling
            sf::Vector2u texture_size = entity_sprite.getTexture()->getSize();
            entity_sprite.setScale(
                static_cast<float>(m_tile_size) / texture_size.x,
                static_cast<float>(m_tile_size) / texture_size.y
            );

            // Draw the entity sprite
            m_window.draw(entity_sprite);

        } else {
            std::cerr << "Warning: No texture mapped for animal type: " << static_cast<int>(entity_type) << std::endl;
        }
    }
}

void GraphicsRenderer::drawUI(const World& world) {
    // Check if font is loaded successfully
    if (m_font.getInfo().family.empty()) {
        return; // Exit if font is not loaded
    }

    // Get counts from World/EntityManager
    const EntityManager& data = world.getEntityManager();
    size_t num_entities = data.getEntityCount();

    int herbivore_count = 0;
    int carnivore_count = 0;
    int omnivore_count = 0;

    // Iterate through entities to count living ones
    for (size_t i = 0; i < num_entities; ++i) {
        if (data.is_alive[i]) {
            switch (data.type[i]) {
                case AnimalType::HERBIVORE: herbivore_count++; break;
                case AnimalType::CARNIVORE: carnivore_count++; break;
                case AnimalType::OMNIVORE:  omnivore_count++; break;
                default: break;
            }
        }
    }

    // Create text string using stringstream
    std::stringstream ss;
    ss << "Turn: " << world.getTurnCount()
    << " H: " << herbivore_count
    << " C: " << carnivore_count
    << " O: " << omnivore_count
    << " Total: " << num_entities; // Total entities in the manager (living + dead before cleanup)

    // Create SFML Text object
    sf::Text stats_text;
    stats_text.setFont(m_font); // Set the font
    stats_text.setString(ss.str()); // Set the text content
    stats_text.setCharacterSize(8); // Set the character size (in pixels)
    stats_text.setFillColor(sf::Color::White); // Set the color
    stats_text.setPosition(10, 10); // Set the position (e.g., top-left corner)

    // Draw the text
    m_window.draw(stats_text);
}