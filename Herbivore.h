#ifndef HERBIVORE_H
#define HERBIVORE_H

#include "Animal.h"

class Herbivore : public Animal {
public:
  Herbivore(int x, int y);
  void update(World& world) override;
  std::unique_ptr<Animal> reproduce() override;
};

#endif // HERBIVORE_H