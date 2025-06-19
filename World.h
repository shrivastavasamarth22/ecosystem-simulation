#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>
#include <cmath> // For std::sqrt or distance calculation
#include "Animal.h"

class World {
private:
  int width;
  int height;
  int turn_count;
  std::vector<std::unique_ptr<Animal>> animals;

  void cleanup();

public:
  World(int w, int h);

  void init(int initial_herbivores, int initial_carnivores, int initial_omnivores);
  void update();
  void draw() const;
  bool isEcosystemCollapsed() const;

  // Getters for other classes to use
  int getWidth() const { return width; }
  int getHeight() const { return height; }

  // Templated helper to get specific animal types near a point
  template<typename T>
  std::vector<Animal*> getAnimalsNear(int x, int y, int radius) {
    std::vector<Animal*> nearby;
    for (const auto& animal : animals) {
      // Use dynamic_cast to check if the animal is of type T
      if (dynamic_cast<T*>(animal.get())) {
        if (!animal->isDead()) {
          int dx = animal->getX() - x;
          int dy = animal->getY() - y;
          // Using squared distance is more efficient (avoids sqrt)
          if (dx * dx + dy * dy <= radius * radius) {
            nearby.push_back(animal.get());
          }
        }
      }
    }
    return nearby;
  }
};

#endif // WORLD_H