#include "World.h"
#include "Herbivore.h"
#include "Carnivore.h"
#include "Omnivore.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

// Extern the global random number generator from Animal.cpp

extern std::mt19937 rng;

World::World(int w, int h) : width(w), height(h), turn_count(0) {}

void World::init(int initial_herbivores, int initial_carnivores, int initial_omnivores) {
  animals.clear();
  std::uniform_int_distribution<int> distX(0, width - 1);
  std::uniform_int_distribution<int> distY(0, height - 1);

  for (int i = 0; i < initial_herbivores; ++i) {
    animals.push_back(std::make_unique<Herbivore>(distX(rng), distY(rng)));
  }
  for (int i = 0; i < initial_carnivores; ++i) {
    animals.push_back(std::make_unique<Carnivore>(distX(rng), distY(rng)));
  }
  for (int i = 0; i < initial_omnivores; ++i) {
    animals.push_back(std::make_unique<Omnivore>(distX(rng), distY(rng)));
  }
}

void World::update() {
    turn_count++;

    // Shuffle for fairness
    std::shuffle(animals.begin(), animals.end(), rng);

    // --- NEW THREE-PHASE UPDATE LOOP ---

    // 1. AI Update Phase: All animals perceive the world and decide on their next action.
    for (auto& animal : animals) {
        if (!animal->isDead()) {
            animal->updateAI(*this);
        }
    }

    // 2. Action Phase: All animals execute their chosen action (move, attack, etc.).
    for (auto& animal : animals) {
        if (!animal->isDead()) {
            animal->act(*this);
        }
    }

    // 3. Post-Turn & Reproduction Phase
    std::vector<std::unique_ptr<Animal>> new_animals;
    for (auto& animal : animals) {
        if (!animal->isDead()) {
            animal->postTurnUpdate(); // Aging, energy loss
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
    
    // Remove dead animals from the world
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

void World::draw() const {
  std::vector<std::vector<char>> grid (height, std::vector<char>(width, '.'));

  int herbivore_count = 0;
  int carnivore_count = 0;
  int omnivore_count = 0;

  for (const auto& animal : animals) {
    if (!animal->isDead()) {
      if (grid[animal->getY()][animal->getX()] == '.') {
        grid[animal->getY()][animal->getX()] = animal->getSymbol();
      }

      if (animal->getSymbol() == 'H') {
        herbivore_count++;
      } else if (animal->getSymbol() == 'C') {
        carnivore_count++;
      } else if (animal->getSymbol() == 'O') {
        omnivore_count++;
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
    << " | Carnivores (C): " << carnivore_count
    << " | Omnivores (O): " << omnivore_count << std::endl;
}

bool World::isEcosystemCollapsed() const {
  int herbivore_count = 0;
  int carnivore_count = 0;
  int omnivore_count = 0;


  for (const auto& animal : animals) {
    if (dynamic_cast<Herbivore*>(animal.get())) {
      herbivore_count++;
    } else if (dynamic_cast<Carnivore*>(animal.get())) {
      carnivore_count++;
    } else if (dynamic_cast<Omnivore*>(animal.get())) {
      omnivore_count++;
    }
  }
  if (herbivore_count == 0 && omnivore_count == 0) return true;
  if (carnivore_count + omnivore_count == 0) return true;
  if (herbivore_count + carnivore_count == 0) return true;

  return animals.empty();
}