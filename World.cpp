#include "World.h"
#include "Herboviore.h"
#include "Carnivore.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

// Extern the global random number generator from Animal.cpp

extern std::mt19937 rng;

World::World(int w, int h) : width(w), height(h), turn_count(0) {}

void World::init(int initial_herbivores, int initial_carnivores) {
  std::uniform_int_distribution<int> distX(0, width - 1);
  std::uniform_int_distribution<int> distY(0, height - 1);

  for (int i = 0; i < initial_herbivores; ++i) {
    animals.push_back(std::make_unique<Herbivore>(distX(rng), distY(rng)));
  }
  for (int i = 0; i < initial_carnivores; ++i) {
    animals.push_back(std::make_unique<Carnivore>(distX(rng), distY(rng)));
  }
}

void World::update() {
  turn_count++;

  // Update, move, and handle reproduction for all animals
  std::vector<std::unique_ptr<Animal>> new_animals;
  for (auto& animal : animals) {
    if (!animal->isDead()) {
      animal->move(*this);
      animal->update(*this);

      auto newborn = animal->reproduce();
      if (newborn) {
        new_animals.push_back(std::move(newborn));
      }
    }
  }


  // Add newborns to the world
  for (auto& newborn : new_animals) {
    animals.push_back(std::move(newborn));
  }

  cleanup();
}

void World::cleanup() {
  animals.erase(
    std::remove_if(animals.begin(), animals.end(), [](const std::unique_ptr<Animal>& a) {
      return a->isDead();
    }),
    animals.end()
  );
}

std::vector<Animal*> World::getHerbivoresNear(int x, int y, int radius) {
  std::vector<Animal*> nearby;
  for (const auto& animal: animals) {
    if (dynamic_cast<Herbivore*>(animal.get())) {
      if (!animal->isDead()) {
        int dx = animal->getX() - x;
        int dy = animal->getY() - y;
        if (dx * dx + dy * dy <= radius * radius) {
          nearby.push_back(animal.get());
        }
      }
    }
  }
  return nearby;
}

void World::draw() const {
  std::vector<std::vector<char>> grid (height, std::vector<char>(width, '.'));

  int herbivore_count = 0;
  int carnivore_count = 0;

  for (const auto& animal : animals) {
    if (!animal->isDead()) {
      grid[animal->getY()][animal->getX()] = animal->getSymbol();
      if(animal->getSymbol() == 'H') {
        herbivore_count++;
      } 
      if(animal->getSymbol() == 'W') {
        carnivore_count++;
      }
    }
  }

  // Clear screen (simple cross-platform way)
  #ifdef _WIN32
  system("cls");
  #else
  system("clear");
  #endif

  std::cout << "--- Predator-Prey Simulation ---" << std::endl;
  for (int r = 0; r < height; ++r) {
    for (int c = 0; c < width; ++c) {
      std::cout << grid[r][c] << ' ';
    }
    std::cout << std::endl;
  }
  std::cout << "--------------------------------" << std::endl;
  std::cout << "Turn: " << turn_count
    << " | Herbivores (H): " << herbivore_count
    << " | Carnivores (W): " << carnivore_count << std::endl;
}

bool World::isExtinct() const {
  int herbivore_count = 0;
  int carnivore_count = 0;
  for (const auto& animal : animals) {
    if (dynamic_cast<Herbivore*>(animal.get())) herbivore_count++;
    if (dynamic_cast<Carnivore*>(animal.get())) carnivore_count++;
  }
  return herbivore_count == 0 || carnivore_count == 0;
}