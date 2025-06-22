#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>
#include <cmath> // For std::sqrt or distance calculation
#include "Animal.h"
#include "Tile.h"

class World {
private:
  int width;
  int height;
  int turn_count;
  std::vector<std::unique_ptr<Animal>> animals;
  std::vector<std::vector<Tile>> grid;

  void cleanup();
  void updateResources();

public:
    World(int w, int h);

    void init(int initial_herbivores, int initial_carnivores, int initial_omnivores);
    void update();
    void draw() const;
    bool isEcosystemCollapsed() const;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    template<typename T>
    std::vector<Animal*> getAnimalsNear(int x, int y, int radius);

    // --- NEW Resource Interaction Functions ---
    Tile& getTile(int x, int y); // Access a tile by coordinates
    const Tile& getTile(int x, int y) const; // Const version for drawing/inspection
};

// getAnimalsNear template implementation remains in the header because it's a template
// (The implementation from the previous version goes here)
template<typename T>
std::vector<Animal*> World::getAnimalsNear(int x, int y, int radius) {
    std::vector<Animal*> nearby;
    // Ensure radius is non-negative
    if (radius < 0) radius = 0;

    // Using squared distance optimization
    int radius_sq = radius * radius;

    for (const auto& animal : animals) {
        if (dynamic_cast<T*>(animal.get())) {
            if (!animal->isDead()) {
                int dx = animal->getX() - x;
                int dy = animal->getY() - y;
                if (dx * dx + dy * dy <= radius_sq) {
                    nearby.push_back(animal.get());
                }
            }
        }
    }
    return nearby;
}


#endif // WORLD_H