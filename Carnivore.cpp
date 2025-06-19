#include "Carnivore.h"
#include "World.h"
#include "Herbivore.h"
#include "Omnivore.h"
#include <vector>

const int CARNIVORE_STARTING_ENERGY = 25;
const int CARNIVORE_REPRODUCE_ENERGY = 40;
const int CARNIVORE_MAX_AGE = 70;
const int CARNIVORE_HUNT_GAIN = 20;
const int CARNIVORES_NEEDED_TO_HUNT_OMNIVORE = 3;

Carnivore::Carnivore(int x, int y) : Animal(x, y, CARNIVORE_STARTING_ENERGY, 'C') {}

void Carnivore::update(World& world) {
  if (!is_alive) return;

  age++;
  energy-= 3;

  // --- Hunting logic ---
  // 1. Hunt Herbivores (alone)
  auto nearby_herbivores = world.getAnimalsNear<Herbivore>(x, y, 1);
  if (!nearby_herbivores.empty()) {
    nearby_herbivores[0]->kill();
    gainEnergy(CARNIVORE_HUNT_GAIN);
    return; // Stop after a successful hunt
  }

  // 2. Hunt Omnivores (in a group)
  auto nearby_omnivores = world.getAnimalsNear<Omnivore>(x, y, 2);
  for (auto& target_omnivore : nearby_omnivores) {
    if (target_omnivore->isDead()) continue;
    auto allies = world.getAnimalsNear<Carnivore>(target_omnivore->getX(), target_omnivore->getY(), 1);
    if (allies.size() >= CARNIVORES_NEEDED_TO_HUNT_OMNIVORE) {
      target_omnivore->kill();
      gainEnergy(CARNIVORE_HUNT_GAIN);
      break;
    }
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



