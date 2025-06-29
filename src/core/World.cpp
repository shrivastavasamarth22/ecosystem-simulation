#include "core/World.h"
#include "resources/Resource.h"
#include "resources/Tile.h"
#include "resources/Biome.h"
#include "resources/Terrain.h"
#include "systems/SimulationSystems.h"
#include "common/AnimalConfig.h"
#include "core/Random.h"
#include "graphics/GraphicsRenderer.h"

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cstddef>
#include <cmath>

World::World(int w, int h, int cell_size)
    : width(w), height(h), turn_count(0),
      m_entityManager(), // Default construct the entity manager
      grid(height, std::vector<Tile>(width))
{
    // Calculate optimal cell size if not provided
    if (cell_size <= 0) {
        spatial_grid_cell_size = calculateOptimalCellSize();
    } else {
        spatial_grid_cell_size = cell_size;
    }
    
    // Calculate and initialize spatial grid dimensions
    spatial_grid_width = (width + spatial_grid_cell_size - 1) / spatial_grid_cell_size;
    spatial_grid_height = (height + spatial_grid_cell_size - 1) / spatial_grid_cell_size;
    spatial_grid.resize(spatial_grid_height, std::vector<SpatialGridCell>(spatial_grid_width));
    
    std::cout << "Spatial grid initialized: " << spatial_grid_width << "x" << spatial_grid_height 
              << " cells (cell size: " << spatial_grid_cell_size << ")" << std::endl;
}

void World::init(int initial_herbivores, int initial_carnivores, int initial_omnivores) {
    m_entityManager.clear(); // Ensure the entity manager is empty

    // --- NEW: Biome-based Terrain Generation ---
    generateBiomes();
    seedResources();
    // --- END NEW ---

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

void World::generateBiomes() {
    // Two-phase approach: Large regions first, then WFC for boundaries
    
    // Phase 1: Generate large biome regions using a coarser grid
    const int region_size = 15; // Each region is 15x15 tiles
    const int regions_x = (width + region_size - 1) / region_size;
    const int regions_y = (height + region_size - 1) / region_size;
    
    // Create a coarse grid of biome regions
    std::vector<std::vector<const BiomeType*>> region_grid(regions_y, std::vector<const BiomeType*>(regions_x));
    
    std::uniform_real_distribution<float> dist_chance(0.0f, 1.0f);
    
    // Assign biomes to regions
    for (int ry = 0; ry < regions_y; ++ry) {
        for (int rx = 0; rx < regions_x; ++rx) {
            float chance = dist_chance(rng);
            
            const BiomeType* region_biome;
            if (chance < 0.15f) {
                region_biome = &BIOME_WATER;
            } else if (chance < 0.25f) {
                region_biome = &BIOME_ROCKY;
            } else if (chance < 0.35f) {
                region_biome = &BIOME_FERTILE;
            } else if (chance < 0.50f) {
                region_biome = &BIOME_FOREST;
            } else if (chance < 0.75f) {
                region_biome = &BIOME_GRASSLAND;
            } else {
                region_biome = &BIOME_BARREN;
            }
            
            region_grid[ry][rx] = region_biome;
        }
    }
    
    // Phase 2: Apply WFC only near region boundaries for smooth transitions
    WFCGenerator wfc(width, height, rng);
    
    // First, assign base biomes from regions
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int rx = x / region_size;
            int ry = y / region_size;
            
            // Clamp to valid region indices
            rx = std::min(rx, regions_x - 1);
            ry = std::min(ry, regions_y - 1);
            
            grid[y][x].setBiome(region_grid[ry][rx]);
        }
    }
    
    // Phase 3: Use WFC to smooth boundaries between different regions
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            // Check if this tile is near a biome boundary
            const BiomeType* center_biome = grid[y][x].getBiome();
            bool near_boundary = false;
            
            // Check 3x3 neighborhood for different biomes
            for (int dy = -1; dy <= 1 && !near_boundary; ++dy) {
                for (int dx = -1; dx <= 1 && !near_boundary; ++dx) {
                    if (dx == 0 && dy == 0) continue;
                    
                    int nx = x + dx;
                    int ny = y + dy;
                    
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        if (grid[ny][nx].getBiome() != center_biome) {
                            near_boundary = true;
                        }
                    }
                }
            }
            
            // If near boundary, use WFC to create natural transition
            if (near_boundary) {
                // Get possible transitions based on neighbors
                std::set<const BiomeType*> neighbor_biomes;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        int nx = x + dx;
                        int ny = y + dy;
                        
                        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                            neighbor_biomes.insert(grid[ny][nx].getBiome());
                        }
                    }
                }
                
                // Choose a biome that's compatible with neighbors
                std::vector<const BiomeType*> candidates;
                for (const BiomeType* candidate : neighbor_biomes) {
                    bool is_compatible = true;
                    
                    // Check if this candidate is compatible with all neighbors
                    for (int dy = -1; dy <= 1 && is_compatible; ++dy) {
                        for (int dx = -1; dx <= 1 && is_compatible; ++dx) {
                            if (dx == 0 && dy == 0) continue;
                            
                            int nx = x + dx;
                            int ny = y + dy;
                            
                            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                                const BiomeType* neighbor = grid[ny][nx].getBiome();
                                
                                // Check adjacency rules
                                auto it = wfc.getAdjacencyRules().find(candidate);
                                if (it != wfc.getAdjacencyRules().end()) {
                                    if (it->second.find(neighbor) == it->second.end()) {
                                        is_compatible = false;
                                    }
                                }
                            }
                        }
                    }
                    
                    if (is_compatible) {
                        candidates.push_back(candidate);
                    }
                }
                
                // Randomly select from compatible candidates
                if (!candidates.empty()) {
                    std::uniform_int_distribution<int> candidate_dist(0, candidates.size() - 1);
                    grid[y][x].setBiome(candidates[candidate_dist(rng)]);
                }
            }
        }
    }
}

void World::seedResources() {
    // First, place terrain based on biome terrain distribution
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            const BiomeType* biome = grid[r][c].getBiome();
            if (!biome) continue;

            std::uniform_real_distribution<float> dist_chance(0.0f, 1.0f);
            float chance = dist_chance(rng);
            float cumulative_prob = 0.0f;

            // Place terrain first
            for (const auto& pair : biome->terrain_distribution) {
                const TerrainType* terrain = pair.first;
                float probability = pair.second;
                cumulative_prob += probability;

                if (chance < cumulative_prob) {
                    grid[r][c].setTerrain(terrain);
                    break;
                }
            }

            // If no terrain was set, default to normal terrain
            if (!grid[r][c].getTerrain()) {
                grid[r][c].setTerrain(&TERRAIN_NORMAL);
            }
        }
    }

    // Then, place resources based on biome resource distribution
    std::uniform_real_distribution<float> dist_grass_amount(RESOURCE_GRASS.max_amount / 2, RESOURCE_GRASS.max_amount);
    std::uniform_real_distribution<float> dist_berry_amount(RESOURCE_BERRIES.max_amount / 2, RESOURCE_BERRIES.max_amount);
    std::uniform_real_distribution<float> dist_bush_amount(RESOURCE_BUSH.max_amount / 2, RESOURCE_BUSH.max_amount);

    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            const BiomeType* biome = grid[r][c].getBiome();
            if (!biome) continue;

            std::uniform_real_distribution<float> dist_chance(0.0f, 1.0f);
            float chance = dist_chance(rng);
            float cumulative_prob = 0.0f;

            // Use the biome's resource distribution map
            for (const auto& pair : biome->resource_distribution) {
                const ResourceType* resource = pair.first;
                float probability = pair.second;
                cumulative_prob += probability;

                if (chance < cumulative_prob) {
                    float initial_amount = 0.0f;
                    if (resource == &RESOURCE_GRASS) {
                        initial_amount = dist_grass_amount(rng);
                    } else if (resource == &RESOURCE_BERRIES) {
                        initial_amount = dist_berry_amount(rng);
                    } else if (resource == &RESOURCE_BUSH) {
                        initial_amount = dist_bush_amount(rng);
                    }
                    grid[r][c].setResource(resource, initial_amount);
                    break; // Move to the next tile once a resource is placed
                }
            }
        }
    }
}

void World::updateSpatialGrid() {
    // 1. Clear the spatial grid from the previous turn
    // Optimized: Use a single loop instead of nested loops
    for (auto& row : spatial_grid) {
        for (auto& cell : row) {
            cell.clear();
        }
    }

    // 2. Populate the spatial grid with current entity positions (indices)
    size_t num_entities = m_entityManager.getEntityCount();

    // Reserve capacity to reduce reallocations during population
    // Estimate average entities per cell to pre-allocate
    size_t total_cells = spatial_grid_width * spatial_grid_height;
    size_t avg_entities_per_cell = (num_entities / total_cells) + 2; // +2 for safety margin

    for (auto& row : spatial_grid) {
        for (auto& cell : row) {
            cell.reserve(avg_entities_per_cell);
        }
    }

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

    // Phase 0: Animation State Capture (NEW)
    // Capture all current positions before any logic modifies them.
    AnimationSystem::capturePreviousPositions(m_entityManager);

    // Phase 1: Environment & Spatial Grid (Based on positions from END of PREVIOUS turn)
    updateResources();
    updateSpatialGrid(); // Relies on positions FROM LAST TURN's Movement


    // Phase 2: AI (Decisions for THIS turn)
    AISystem::run(m_entityManager, *this);


    // Phase 3: Action (Movement & Combat/Consumption)
    // MovementSystem::run must run after AI (needs targets/states)
    MovementSystem::run(m_entityManager, *this);

    // ActionSystem::run must run after Movement (needs new positions for adjacency)
    // This is a single-threaded system. Implicit synchronization point here.
    ActionSystem::run(m_entityManager, *this);


    // Phase 4: Post-Action Consequences
    // MetabolismSystem::run reads results of actions (damage, energy)
    MetabolismSystem::run(m_entityManager, *this);

    // Cleanup must happen after actions and metabolism finalize who is dead
    // This is a single-threaded operation that modifies the entity list structure.
    // Implicit synchronization point here.
    m_entityManager.destroyDeadEntities();

    // Reproduction happens from survivors after cleanup
    // This is a single-threaded operation that modifies the entity list structure.
    // Implicit synchronization point here.
    ReproductionSystem::run(m_entityManager);

    // Next turn's Spatial Grid update (Phase 1) will use the positions
    // resulting from THIS turn's Movement. This requires positions to be stable
    // throughout Cleanup and Reproduction.
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

    // Optimized cell range calculation
    int start_cell_x = std::max(0, (x - radius) / spatial_grid_cell_size);
    int end_cell_x   = std::min(spatial_grid_width - 1, (x + radius) / spatial_grid_cell_size);
    int start_cell_y = std::max(0, (y - radius) / spatial_grid_cell_size);
    int end_cell_y   = std::min(spatial_grid_height - 1, (y + radius) / spatial_grid_cell_size);

    // Pre-allocate result vector based on estimated capacity
    // Conservative estimate: assume ~3 entities per cell in search area
    int search_area_cells = (end_cell_x - start_cell_x + 1) * (end_cell_y - start_cell_y + 1);
    nearby_ids.reserve(search_area_cells * 3);

    for (int cell_y = start_cell_y; cell_y <= end_cell_y; ++cell_y) {
        for (int cell_x = start_cell_x; cell_x <= end_cell_x; ++cell_x) {
            const auto& cell = spatial_grid[cell_y][cell_x];
            
            // Iterate through entities in this cell
            for (size_t entity_id : cell) {
                // Check if the entity is alive and of the correct type (direct comparison)
                if (data.is_alive[entity_id] && data.type[entity_id] == target_type) {
                    // Final distance check using squared distance to avoid sqrt
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

int World::calculateOptimalCellSize() const {
    // Optimal cell size balances:
    // 1. Not too small (overhead of many empty cells)
    // 2. Not too large (entities spread across too many cells)
    
    // Base calculation: aim for cells that contain ~5-15 entities on average
    // Assume reasonable entity density for ecosystem simulation
    const float TARGET_ENTITIES_PER_CELL = 8.0f;
    const int EXPECTED_TOTAL_ENTITIES = 300; // Conservative estimate
    
    // Calculate total spatial grid cells needed
    float total_cells_needed = EXPECTED_TOTAL_ENTITIES / TARGET_ENTITIES_PER_CELL;
    
    // Calculate cell size to achieve this number of cells
    float world_area = width * height;
    float cell_area = world_area / total_cells_needed;
    int calculated_cell_size = static_cast<int>(std::sqrt(cell_area));
    
    // Clamp to reasonable bounds
    const int MIN_CELL_SIZE = 8;   // Prevent too much overhead
    const int MAX_CELL_SIZE = 32;  // Prevent inefficient large cells
    
    calculated_cell_size = std::max(MIN_CELL_SIZE, std::min(MAX_CELL_SIZE, calculated_cell_size));
    
    // Also consider sight radius - cells should be reasonably sized relative to max sight
    const int MAX_SIGHT_RADIUS = 10; // From animal configs
    const int SIGHT_BASED_SIZE = MAX_SIGHT_RADIUS * 2; // Cell size should be ~2x max sight
    
    // Use the smaller of the two calculations for better performance
    return std::min(calculated_cell_size, SIGHT_BASED_SIZE);
}

