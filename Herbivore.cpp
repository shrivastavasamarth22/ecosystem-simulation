#include "Herboviore.h"

const int HERBIVORE_STARTING_ENERGY = 10;
const int HERBIVORE_REPRODUCE_ENERGY = 15;
const int HERBIVORE_MAX_AGE = 50;
const int HERBIVORE_ENERGY_GAIN = 1; // Gained from eating grass

Herbivore::Herbivore(int x, int y)
  : Animal(x, y, HERBIVORE_STARTING_ENERGY, 'H') {}

void Herbivore::update(World& world) {
  if (!is_alive) return;

  age++;
  energy--; // Energy cost of living
  energy += HERBIVORE_ENERGY_GAIN;

  if (energy <= 0 || age > HERBIVORE_MAX_AGE) {
    kill();
  }
}

std::unique_ptr<Animal> Herbivore::reproduce() {
  if (energy > HERBIVORE_REPRODUCE_ENERGY) {
    energy -= (HERBIVORE_REPRODUCE_ENERGY / 2);
    return std::make_unique<Herbivore>(x, y);
  }
  return nullptr;
}
