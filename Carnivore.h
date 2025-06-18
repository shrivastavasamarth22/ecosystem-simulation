#ifndef CARNIVORE_H
#define CARNIVORE_H

#include "Animal.h"

class Carnivore : public Animal {
  public:
    // Constructor
    Carnivore(int x, int y);

    // Override the update method
    void update(World& world) override;

    // Override the reproduce method
    std::unique_ptr<Animal> reproduce() override;
};

#endif // CARNIVORE_H