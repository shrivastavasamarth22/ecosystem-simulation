#include "graphics/GraphicsRenderer.h"
#include "common/AnimalTypes.h"
#include "resources/Resource.h"
#include "resources/Tile.h"
#include "core/EntityManager.h"
#include <iostream>
#include <sstream>

// Define texture file paths
const std::string ASSETS_PATH = "assets/";
const std::string EMPTY_TILE_TEXTURE_PATH = ASSETS_PATH + "textures/empty_tile.png";
const std::string GRASS_TILE_TEXTURE_PATH = ASSETS_PATH + "textures/grass_tile.png";
const std::string BERRY_TILE_TEXTURE_PATH = ASSETS_PATH + "textures/berry_tile.png"; 

// --- Animal Texture Paths ---
const std::string HERBIVORE_TEXTURE_PATH = ASSETS_PATH + "textures/herbivore.png";
const std::string CARNIVORE_TEXTURE_PATH = ASSETS_PATH + "textures/carnivore.png";
const std::string OMNIVORE_TEXTURE_PATH = ASSETS_PATH + "textures/omnivore.png";

// --- Font File Path ---
const std::string FONT_PATH = ASSETS_PATH + "fonts/daydream.ttf";

// Backround music file path
const std::string BACKGROUND_MUSIC_PATH = ASSETS_PATH + "audio/background_music.mp3";

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
    while (m_window.pollEvent(event)) {
        // --- Basic window closing ---
        if (event.type == sf::Event::Closed) {
            m_window.close();
        }

        // --- NEW: Handle Keyboard Input ---
        if (event.type == sf::Event::KeyPressed) {
            // Example: Close window on Escape key
            if (event.key.code == sf::Keyboard::Escape) {
                m_window.close();
            }
            // We will handle pause/unpause in main.cpp as it controls the simulation state.
        }
    }
}

void GraphicsRenderer::clear(const sf::Color& color) {
    m_window.clear(color); // Clear the window with the specified color
}

void GraphicsRenderer::display() {
    m_window.display(); // Display what has been drawn to the window
}

// --- drawWorld Implementation ---
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

            // Scale the sprite if the texture size doesn't match tile_size
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

void GraphicsRenderer::drawUI(const World& world, bool is_paused) {
    // --- NEW: Set UI View ---
    // Create a view that maps directly to the current window dimensions (pixel coordinates)
    sf::View ui_view(sf::FloatRect(0, 0, m_window.getSize().x, m_window.getSize().y));
    m_window.setView(ui_view); // Apply this view for drawing UI

    if (m_font.getInfo().family.empty()) {
        // Optionally restore the previous view here if we had one
        // m_window.setView(m_world_view); // e.g.
        return;
    }    const EntityManager& data = world.getEntityManager();
    size_t num_entities = data.getEntityCount();
    int herbivore_count = 0; int carnivore_count = 0; int omnivore_count = 0;
    for (size_t i = 0; i < num_entities; ++i) { if (data.is_alive[i]) { switch (data.type[i]) { case AnimalType::HERBIVORE: herbivore_count++; break; case AnimalType::CARNIVORE: carnivore_count++; break; case AnimalType::OMNIVORE:  omnivore_count++; break; default: break; } } }

    // Calculate total living entities
    int total_living_entities = herbivore_count + carnivore_count + omnivore_count;

    // Create turn counter text
    std::stringstream turn_ss;
    turn_ss << "Turn: " << world.getTurnCount() << " | Total: " << total_living_entities;
    
    sf::Text turn_text;
    turn_text.setFont(m_font);
    turn_text.setString(turn_ss.str());
    turn_text.setCharacterSize(12);
    turn_text.setFillColor(sf::Color::White);
    turn_text.setPosition(10, 10);

    // Add background for turn text
    sf::FloatRect turn_bounds = turn_text.getGlobalBounds();
    sf::RectangleShape turn_background;
    turn_background.setSize(sf::Vector2f(turn_bounds.width + 20, turn_bounds.height + 10));
    turn_background.setFillColor(sf::Color(0, 0, 0, 128));
    turn_background.setPosition(5, 5);
    
    m_window.draw(turn_background);
    m_window.draw(turn_text);    // Draw animal sprites with counts
    float y_offset = 40; // Start below the turn counter
    float sprite_size = 24; // Size for UI sprites (increased from 20)
    float spacing = 32; // Spacing between entries (increased slightly)

    // Helper function to draw animal stat with sprite
    auto drawAnimalStat = [&](AnimalType type, int count, float y_pos) {
        auto it = m_animal_textures.find(type);
        if (it != m_animal_textures.end()) {
            // Create sprite
            sf::Sprite animal_sprite;
            animal_sprite.setTexture(it->second);
            animal_sprite.setPosition(10, y_pos);
            
            // Scale sprite to UI size
            sf::Vector2u texture_size = animal_sprite.getTexture()->getSize();
            animal_sprite.setScale(
                sprite_size / texture_size.x,
                sprite_size / texture_size.y
            );            // Create count text
            sf::Text count_text;
            count_text.setFont(m_font);
            count_text.setString(std::to_string(count));
            count_text.setCharacterSize(14);
            count_text.setFillColor(sf::Color::White);
            count_text.setPosition(40, y_pos + 4); // Position next to sprite, adjusted for centering

            // Create background for this stat
            sf::FloatRect stat_bounds = count_text.getGlobalBounds();
            sf::RectangleShape stat_background;
            stat_background.setSize(sf::Vector2f(stat_bounds.width + sprite_size + 20, sprite_size + 6));
            stat_background.setFillColor(sf::Color(0, 0, 0, 128));
            stat_background.setPosition(5, y_pos - 3);

            // Draw background, sprite, and text
            m_window.draw(stat_background);
            m_window.draw(animal_sprite);
            m_window.draw(count_text);
        }
    };

    // Draw each animal type with its count
    drawAnimalStat(AnimalType::HERBIVORE, herbivore_count, y_offset);
    drawAnimalStat(AnimalType::CARNIVORE, carnivore_count, y_offset + spacing);
    drawAnimalStat(AnimalType::OMNIVORE, omnivore_count, y_offset + spacing * 2);    // Draw Pause Indicator
    if (is_paused) {
        sf::Text pause_text;
        pause_text.setFont(m_font);
        pause_text.setString("PAUSED");
        pause_text.setCharacterSize(18);
        pause_text.setFillColor(sf::Color::Yellow);

        // Position pause text relative to the UI view's top-right
        // Get the bounds AFTER setting string and size to get correct width
        sf::FloatRect pause_bounds = pause_text.getGlobalBounds();
        pause_text.setPosition(m_window.getSize().x - pause_bounds.width - 10, 10); // 10 pixels from right edge, 10 from top
        
        // Add background for pause text
        sf::RectangleShape pause_background;
        pause_background.setSize(sf::Vector2f(pause_bounds.width + 20, pause_bounds.height + 10));
        pause_background.setFillColor(sf::Color(0, 0, 0, 128));
        pause_background.setPosition(m_window.getSize().x - pause_bounds.width - 20, 5);
        
        m_window.draw(pause_background);
        m_window.draw(pause_text);
    }

    // --- Restore Previous View ---
    // After drawing UI, restore the view that was active before this function was called.
    m_window.setView(m_window.getDefaultView());
}

void GraphicsRenderer::drawSimulationEndedMessage() {
    // Set UI view for drawing centered text
    sf::View ui_view(sf::FloatRect(0, 0, m_window.getSize().x, m_window.getSize().y));
    m_window.setView(ui_view);

    if (m_font.getInfo().family.empty()) {
        m_window.setView(m_window.getDefaultView());
        return;
    }

    // Create large "Simulation Ended" text
    sf::Text end_text;
    end_text.setFont(m_font);
    end_text.setString("Simulation Ended");
    end_text.setCharacterSize(48);
    end_text.setFillColor(sf::Color::Red);
    end_text.setStyle(sf::Text::Bold);

    // Center the text on screen
    sf::FloatRect text_bounds = end_text.getLocalBounds();
    end_text.setOrigin(text_bounds.left + text_bounds.width / 2.0f, 
                       text_bounds.top + text_bounds.height / 2.0f);
    end_text.setPosition(m_window.getSize().x / 2.0f, m_window.getSize().y / 2.0f);

    // Add semi-transparent background for better visibility
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(text_bounds.width + 40, text_bounds.height + 20));
    background.setFillColor(sf::Color(0, 0, 0, 128)); // Semi-transparent black
    background.setOrigin(background.getSize().x / 2.0f, background.getSize().y / 2.0f);
    background.setPosition(m_window.getSize().x / 2.0f, m_window.getSize().y / 2.0f);

    // Draw background then text
    m_window.draw(background);
    m_window.draw(end_text);

    // Restore default view
    m_window.setView(m_window.getDefaultView());
}