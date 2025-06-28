#include "graphics/UIManager.h"
#include <iostream>
#include <sstream>

const std::string ASSETS_PATH_UI = "assets/";
const std::string FONT_PATH_UI = ASSETS_PATH_UI + "fonts/daydream.ttf";
const std::string CURSOR_TEXTURE_PATH_UI = ASSETS_PATH_UI + "textures/cursor.png";
const std::string HERBIVORE_TEXTURE_PATH_UI = ASSETS_PATH_UI + "textures/herbivore.png";
const std::string CARNIVORE_TEXTURE_PATH_UI = ASSETS_PATH_UI + "textures/carnivore.png";
const std::string OMNIVORE_TEXTURE_PATH_UI = ASSETS_PATH_UI + "textures/omnivore.png";

UIManager::UIManager() {}

bool UIManager::loadAssets() {
    if (!m_font.loadFromFile(FONT_PATH_UI)) {
        std::cerr << "Error loading font: " << FONT_PATH_UI << std::endl;
        return false;
    }

    if (!m_cursor_texture.loadFromFile(CURSOR_TEXTURE_PATH_UI)) {
        std::cerr << "Error loading cursor texture: " << CURSOR_TEXTURE_PATH_UI << std::endl;
        return false;
    }
    m_cursor_sprite.setTexture(m_cursor_texture);
    m_cursor_sprite.setScale(0.5f, 0.5f);

    if (!m_animal_textures[AnimalType::HERBIVORE].loadFromFile(HERBIVORE_TEXTURE_PATH_UI)) {
        std::cerr << "Error loading herbivore texture: " << HERBIVORE_TEXTURE_PATH_UI << std::endl;
        return false;
    }
    if (!m_animal_textures[AnimalType::CARNIVORE].loadFromFile(CARNIVORE_TEXTURE_PATH_UI)) {
        std::cerr << "Error loading carnivore texture: " << CARNIVORE_TEXTURE_PATH_UI << std::endl;
        return false;
    }
    if (!m_animal_textures[AnimalType::OMNIVORE].loadFromFile(OMNIVORE_TEXTURE_PATH_UI)) {
        std::cerr << "Error loading omnivore texture: " << OMNIVORE_TEXTURE_PATH_UI << std::endl;
        return false;
    }

    return true;
}

void UIManager::drawUI(sf::RenderWindow& window, const World& world, bool is_paused) {
    sf::View ui_view(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
    window.setView(ui_view);

    if (m_font.getInfo().family.empty()) {
        return;
    }

    const EntityManager& data = world.getEntityManager();
    size_t num_entities = data.getEntityCount();
    int herbivore_count = 0; int carnivore_count = 0; int omnivore_count = 0;
    for (size_t i = 0; i < num_entities; ++i) { if (data.is_alive[i]) { switch (data.type[i]) { case AnimalType::HERBIVORE: herbivore_count++; break; case AnimalType::CARNIVORE: carnivore_count++; break; case AnimalType::OMNIVORE:  omnivore_count++; break; default: break; } } }

    int total_living_entities = herbivore_count + carnivore_count + omnivore_count;

    std::stringstream turn_ss;
    turn_ss << "Turn: " << world.getTurnCount() << " Total: " << total_living_entities;
    
    sf::Text turn_text;
    turn_text.setFont(m_font);
    turn_text.setString(turn_ss.str());
    turn_text.setCharacterSize(12);
    turn_text.setFillColor(sf::Color::White);
    turn_text.setPosition(10, 10);

    sf::FloatRect turn_bounds = turn_text.getGlobalBounds();
    sf::RectangleShape turn_background;
    turn_background.setSize(sf::Vector2f(turn_bounds.width + 20, turn_bounds.height + 10));
    turn_background.setFillColor(sf::Color(0, 0, 0, 128));
    turn_background.setPosition(5, 5);
    
    window.draw(turn_background);
    window.draw(turn_text);

    float y_offset = 40;
    float sprite_size = 24;
    float spacing = 32;

    auto drawAnimalStat = [&](AnimalType type, int count, float y_pos) {
        auto it = m_animal_textures.find(type);
        if (it != m_animal_textures.end()) {
            sf::Sprite animal_sprite;
            animal_sprite.setTexture(it->second);
            animal_sprite.setPosition(10, y_pos);
            
            // Optimized scaling: 64x64 animal texture to 24px UI sprite
            animal_sprite.setScale(0.375f, 0.375f); // 24/64 = 0.375

            sf::Text count_text;
            count_text.setFont(m_font);
            count_text.setString(std::to_string(count));
            count_text.setCharacterSize(14);
            count_text.setFillColor(sf::Color::White);
            count_text.setPosition(40, y_pos + 4);

            sf::FloatRect stat_bounds = count_text.getGlobalBounds();
            sf::RectangleShape stat_background;
            stat_background.setSize(sf::Vector2f(stat_bounds.width + sprite_size + 20, sprite_size + 6));
            stat_background.setFillColor(sf::Color(0, 0, 0, 128));
            stat_background.setPosition(5, y_pos - 3);

            window.draw(stat_background);
            window.draw(animal_sprite);
            window.draw(count_text);
        }
    };

    drawAnimalStat(AnimalType::HERBIVORE, herbivore_count, y_offset);
    drawAnimalStat(AnimalType::CARNIVORE, carnivore_count, y_offset + spacing);
    drawAnimalStat(AnimalType::OMNIVORE, omnivore_count, y_offset + spacing * 2);

    if (is_paused) {
        sf::Text pause_text;
        pause_text.setFont(m_font);
        pause_text.setString("PAUSED");
        pause_text.setCharacterSize(18);
        pause_text.setFillColor(sf::Color::Yellow);

        // Use getLocalBounds for more reliable measurements
        sf::FloatRect text_bounds = pause_text.getLocalBounds();

        // Set origin to the top-right of the text's content for easy alignment
        pause_text.setOrigin(text_bounds.left + text_bounds.width, text_bounds.top);
        
        // Position the text's origin at the top-right of the screen with padding
        pause_text.setPosition(sf::Vector2f(window.getSize().x - 15, 10));

        // Add a background for better readability
        sf::RectangleShape pause_background;
        pause_background.setSize(sf::Vector2f(text_bounds.width + 20, text_bounds.height + 12));
        pause_background.setFillColor(sf::Color(0, 0, 0, 128));
        
        // Set the background's origin to its own top-right corner
        pause_background.setOrigin(pause_background.getSize().x, 0);
        
        // Position the background to align correctly with the text
        pause_background.setPosition(sf::Vector2f(window.getSize().x - 5, 5));
        
        window.draw(pause_background);
        window.draw(pause_text);
    }
}

void UIManager::drawSimulationEndedMessage(sf::RenderWindow& window) {
    sf::View ui_view(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
    window.setView(ui_view);

    if (m_font.getInfo().family.empty()) {
        return;
    }

    sf::Text end_text;
    end_text.setFont(m_font);
    end_text.setString("Simulation Ended");
    end_text.setCharacterSize(48);
    end_text.setFillColor(sf::Color::Red);
    end_text.setStyle(sf::Text::Bold);

    sf::FloatRect text_bounds = end_text.getLocalBounds();
    end_text.setOrigin(text_bounds.left + text_bounds.width / 2.0f, 
                       text_bounds.top + text_bounds.height / 2.0f);
    end_text.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

    sf::RectangleShape background;
    background.setSize(sf::Vector2f(text_bounds.width + 40, text_bounds.height + 20));
    background.setFillColor(sf::Color(0, 0, 0, 128));
    background.setOrigin(background.getSize().x / 2.0f, background.getSize().y / 2.0f);
    background.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

    window.draw(background);
    window.draw(end_text);
}

void UIManager::drawCursor(sf::RenderWindow& window) {
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
    m_cursor_sprite.setPosition(static_cast<sf::Vector2f>(mouse_pos));
    window.draw(m_cursor_sprite);
}

void UIManager::drawEntityDetailPanel(sf::RenderWindow& window, const EntityManager& entityManager, const Camera& camera) {
    if (!camera.hasSelectedEntity()) {
        return;
    }
    
    size_t selected_id = camera.getSelectedEntity();
    
    // Validate entity exists
    if (selected_id >= entityManager.getEntityCount() || !entityManager.is_alive[selected_id]) {
        return;
    }
    
    // Set UI view
    sf::View ui_view(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
    window.setView(ui_view);
    
    // Panel dimensions and position (right side of screen, moved left)
    float panel_width = 450.0f;  // Increased width
    float panel_height = 750.0f; // Extended height for more spacing
    float panel_x = window.getSize().x - panel_width - 100.0f; // Moved further left (was 20.0f)
    float panel_y = 20.0f;
    
    // Draw semi-transparent background panel
    sf::RectangleShape panel_bg;
    panel_bg.setSize(sf::Vector2f(panel_width, panel_height));
    panel_bg.setPosition(panel_x, panel_y);
    panel_bg.setFillColor(sf::Color(0, 0, 0, 180)); // Semi-transparent black
    panel_bg.setOutlineColor(sf::Color(100, 100, 100, 200));
    panel_bg.setOutlineThickness(2.0f);
    window.draw(panel_bg);
    
    // Title
    sf::Text title_text;
    title_text.setFont(m_font);
    title_text.setString("Entity Details");
    title_text.setCharacterSize(24); // Larger title
    title_text.setFillColor(sf::Color::White);
    title_text.setPosition(panel_x + 15, panel_y + 15);
    window.draw(title_text);
    
    // Entity information
    float text_y = panel_y + 60;
    float line_spacing = 30; // Increased spacing
    float section_spacing = 20; // Additional spacing between sections
    
    // Helper lambda for drawing info lines
    auto drawInfoLine = [&](const std::string& label, const std::string& value) {
        sf::Text info_text;
        info_text.setFont(m_font);
        info_text.setString(label + ": " + value);
        info_text.setCharacterSize(16); // Larger text
        info_text.setFillColor(sf::Color::White);
        info_text.setPosition(panel_x + 15, text_y);
        window.draw(info_text);
        text_y += line_spacing;
    };
    
    // Helper lambda for drawing section headers
    auto drawSectionHeader = [&](const std::string& header) {
        text_y += section_spacing; // Add spacing before each section
        sf::Text header_text;
        header_text.setFont(m_font);
        header_text.setString("--- " + header + " ---");
        header_text.setCharacterSize(18);
        header_text.setFillColor(sf::Color(200, 200, 100)); // Light yellow
        header_text.setPosition(panel_x + 15, text_y);
        window.draw(header_text);
        text_y += line_spacing;
    };
    
    // Basic Info
    drawSectionHeader("Basic Info");
    drawInfoLine("Type", getAnimalTypeString(entityManager.type[selected_id]));
    drawInfoLine("Position", "(" + std::to_string(entityManager.x[selected_id]) + ", " + std::to_string(entityManager.y[selected_id]) + ")");
    drawInfoLine("Age", std::to_string(entityManager.age[selected_id]) + " turns (" + getAgeStage(entityManager.age[selected_id], entityManager.prime_age[selected_id]) + ")");
    
    // Health & Energy
    drawSectionHeader("Health & Energy");
    drawInfoLine("Health", std::to_string(static_cast<int>(entityManager.health[selected_id])) + "/" + std::to_string(static_cast<int>(entityManager.max_health[selected_id])) + " (" + getHealthCondition(entityManager.health[selected_id], entityManager.max_health[selected_id]) + ")");
    drawInfoLine("Energy", std::to_string(static_cast<int>(entityManager.energy[selected_id])) + "/" + std::to_string(static_cast<int>(entityManager.max_energy[selected_id])) + " (" + getEnergyCondition(entityManager.energy[selected_id], entityManager.max_energy[selected_id]) + ")");
    drawInfoLine("Recovery", "+" + std::to_string(entityManager.turns_since_damage[selected_id]) + " turns");
    
    // Current vs Base Stats (shows aging/hunger effects)
    drawSectionHeader("Current Stats");
    drawInfoLine("Damage", std::to_string(static_cast<int>(entityManager.current_damage[selected_id])) + " (base: " + std::to_string(static_cast<int>(entityManager.base_damage[selected_id])) + ")");
    drawInfoLine("Speed", std::to_string(static_cast<int>(entityManager.current_speed[selected_id])) + " (base: " + std::to_string(static_cast<int>(entityManager.base_speed[selected_id])) + ")");
    drawInfoLine("Sight", std::to_string(static_cast<int>(entityManager.current_sight_radius[selected_id])) + " (base: " + std::to_string(static_cast<int>(entityManager.base_sight_radius[selected_id])) + ")");
    
    // Life Cycle Information
    drawSectionHeader("Life Cycle");
    drawInfoLine("Prime Age", std::to_string(entityManager.prime_age[selected_id]) + " turns");
    drawInfoLine("Nutritional Value", std::to_string(static_cast<int>(entityManager.base_nutritional_value[selected_id])));
    drawInfoLine("Age Penalties", entityManager.age[selected_id] > entityManager.prime_age[selected_id] ? "Active" : "None");
    
    // AI State & Target
    drawSectionHeader("AI & Behavior");
    drawInfoLine("Current State", getAIStateString(entityManager.state[selected_id]));
    
    // Target information (if has target)
    if (entityManager.target_id[selected_id] < entityManager.getEntityCount() && 
        entityManager.is_alive[entityManager.target_id[selected_id]]) {
        size_t target_id = entityManager.target_id[selected_id];
        drawInfoLine("Target", getAnimalTypeString(entityManager.type[target_id]) + " at (" + 
                     std::to_string(entityManager.x[target_id]) + ", " + 
                     std::to_string(entityManager.y[target_id]) + ")");
    } else if (entityManager.target_x[selected_id] != -1 && entityManager.target_y[selected_id] != -1) {
        drawInfoLine("Target Location", "(" + std::to_string(entityManager.target_x[selected_id]) + ", " + 
                     std::to_string(entityManager.target_y[selected_id]) + ")");
    } else {
        drawInfoLine("Target", "None");
    }
}

std::string UIManager::getAnimalTypeString(AnimalType type) {
    switch (type) {
        case AnimalType::HERBIVORE: return "Herbivore";
        case AnimalType::CARNIVORE: return "Carnivore";
        case AnimalType::OMNIVORE: return "Omnivore";
        default: return "Unknown";
    }
}

std::string UIManager::getAIStateString(AIState state) {
    switch (state) {
        case AIState::WANDERING: return "Wandering";
        case AIState::SEEKING_FOOD: return "Seeking Food";
        case AIState::FLEEING: return "Fleeing";
        case AIState::CHASING: return "Chasing";
        case AIState::HERDING: return "Herding";
        case AIState::PACK_HUNTING: return "Pack Hunting";
        default: return "Unknown";
    }
}

std::string UIManager::getHealthCondition(float health, float max_health) {
    float health_percent = (health / max_health) * 100.0f;
    if (health_percent >= 80.0f) return "Excellent";
    if (health_percent >= 60.0f) return "Good";
    if (health_percent >= 40.0f) return "Injured";
    if (health_percent >= 20.0f) return "Severely Injured";
    return "Critical";
}

std::string UIManager::getEnergyCondition(float energy, float max_energy) {
    float energy_percent = (energy / max_energy) * 100.0f;
    if (energy_percent >= 80.0f) return "Well Fed";
    if (energy_percent >= 60.0f) return "Satisfied";
    if (energy_percent >= 40.0f) return "Hungry";
    if (energy_percent >= 20.0f) return "Very Hungry";
    return "Starving";
}

std::string UIManager::getAgeStage(int age, int prime_age) {
    if (age < prime_age / 2) return "Young";
    if (age < prime_age) return "Adult";
    if (age < prime_age * 1.5f) return "Mature";
    if (age < prime_age * 2.0f) return "Aging";
    return "Elder";
}
