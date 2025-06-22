#include "includes/World.h"
#include "includes/Herbivore.h"
#include "includes/Carnivore.h"
#include "includes/Omnivore.h"
#include "includes/Resource.h" // Include Resource definitions
#include "includes/Tile.h"     // Include Tile definition
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

extern std::mt19937 rng;

// Constructor updated for spatial grid
World::World(int w, int h, int cell_size)
    : width(w), height(h), turn_count(0),
      grid(height, std::vector<Tile>(width)),
      spatial_grid_cell_size(cell_size)
{
    // Calculate dimensions of the spatial grid
    spatial_grid_width = (width + cell_size - 1) / cell_size;
    spatial_grid_height = (height + cell_size - 1) / cell_size;

    // Initialize the spatial grid itself
    spatial_grid.resize(spatial_grid_height, std::vector<SpatialGridCell>(spatial_grid_width));
}

void World::init(int initial_herbivores, int initial_carnivores, int initial_omnivores) {
    animals.clear(); // Ensure animals vector is empty

    // --- Initialize Resources on the Grid ---
    // A simple approach: fill most of the grid with grass initially
    std::uniform_int_distribution<int> dist_resource_amount(RESOURCE_GRASS.max_amount / 2, RESOURCE_GRASS.max_amount);
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            // Assign Grass resource to most tiles
            if (std::uniform_real_distribution<float>(0.0f, 1.0f)(rng) < 0.35) { // 40% chance of grass
                 grid[r][c] = Tile(&RESOURCE_GRASS, dist_resource_amount(rng));
            } else {
                 grid[r][c] = Tile(); // Empty tile
            }
        }
    }


    // --- Initialize Animals ---
    std::uniform_int_distribution<int> distX(0, width - 1);
    std::uniform_int_distribution<int> distY(0, height - 1);

    for (int i = 0; i < initial_herbivores; ++i) {
        animals.push_back(std::make_unique<Herbivore>(distX(rng), distY(rng)));
    }
    for (int i = 0; i < initial_carnivores; ++i) {
        animals.push_back(std::make_unique<Carnivore>(distX(rng), distY(rng)));
    }
    for (int i = 0; i < initial_omnivores; ++i) {
        animals.push_back(std::make_unique<Omnivore>(distX(rng), distY(rng)));
    }
}

void World::updateSpatialGrid() {
    // 1. Clear the spatial grid from the previous turn
    for (int r = 0; r < spatial_grid_height; ++r) {
        for (int c = 0; c < spatial_grid_width; ++c) {
            spatial_grid[r][c].clear();
        }
    }

    // 2. Populate the spatial grid with current animal positions
    for (auto& animal : animals) {
        if (!animal->isDead()) {
            int cell_x = animal->getX() / spatial_grid_cell_size;
            int cell_y = animal->getY() / spatial_grid_cell_size;

            // Boundary checks for safety
            if (cell_x >= 0 && cell_x < spatial_grid_width &&
                cell_y >= 0 && cell_y < spatial_grid_height)
            {
                spatial_grid[cell_y][cell_x].push_back(animal.get());
            }
        }
    }
}

// --- update() function now includes spatial grid update ---
void World::update() {
    turn_count++;

    // Phase 0: Update Environment
    updateResources();

    // --- NEW: Phase 0.5: Update Spatial Grid ---
    // This MUST happen *before* the AI update, as the AI relies on it.
    updateSpatialGrid();

    // Shuffle for fairness (still a good idea)
    std::shuffle(animals.begin(), animals.end(), rng);

    // Phase 1: AI Update (Perception and Decision)
    for (auto& animal : animals) {
        if (!animal->isDead()) {
            animal->updateAI(*this);
        }
    }

    // Phase 2: Action
    for (auto& animal : animals) {
        if (!animal->isDead()) {
            animal->act(*this);
        }
    }

    // Phase 3: Post-Turn, Reproduction, and Cleanup
    std::vector<std::unique_ptr<Animal>> new_animals;
    for (auto& animal : animals) {
        if (!animal->isDead()) {
            animal->postTurnUpdate();

            auto newborn = animal->reproduce();
            if (newborn) {
                new_animals.push_back(std::move(newborn));
            }
        }
    }

    for (auto& newborn : new_animals) {
        animals.push_back(std::move(newborn));
    }

    cleanup();
}

// --- NEW Resource Update Function ---
void World::updateResources() {
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            grid[r][c].regrow(); // Tell each tile to regrow its resource
        }
    }
}


void World::cleanup() {
    animals.erase(
        std::remove_if(animals.begin(), animals.end(),
            [](const std::unique_ptr<Animal>& a) {
                return a->isDead();
            }),
        animals.end()
    );
}

// getAnimalsNear template implementation is in the header file

// --- NEW Tile Accessors ---
Tile& World::getTile(int x, int y) {
    // Basic boundary check
    if (x >= 0 && x < width && y >= 0 && y < height) {
        return grid[y][x];
    }
    // Handle invalid access - could throw an exception or return a default tile
    // For simplicity, let's return a reference to a static empty tile.
    // NOTE: This is NOT ideal for writing, only for reading.
    // A better solution for robustness would be to add checks *before* calling this.
    static Tile empty_tile; // Sentinel empty tile
    return empty_tile; // Warning: Accessing this returned tile might be unexpected.
                      // Ensure you call getTile only for valid coordinates.
}

const Tile& World::getTile(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        return grid[y][x];
    }
    static const Tile empty_tile_const; // Sentinel empty tile for const access
    return empty_tile_const;
}


void World::draw() const {
    // Create a grid for drawing, initialized with tile symbols
    std::vector<std::vector<char>> draw_grid(height, std::vector<char>(width));
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            draw_grid[r][c] = grid[r][c].getSymbol(); // <-- Use Tile's symbol
        }
    }


    int herbivore_count = 0;
    int carnivore_count = 0;
    int omnivore_count = 0;

    // Draw animals over the resource symbols
    for (const auto& animal : animals) {
        if (!animal->isDead()) {
            // Ensure animal is within grid boundaries (should be guaranteed by movement)
            if (animal->getX() >= 0 && animal->getX() < width &&
                animal->getY() >= 0 && animal->getY() < height)
            {
                // If multiple animals are on the same spot, decide how to draw (e.g., draw the last one)
                draw_grid[animal->getY()][animal->getX()] = animal->getSymbol();
            }

            // Count animals
            if (dynamic_cast<Herbivore*>(animal.get())) herbivore_count++;
            else if (dynamic_cast<Carnivore*>(animal.get())) carnivore_count++;
            else if (dynamic_cast<Omnivore*>(animal.get())) omnivore_count++;
        }
    }

    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif

    std::cout << "--- Resource Ecosystem Simulation ---" << std::endl;
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            std::cout << draw_grid[r][c] << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << "------------------------------------" << std::endl;
    std::cout << "Turn: " << turn_count
              << " | H: " << herbivore_count
              << " | C: " << carnivore_count
              << " | O: " << omnivore_count << std::endl;
}

bool World::isEcosystemCollapsed() const {
    if (animals.empty()) {
        return true;
    }

    int herbivore_count = 0;
    int carnivore_count = 0;
    int omnivore_count = 0;

    for (const auto& animal : animals) {
        if (dynamic_cast<Herbivore*>(animal.get())) herbivore_count++;
        else if (dynamic_cast<Carnivore*>(animal.get())) carnivore_count++;
        else if (dynamic_cast<Omnivore*>(animal.get())) omnivore_count++;
    }

    // Ecosystem collapses if any species is extinct
    return (herbivore_count == 0 || carnivore_count == 0 || omnivore_count == 0);
}