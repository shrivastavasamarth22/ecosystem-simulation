#ifndef OMNIVORE_H
#define OMNIVORE_H

#include "Animal.h"

class Omnivore : public Animal {
  public:
    Omnivore(int x, int y);
    void update(World& world) override;
    std::unique_ptr<Animal> reproduce() override;
};

#endif // OMNIVORE_H