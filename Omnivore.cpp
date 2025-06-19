#include "Omnivore.h"
#include "World.h"
#include "Herbivore.h"
#include "Carnivore.h"
#include <vector>

// Define constants for behavior
const int OMNIVORE_STARTING_ENERGY = 20;
const int OMNIVORE_REPRODUCE_ENERGY = 30;
const int OMNIVORE_MAX_AGE = 60;
const int OMNIVORE_GRAZE_GAIN = 1;
const int OMNIVORE_HUNT_GAIN = 15;
const int OMNIVORES_NEEDED_TO_HUNT_CARNIVORE = 3;

Omnivore::Omnivore(int x, int y) : Animal(x, y, OMNIVORE_STARTING_ENERGY, 'O') {}

void Omnivore::update(World& world) {
  if (!is_alive) return;

  age++;

  energy -= 2;
  energy += OMNIVORE_GRAZE_GAIN; // Gain energy from grazing

  // --- Hunting Logic ---

  // 1. Hunt Herbivores (alone)

  auto nearby_herbivores = world.getAnimalsNear<Herbivore>(x, y, 1);
  if (!nearby_herbivores.empty()) {
    nearby_herbivores[0]->kill();
    gainEnergy(OMNIVORE_HUNT_GAIN);
    return; // Stop after hunting one herbivore
  }

  // 2. Hunt Carnivores (in a group)
  auto nearby_carnivores = world.getAnimalsNear<Carnivore>(x, y, 2);
  for  (auto& target_carnivore: nearby_carnivores) {
    if (target_carnivore->isDead()) continue;

    auto allies = world.getAnimalsNear<Omnivore>(target_carnivore->getX(), target_carnivore->getY(), 1);
    if (allies.size() >= OMNIVORES_NEEDED_TO_HUNT_CARNIVORE) {
      target_carnivore->kill();
      gainEnergy(OMNIVORE_HUNT_GAIN);
      break; // Stop after hunting one carnivore
    }
  }

  if (energy <= 0 || age >= OMNIVORE_MAX_AGE) {
    kill(); // Omnivore dies if energy is zero or max age reached
  }

}

std::unique_ptr<Animal> Omnivore::reproduce() {
  if (energy > OMNIVORE_REPRODUCE_ENERGY) {
    energy -= (OMNIVORE_REPRODUCE_ENERGY / 2); // Reduce energy for reproduction
    return std::make_unique<Omnivore>(x, y); // Create a new
  }
  return nullptr; // Not enough energy to reproduce
}