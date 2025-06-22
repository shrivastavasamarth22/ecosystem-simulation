#ifndef WORLD_H
#define WORLD_H

#include "Tile.h"
#include "EntityManager.h"
#include <vector>
#include <memory>
#include <cmath>

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
    void cleanup(); // Will become m_entityManager.destroyDeadEntities() in the future
    void updateResources();
    void updateSpatialGrid();

    public:
    World(int w, int h, int cell_size = 15);

    void init(int initial_herbivores, int initial_carnivores, int initial_omnivores);
    void update(); // This will be the home of our System calls
    void draw() const;
    bool isEcosystemCollapsed() const;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

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
};

#endif // WORLD_H