#include "Animal.h"
#include "World.h"
#include <random>

std::mt19937 rng(std::random_device{}());

Animal::Animal(int start_x, int start_y, int start_energy, char sym)
  : x(start_x), y(start_y), energy(start_energy), age(0), is_alive(true), symbol(sym) {}

void Animal::move(const World& world) {
  if (!is_alive) return;

  std::uniform_int_distribution<int> dist(-1, 1);
  int dx = dist(rng);
  int dy = dist(rng);

  int new_x = x + dx;
  int new_y = y + dy;

  // Check world boundaries
  if (new_x >= 0 && new_x < world.getWidth() &&
      new_y >= 0 && new_y < world.getHeight()) {
    x = new_x;
    y = new_y;
  }
}

void Animal::gainEnergy(int amount) {
  energy += amount;
}

bool Animal::isDead() const {
  return !is_alive;
}

void Animal::kill() {
  is_alive = false;
}