#include "graphics/GraphicsRenderer.h"
#include "resources/Resource.h"
#include "resources/Tile.h"
#include "core/EntityManager.h"
#include "resources/Biome.h"
#include <iostream>
#include <algorithm>

// Define texture file paths
const std::string ASSETS_PATH = "assets/";
const std::string EMPTY_TILE_TEXTURE_PATH = ASSETS_PATH + "textures/empty_tile.png";
const std::string GRASS_TILE_TEXTURE_PATH = ASSETS_PATH + "textures/grass_tile.png";
const std::string BERRY_TILE_TEXTURE_PATH = ASSETS_PATH + "textures/berry_tile.png"; 

// --- Animal Texture Paths ---
const std::string HERBIVORE_TEXTURE_PATH = ASSETS_PATH + "textures/herbivore.png";
const std::string CARNIVORE_TEXTURE_PATH = ASSETS_PATH + "textures/carnivore.png";
const std::string OMNIVORE_TEXTURE_PATH = ASSETS_PATH + "textures/omnivore.png";

// Backround music file path
const std::string BACKGROUND_MUSIC_PATH = ASSETS_PATH + "audio/background_music.mp3";

GraphicsRenderer::GraphicsRenderer() : m_window(), m_tile_size(0) {}

GraphicsRenderer::~GraphicsRenderer() {}

void GraphicsRenderer::init(unsigned int window_width, unsigned int window_height, int world_width, int world_height, int tile_size, const std::string& title) {
    m_tile_size = tile_size;

    m_window.create(sf::VideoMode(window_width, window_height), title);
    m_window.setVerticalSyncEnabled(true);

    // --- Load World Textures ---
    if (!m_empty_tile_texture.loadFromFile(EMPTY_TILE_TEXTURE_PATH)) {
        std::cerr << "Error loading empty tile texture: " << EMPTY_TILE_TEXTURE_PATH << std::endl;
    }
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

    // --- Load and Play Background Music ---
    if (!m_background_music.openFromFile(BACKGROUND_MUSIC_PATH)) {
        std::cerr << "Error loading background music: " << BACKGROUND_MUSIC_PATH << std::endl;
    } else {
        m_background_music.setLoop(true);
        m_background_music.play();
    }

    // --- Initialize Camera and UI Manager ---
    m_camera = std::make_unique<Camera>(window_width, window_height, world_width, world_height, tile_size);
    m_ui_manager = std::make_unique<UIManager>();
    if (!m_ui_manager->loadAssets()) {
        std::cerr << "Failed to load UI assets." << std::endl;
    }
    
    m_window.setMouseCursorVisible(false);
}

bool GraphicsRenderer::isOpen() const {
    return m_window.isOpen();
}

void GraphicsRenderer::handleEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
            m_window.close();
        }
        
        m_camera->handleEvent(event, m_window);
    }
}

void GraphicsRenderer::clear(const sf::Color& color) {
    m_window.clear(color);
}

void GraphicsRenderer::display() {
    m_window.display();
}

void GraphicsRenderer::update(float delta_time) {
    m_camera->update(delta_time);
}

void GraphicsRenderer::drawWorld(const World& world) {
    m_window.setView(m_camera->getView());

    // View frustum culling - only render visible tiles
    sf::FloatRect visible_bounds = getVisibleBounds();
    
    // Calculate tile range that intersects with visible area
    int start_x = std::max(0, static_cast<int>(visible_bounds.left / m_tile_size) - 1);
    int end_x = std::min(world.getWidth() - 1, static_cast<int>((visible_bounds.left + visible_bounds.width) / m_tile_size) + 1);
    int start_y = std::max(0, static_cast<int>(visible_bounds.top / m_tile_size) - 1);
    int end_y = std::min(world.getHeight() - 1, static_cast<int>((visible_bounds.top + visible_bounds.height) / m_tile_size) + 1);

    // Only iterate through visible tiles
    for (int y = start_y; y <= end_y; ++y) {
        for (int x = start_x; x <= end_x; ++x) {
            const Tile& tile = world.getTile(x, y);

            // 1. Draw the base empty tile texture for the grid background
            sf::Sprite background_sprite;
            background_sprite.setTexture(m_empty_tile_texture);
            background_sprite.setPosition(x * m_tile_size, y * m_tile_size);
            sf::Vector2u texture_size_bg = background_sprite.getTexture()->getSize();
            background_sprite.setScale(
                static_cast<float>(m_tile_size) / texture_size_bg.x,
                static_cast<float>(m_tile_size) / texture_size_bg.y
            );
            m_window.draw(background_sprite);

            // 2. Draw the resource on top, if it exists
            if (tile.resource_type != nullptr && tile.resource_amount > 0.0f) {
                sf::Sprite resource_sprite;
                auto it = m_resource_tile_textures.find(tile.resource_type);
                if (it != m_resource_tile_textures.end()) {
                    resource_sprite.setTexture(it->second);
                    resource_sprite.setPosition(x * m_tile_size, y * m_tile_size);
                    sf::Vector2u texture_size = resource_sprite.getTexture()->getSize();
                    resource_sprite.setScale(
                        static_cast<float>(m_tile_size) / texture_size.x,
                        static_cast<float>(m_tile_size) / texture_size.y
                    );
                    m_window.draw(resource_sprite);
                }
            }
        }
    }
}

void GraphicsRenderer::drawEntities(const EntityManager& entityManager) {
    m_window.setView(m_camera->getView());

    // View frustum culling - only render entities in visible area
    sf::FloatRect visible_bounds = getVisibleBounds();

    for (size_t i = 0; i < entityManager.getEntityCount(); ++i) {
        if (!entityManager.is_alive[i]) {
            continue;
        }

        // Check if entity is within visible bounds
        float entity_pixel_x = entityManager.x[i] * m_tile_size;
        float entity_pixel_y = entityManager.y[i] * m_tile_size;
        
        if (entity_pixel_x + m_tile_size < visible_bounds.left || 
            entity_pixel_x > visible_bounds.left + visible_bounds.width ||
            entity_pixel_y + m_tile_size < visible_bounds.top || 
            entity_pixel_y > visible_bounds.top + visible_bounds.height) {
            continue; // Entity is outside visible area, skip rendering
        }

        AnimalType entity_type = entityManager.type[i];
        auto it = m_animal_textures.find(entity_type);
        if (it != m_animal_textures.end()) {
            sf::Sprite entity_sprite;
            entity_sprite.setTexture(it->second);
            entity_sprite.setPosition(entity_pixel_x, entity_pixel_y);

            sf::Vector2u texture_size = entity_sprite.getTexture()->getSize();
            entity_sprite.setScale(
                static_cast<float>(m_tile_size) / texture_size.x,
                static_cast<float>(m_tile_size) / texture_size.y
            );
            m_window.draw(entity_sprite);
        }
    }
}

void GraphicsRenderer::drawUI(const World& world, bool is_paused) {
    m_ui_manager->drawUI(m_window, world, is_paused);
}

void GraphicsRenderer::drawSimulationEndedMessage() {
    m_ui_manager->drawSimulationEndedMessage(m_window);
}

void GraphicsRenderer::drawCursor() {
    m_ui_manager->drawCursor(m_window);
}

sf::FloatRect GraphicsRenderer::getVisibleBounds() const {
    sf::View current_view = m_camera->getView();
    sf::Vector2f center = current_view.getCenter();
    sf::Vector2f size = current_view.getSize();
    
    return sf::FloatRect(
        center.x - size.x / 2.0f,
        center.y - size.y / 2.0f,
        size.x,
        size.y
    );
}