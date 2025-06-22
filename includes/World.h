#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>
#include <cmath> // For std::sqrt or distance calculation
#include "Animal.h"
#include "Tile.h"

using SpatialGridCell = std::vector<Animal*>;

class World {
private:
  int width;
  int height;
  int turn_count;
  std::vector<std::unique_ptr<Animal>> animals;
  std::vector<std::vector<Tile>> grid;

  // --- NEW: Spatial Partitioning Grid ---

  std::vector<std::vector<SpatialGridCell>> spatial_grid;
  int spatial_grid_cell_size;
  int spatial_grid_width;
  int spatial_grid_height;


  void cleanup();
  void updateResources();
  void updateSpatialGrid();

public:
     // Constructor updated for spatial grid
    World(int w, int h, int cell_size = 10); // cell_size is a tunable parameter

    void init(int initial_herbivores, int initial_carnivores, int initial_omnivores);
    void update();
    void draw() const;
    bool isEcosystemCollapsed() const;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // This function will be rewritten to use the spatial grid
    template<typename T>
    std::vector<Animal*> getAnimalsNear(int x, int y, int radius);

    Tile& getTile(int x, int y); // Access a tile by coordinates
    const Tile& getTile(int x, int y) const; // Const version for drawing/inspection
};

// --- REWRITTEN getAnimalsNear ---
// Because it's a template, the implementation must be in the header file

template<typename T>
std::vector<Animal*> World::getAnimalsNear(int x, int y, int radius) {
    std::vector<Animal*> nearby;

    if (radius < 0) return nearby;

    int radius_sq = radius * radius;

    // Determine the range of spatial grid cells to check
    int start_cell_x = std::max(0, (x - radius) / spatial_grid_cell_size);
    int end_cell_x   = std::min(spatial_grid_width - 1, (x + radius) / spatial_grid_cell_size);
    int start_cell_y = std::max(0, (y - radius) / spatial_grid_cell_size);
    int end_cell_y   = std::min(spatial_grid_height - 1, (y + radius) / spatial_grid_cell_size);

    // Iterate through the relevant spatial grid cells
    for (int cell_y = start_cell_y; cell_y <= end_cell_y; ++cell_y) {
        for (int cell_x = start_cell_x; cell_x <= end_cell_x; ++cell_x) {
            // Iterate through animals in this cell
            for (Animal* animal : spatial_grid[cell_y][cell_x]) {
                // Check if the animal is of the correct type (T)
                if (dynamic_cast<T*>(animal)) {
                    if (!animal->isDead()) {
                        // Final distance check
                        int dx = animal->getX() - x;
                        int dy = animal->getY() - y;
                        if (dx * dx + dy * dy <= radius_sq) {
                            nearby.push_back(animal);
                        }
                    }
                }
            }
        }
    }
    return nearby;
}

#endif // WORLD_H