#include "World.h"
#include "Resource.h"
#include "Tile.h"
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

// --- WARNING: The following functions are now BROKEN and will be rewritten later ---

void World::updateSpatialGrid() {
    // This function will be rewritten to use m_entityManager
}

void World::update() {
    // This function will be rewritten to call Systems
}

void World::cleanup() {
    // This will be replaced by a call to m_entityManager
}

void World::draw() const {
    // This function will be rewritten to use m_entityManager
}

bool World::isEcosystemCollapsed() const {
    // This function will be rewritten to use m_entityManager
    return false; // Placeholder
}


// --- These functions are still valid ---
void World::updateResources() {
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            grid[r][c].regrow();
        }
    }
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