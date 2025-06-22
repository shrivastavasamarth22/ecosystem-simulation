#include "includes/World.h"
#include "includes/Resource.h"
#include "includes/Tile.h"
#include "includes/SimulationSystems.h"
#include "includes/AnimalConfig.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

extern std::mt19937 rng;



World::World(int w, int h, int cell_size)
    : width(w), height(h), turn_count(0),
      m_entityManager(), // Default construct the entity manager
      grid(height, std::vector<Tile>(width)),
      spatial_grid_cell_size(cell_size)
{
    // Calculate and initialize spatial grid dimensions
    spatial_grid_width = (width + cell_size - 1) / cell_size;
    spatial_grid_height = (height + cell_size - 1) / cell_size;
    spatial_grid.resize(spatial_grid_height, std::vector<SpatialGridCell>(spatial_grid_width));
}

void World::init(int initial_herbivores, int initial_carnivores, int initial_omnivores) {
    m_entityManager.clear(); // Ensure the entity manager is empty

    // --- Initialize Resources on the Grid (no changes here) ---
    std::uniform_int_distribution<int> dist_resource_amount(RESOURCE_GRASS.max_amount / 2, RESOURCE_GRASS.max_amount);
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            if (std::uniform_real_distribution<float>(0.0f, 1.0f)(rng) < 0.35f) { // Using your 35% chance
                 grid[r][c] = Tile(&RESOURCE_GRASS, dist_resource_amount(rng));
            } else {
                 grid[r][c] = Tile();
            }
        }
    }

    // --- Initialize Animals using EntityManager ---
    std::uniform_int_distribution<int> distX(0, width - 1);
    std::uniform_int_distribution<int> distY(0, height - 1);

    for (int i = 0; i < initial_herbivores; ++i) {
        m_entityManager.createHerbivore(distX(rng), distY(rng));
    }
    for (int i = 0; i < initial_carnivores; ++i) {
        m_entityManager.createCarnivore(distX(rng), distY(rng));
    }
    for (int i = 0; i < initial_omnivores; ++i) {
        m_entityManager.createOmnivore(distX(rng), distY(rng));
    }
}

void World::updateSpatialGrid() {
    // 1. clear the spatial grid from the previous turn
    for (int r = 0; r < spatial_grid_height; ++r) {
        for (int c = 0; c < spatial_grid_width; ++c) {
            spatial_grid[r][c].clear();
        }
    }

    // 2. Populate the spaatial grid with current entity positions (indices)
    size_t num_entities = m_entityManager.getEntityCount();

    for (size_t i = 0; i < num_entities; ++i) {
        // Only add living entities to the spatial grid
        if (m_entityManager.is_alive[i]) {
            int entity_x = m_entityManager.x[i];
            int entity_y = m_entityManager.y[i];

            int cell_x = entity_x / spatial_grid_cell_size;
            int cell_y = entity_y / spatial_grid_cell_size;

            // Boundary checks (shouldn't be necessary if movement is correct, but safe)
            if (cell_x >= 0 && cell_x < spatial_grid_width &&
                cell_y >= 0 && cell_y < spatial_grid_height)
            {
                spatial_grid[cell_y][cell_x].push_back(i); // <-- Store entity ID (index)
            }
        }
    }

}

void World::update() {
    turn_count++;

    updateResources();
    updateSpatialGrid(); // After resources regrow, before AI decides

    // --- Call our new Systems ---

    // Phase 1: Perception & Decision
    AISystem::run(m_entityManager, *this);

    // Phase 2: Action (Movement and Combat Systems)
    MovementSystem::run(m_entityManager, *this);
    
    // Action (Combat, Resource Consumption) MUST happen after Movement
    ActionSystem::run(m_entityManager, *this); // This system will call MetabolismSystem::applyDamage

    // Phase 3: Post-Turn Logic (Metabolism and Reproduction)
    MetabolismSystem::run(m_entityManager); // <-- Call our new Metabolism System
    // Cleanup - Remove dead entities from the EntityManager
    m_entityManager.destroyDeadEntities();
    
    ReproductionSystem::run(m_entityManager);
}

bool World::isEcosystemCollapsed() const {
    const EntityManager& data = getEntityManager();
    size_t num_entities = data.getEntityCount();

    if (num_entities == 0) {
        return true; // No entities left
    }

    int herbivore_count = 0;
    int carnivore_count = 0;
    int omnivore_count = 0;

    // Iterate through all entities in the EntityManager
    for (size_t i = 0; i < num_entities; ++i) {
        // We only count living entities for population checks
        if (data.is_alive[i]) {
            switch (data.type[i]) {
                case AnimalType::HERBIVORE:
                    herbivore_count++;
                    break;
                case AnimalType::CARNIVORE:
                    carnivore_count++;
                    break;
                case AnimalType::OMNIVORE:
                    omnivore_count++;
                    break;
                default:
                    // Should not happen with known types
                    break;
            }
        }
    }

    // The ecosystem is considered collapsed if any one species is completely wiped out (has 0 living members).
    return (herbivore_count == 0 || carnivore_count == 0 || omnivore_count == 0);
}

void World::draw() const {
    // Create a grid for drawing, initialized with tile symbols
    std::vector<std::vector<char>> draw_grid(height, std::vector<char>(width));
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            draw_grid[r][c] = grid[r][c].getSymbol();
        }
    }

    int herbivore_count = 0;
    int carnivore_count = 0;
    int omnivore_count = 0;

    // Draw animals (entities) over the resource symbols
    size_t num_entities = m_entityManager.getEntityCount();
    for (size_t i = 0; i < num_entities; ++i) {
        // Check if the entity is alive
        if (m_entityManager.is_alive[i]) {
             int entity_x = m_entityManager.x[i];
             int entity_y = m_entityManager.y[i];
             AnimalType entity_type = m_entityManager.type[i]; // Get type from EntityManager

            // Ensure entity is within grid boundaries
            if (entity_x >= 0 && entity_x < width &&
                entity_y >= 0 && entity_y < height)
            {
                // Get symbol based on type
                char symbol_to_draw = '?'; // Default unknown symbol
                if (entity_type == AnimalType::HERBIVORE) symbol_to_draw = 'H';
                else if (entity_type == AnimalType::CARNIVORE) symbol_to_draw = 'C';
                else if (entity_type == AnimalType::OMNIVORE) symbol_to_draw = 'O';

                // If multiple entities on the same spot, decide how to draw (draw the last one)
                draw_grid[entity_y][entity_x] = symbol_to_draw;
            }

            // Count entities based on type
            if (entity_type == AnimalType::HERBIVORE) herbivore_count++;
            else if (entity_type == AnimalType::CARNIVORE) carnivore_count++;
            else if (entity_type == AnimalType::OMNIVORE) omnivore_count++;
        }
    }

    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif

    std::cout << "--- Data-Oriented Ecosystem Simulation ---" << std::endl; // Updated title
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            std::cout << draw_grid[r][c] << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << "------------------------------------" << std::endl;
    std::cout << "Turn: " << turn_count
              << " | Entities: " << num_entities // Total number of entities
              << " | H: " << herbivore_count
              << " | C: " << carnivore_count
              << " | O: " << omnivore_count << std::endl;
}


// --- These functions are still valid ---
void World::updateResources() {
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            grid[r][c].regrow();
        }
    }
}

std::vector<size_t> World::getAnimalsNear(const EntityManager& data, int x, int y, int radius, AnimalType target_type) const {
    std::vector<size_t> nearby_ids;
    if (radius < 0) return nearby_ids; // Invalid radius

    int radius_sq = radius * radius;

    int start_cell_x = std::max(0, (x - radius) / spatial_grid_cell_size);
    int end_cell_x   = std::min(spatial_grid_width - 1, (x + radius) / spatial_grid_cell_size);
    int start_cell_y = std::max(0, (y - radius) / spatial_grid_cell_size);
    int end_cell_y   = std::min(spatial_grid_height - 1, (y + radius) / spatial_grid_cell_size);

    for (int cell_y = start_cell_y; cell_y <= end_cell_y; ++cell_y) {
        for (int cell_x = start_cell_x; cell_x <= end_cell_x; ++cell_x) {
            for (size_t entity_id : spatial_grid[cell_y][cell_x]) {
                // Check if the entity is alive and of the correct type (direct comparison)
                if (data.is_alive[entity_id] && data.type[entity_id] == target_type) {
                    // Final distance check
                    int dx = data.x[entity_id] - x;
                    int dy = data.y[entity_id] - y;
                    if (dx * dx + dy * dy <= radius_sq) {
                        nearby_ids.push_back(entity_id);
                    }
                }
            }
        }
    }
    return nearby_ids;
}

Tile& World::getTile(int x, int y) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        return grid[y][x];
    }
    static Tile empty_tile;
    return empty_tile;
}

const Tile& World::getTile(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        return grid[y][x];
    }
    static const Tile empty_tile_const;
    return empty_tile_const;
}

