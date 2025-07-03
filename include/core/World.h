#ifndef WORLD_H
#define WORLD_H

#include "resources/Tile.h"
#include "EntityManager.h"
#include "common/AnimalTypes.h"
#include "resources/Biome.h"
#include "core/WFCGenerator.h"
#include <vector>
#include <memory>
#include <cmath>
#include <cstddef>

using SpatialGridCell = std::vector<size_t>;

class World {
    private:
    int width;
    int height;
    int turn_count;

    // --- Data Management ---
    EntityManager m_entityManager; // World now owns the EntityManager

    std::vector<std::vector<Tile>> grid; // Grid for resources/terrain
    std::vector<std::vector<SpatialGridCell>> spatial_grid; // Stores entity IDs for spatial queries

    // Spatial Grid Properties
    int spatial_grid_cell_size;
    int spatial_grid_width;
    int spatial_grid_height;

    // --- Private Helper Functions ---
    // These will be rewritten or replaced by Systems later
    void updateResources();
    void updateSpatialGrid();
    void generateBiomes(); // <-- New terrain generation function
    void seedResources();  // <-- New resource seeding function
    int calculateOptimalCellSize() const; // <-- NEW: Calculate optimal spatial grid cell size

    public:
    World(int w, int h, int cell_size = 0); // 0 = auto-calculate optimal size

    void init(int initial_herbivores, int initial_carnivores, int initial_omnivores);
    void update(); // This will be the home of our System calls
    bool isEcosystemCollapsed() const;

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getTurnCount() const { return turn_count; }

    // This function signature is now broken because it returns Animal*.
    // We will comment it out and replace it with a new version later.
    /*
    template<typename T>
    std::vector<Animal*> getAnimalsNear(int x, int y, int radius);
    */
    // A new version will be created that returns entity IDs.

    Tile& getTile(int x, int y);
    const Tile& getTile(int x, int y) const;
    
    // Allow Systems to access the entity manager
    const EntityManager& getEntityManager() const { return m_entityManager; }
    EntityManager& getEntityManager() { return m_entityManager; }

    std::vector<size_t> getAnimalsNear(const EntityManager& data, int x, int y, int radius, AnimalType target_type) const;

};

#endif // WORLD_H
