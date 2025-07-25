#include "graphics/GraphicsRenderer.h"
#include "resources/Resource.h"
#include "resources/Tile.h"
#include "resources/Terrain.h"
#include "core/EntityManager.h"
#include "resources/Biome.h"
#include <iostream>
#include <algorithm>
#include <cmath>

// Define texture file paths
const std::string ASSETS_PATH = "assets/";
const std::string EMPTY_TILE_TEXTURE_PATH = ASSETS_PATH + "textures/barren.png";
const std::string GRASS_TILE_TEXTURE_PATH = ASSETS_PATH + "textures/grass.png";
const std::string BERRY_TILE_TEXTURE_PATH = ASSETS_PATH + "textures/berry.png";
const std::string BUSH_TILE_TEXTURE_PATH = ASSETS_PATH + "textures/bush.png";

// --- Terrain Texture Paths ---
const std::string WATER_TILE_TEXTURE_PATH = ASSETS_PATH + "textures/water.png";
const std::string ROCKY_TILE_TEXTURE_PATH = ASSETS_PATH + "textures/rocky.png"; 

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
    
    // Load resource textures
    if (!m_resource_tile_textures[&RESOURCE_GRASS].loadFromFile(GRASS_TILE_TEXTURE_PATH)) {
         std::cerr << "Error loading grass texture: " << GRASS_TILE_TEXTURE_PATH << std::endl;
    }
    if (!m_resource_tile_textures[&RESOURCE_BERRIES].loadFromFile(BERRY_TILE_TEXTURE_PATH)) {
         std::cerr << "Error loading berry texture: " << BERRY_TILE_TEXTURE_PATH << std::endl;
    }
    if (!m_resource_tile_textures[&RESOURCE_BUSH].loadFromFile(BUSH_TILE_TEXTURE_PATH)) {
         std::cerr << "Error loading bush texture: " << BUSH_TILE_TEXTURE_PATH << std::endl;
    }
    
    // Load terrain textures
    if (!m_terrain_tile_textures[&TERRAIN_WATER].loadFromFile(WATER_TILE_TEXTURE_PATH)) {
         std::cerr << "Error loading water texture: " << WATER_TILE_TEXTURE_PATH << std::endl;
    }
    if (!m_terrain_tile_textures[&TERRAIN_ROCKY].loadFromFile(ROCKY_TILE_TEXTURE_PATH)) {
         std::cerr << "Error loading rocky texture: " << ROCKY_TILE_TEXTURE_PATH << std::endl;
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

void GraphicsRenderer::handleEvents(const EntityManager* entityManager) {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
            m_window.close();
        }
        
        m_camera->handleEvent(event, m_window, entityManager);
    }
}

void GraphicsRenderer::clear(const sf::Color& color) {
    m_window.clear(color);
}

void GraphicsRenderer::display() {
    m_window.display();
}

void GraphicsRenderer::update(float delta_time, const EntityManager* entityManager) {
    m_camera->updateFollowMode(entityManager);
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

            // 1. Draw the terrain texture as the base
            sf::Sprite terrain_sprite;
            const TerrainType* terrain = tile.getTerrain();
            
            if (terrain && terrain != &TERRAIN_NORMAL) {
                // Use specific terrain texture
                auto terrain_it = m_terrain_tile_textures.find(terrain);
                if (terrain_it != m_terrain_tile_textures.end()) {
                    terrain_sprite.setTexture(terrain_it->second);
                } else {
                    terrain_sprite.setTexture(m_empty_tile_texture); // Fallback
                }
            } else {
                // Use empty tile texture for normal terrain
                terrain_sprite.setTexture(m_empty_tile_texture);
            }
            
            terrain_sprite.setPosition(x * m_tile_size, y * m_tile_size);
            terrain_sprite.setScale(0.5f, 0.5f); // 40x40 texture to 20x20 tile
            m_window.draw(terrain_sprite);

            // 2. Draw the resource on top, if it exists
            if (tile.resource_type != nullptr && tile.resource_amount > 0.0f) {
                sf::Sprite resource_sprite;
                auto it = m_resource_tile_textures.find(tile.resource_type);
                if (it != m_resource_tile_textures.end()) {
                    resource_sprite.setTexture(it->second);
                    resource_sprite.setPosition(x * m_tile_size, y * m_tile_size);
                    resource_sprite.setScale(0.5f, 0.5f); // 40x40 texture to 20x20 tile
                    m_window.draw(resource_sprite);
                }
            }
        }
    }
}

void GraphicsRenderer::drawEntities(const EntityManager& entityManager, float animation_progress) {
    m_window.setView(m_camera->getView());

    // View frustum culling - only render entities in visible area
    sf::FloatRect visible_bounds = getVisibleBounds();

    // Easing function for smooth animation
    float eased_progress = 1.0f - (1.0f - animation_progress) * (1.0f - animation_progress);

    for (size_t i = 0; i < entityManager.getEntityCount(); ++i) {
        if (!entityManager.is_alive[i]) {
            continue;
        }

        // Interpolate position
        float prev_pixel_x = entityManager.prev_x[i] * m_tile_size;
        float prev_pixel_y = entityManager.prev_y[i] * m_tile_size;
        float current_pixel_x = entityManager.x[i] * m_tile_size;
        float current_pixel_y = entityManager.y[i] * m_tile_size;

        float interp_x = prev_pixel_x + (current_pixel_x - prev_pixel_x) * eased_progress;
        float interp_y = prev_pixel_y + (current_pixel_y - prev_pixel_y) * eased_progress;

        // Check if entity is within visible bounds
        if (interp_x + m_tile_size < visible_bounds.left || 
            interp_x > visible_bounds.left + visible_bounds.width ||
            interp_y + m_tile_size < visible_bounds.top || 
            interp_y > visible_bounds.top + visible_bounds.height) {
            continue; // Entity is outside visible area, skip rendering
        }

        AnimalType entity_type = entityManager.type[i];
        auto it = m_animal_textures.find(entity_type);
        if (it != m_animal_textures.end()) {
            sf::Sprite entity_sprite;
            entity_sprite.setTexture(it->second);
            entity_sprite.setPosition(interp_x, interp_y);

            // Optimized scaling: 64x64 animal texture to 20x20 tile
            entity_sprite.setScale(0.3125f, 0.3125f); // 20/64 = 0.3125
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

void GraphicsRenderer::drawSelectionIndicator(const EntityManager& entityManager, float animation_progress) {
    if (!m_camera->hasSelectedEntity()) {
        return;
    }
    
    size_t selected_id = m_camera->getSelectedEntity();
    
    // Validate that the selected entity still exists
    if (selected_id >= entityManager.getEntityCount() || !entityManager.is_alive[selected_id]) {
        // Entity no longer exists, clear selection
        m_camera->clearSelection();
        return;
    }
    
    m_window.setView(m_camera->getView());
    
    // Calculate interpolated position (same as in drawEntities)
    float eased_progress = 1.0f - (1.0f - animation_progress) * (1.0f - animation_progress);
    
    float prev_pixel_x = entityManager.prev_x[selected_id] * m_tile_size;
    float prev_pixel_y = entityManager.prev_y[selected_id] * m_tile_size;
    float current_pixel_x = entityManager.x[selected_id] * m_tile_size;
    float current_pixel_y = entityManager.y[selected_id] * m_tile_size;
    
    float interp_x = prev_pixel_x + (current_pixel_x - prev_pixel_x) * eased_progress;
    float interp_y = prev_pixel_y + (current_pixel_y - prev_pixel_y) * eased_progress;
    
    // Draw selection circle with pulsing effect
    sf::CircleShape selection_circle;
    float radius = m_tile_size * 0.8f; // Slightly larger than entity
    selection_circle.setRadius(radius);
    selection_circle.setOrigin(radius, radius); // Center the circle
    selection_circle.setPosition(interp_x + m_tile_size / 2.0f, interp_y + m_tile_size / 2.0f);
    
    // Create pulsing effect with time-based animation
    static sf::Clock pulse_clock;
    float pulse_time = pulse_clock.getElapsedTime().asSeconds();
    float pulse_factor = 0.8f + 0.2f * std::sin(pulse_time * 3.0f); // Pulse between 0.8 and 1.0
    
    // Yellow circle with pulsing opacity and thickness
    selection_circle.setFillColor(sf::Color::Transparent);
    selection_circle.setOutlineColor(sf::Color(255, 255, 0, static_cast<sf::Uint8>(200 * pulse_factor)));
    selection_circle.setOutlineThickness(3.0f * pulse_factor);
    
    m_window.draw(selection_circle);
    
    // Add inner highlight circle for better visibility
    sf::CircleShape inner_circle;
    float inner_radius = radius * 0.6f;
    inner_circle.setRadius(inner_radius);
    inner_circle.setOrigin(inner_radius, inner_radius);
    inner_circle.setPosition(interp_x + m_tile_size / 2.0f, interp_y + m_tile_size / 2.0f);
    inner_circle.setFillColor(sf::Color(255, 255, 0, static_cast<sf::Uint8>(60 * pulse_factor)));
    inner_circle.setOutlineColor(sf::Color::Transparent);
    
    m_window.draw(inner_circle);
}

void GraphicsRenderer::drawEntityDetailPanel(const EntityManager& entityManager) {
    m_ui_manager->drawEntityDetailPanel(m_window, entityManager, *m_camera);
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

Camera* GraphicsRenderer::getCamera() const {
    return m_camera.get();
}