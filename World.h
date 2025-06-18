#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>
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

    void init(int initial_herbivores, int initial_carnivores);
    void update();
    void draw() const;
    bool isExtinct() const;

    // Getters for other classes to use
    int getWidth() const {
      return width;
    };
    int getHeight() const {
      return height;
    };

    // Helper for Carnivore hunting logic
    std::vector<Animal*> getHerbivoresNear(int x, int y, int radius);
};

#endif // WORLD_H