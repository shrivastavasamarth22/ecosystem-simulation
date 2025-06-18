#include "Carnivore.h"
#include "World.h"
#include <vector>

const int CARNIVORE_START_ENERGY = 25;
const int CARNIVORE_REPRODUCE_ENERGY = 40;
const int CARNIVORE_MAX_AGE = 70;
const int CARNIVORE_HUNT_ENERGY_GAIN = 20;

Carnivore::Carnivore(int x, int y)
  : Animal(x, y, CARNIVORE_START_ENERGY, 'W') {}

void Carnivore::update(World& world) {
  if (!is_alive) return;

  age++;
  energy -= 2; // Carnivores burn more energy

  // Hunting logic
  std::vector<Animal*> nearby_hervivores = world.getHerbivoresNear(x, y, 1);
  if (!nearby_hervivores.empty()) {
    nearby_hervivores[0]->kill(); // Eat the first herbivore found
    gainEnergy(CARNIVORE_HUNT_ENERGY_GAIN);
  }

  if (energy <= 0 || age > CARNIVORE_MAX_AGE) {
    kill();
  }
}

std::unique_ptr<Animal> Carnivore::reproduce() {
  if (energy > CARNIVORE_REPRODUCE_ENERGY) {
    energy -= (CARNIVORE_REPRODUCE_ENERGY / 2);
    return std::make_unique<Carnivore>(x, y);
  }
  return nullptr;
}

