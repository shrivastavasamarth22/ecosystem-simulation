#include "World.h"
#include "Herbivore.h"
#include "Carnivore.h"
#include "Omnivore.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

// Extern the global random number generator defined in Animal.cpp
extern std::mt19937 rng;

World::World(int w, int h) : width(w), height(h), turn_count(0) {}

void World::init(int initial_herbivores, int initial_carnivores, int initial_omnivores) {
    animals.clear(); // Ensure the world is empty before initializing
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

    // Shuffle the order of animals to ensure fairness in each turn.
    // This prevents animals created first from always getting to act first.
    std::shuffle(animals.begin(), animals.end(), rng);

    // --- NEW THREE-PHASE UPDATE LOOP ---

    // Phase 1: AI Update (Perception and Decision)
    // All animals perceive the world and decide on their next action *before* anyone moves.
    for (auto& animal : animals) {
        if (!animal->isDead()) {
            animal->updateAI(*this);
        }
    }

    // Phase 2: Action
    // All animals execute their chosen action (move, attack, etc.).
    for (auto& animal : animals) {
        if (!animal->isDead()) {
            animal->act(*this);
        }
    }

    // Phase 3: Post-Turn, Reproduction, and Cleanup
    std::vector<std::unique_ptr<Animal>> new_animals;
    for (auto& animal : animals) {
        if (!animal->isDead()) {
            // Apply aging and passive energy loss
            animal->postTurnUpdate();

            // Check for reproduction
            auto newborn = animal->reproduce();
            if (newborn) {
                new_animals.push_back(std::move(newborn));
            }
        }
    }

    // Add newborn animals to the world
    for (auto& newborn : new_animals) {
        animals.push_back(std::move(newborn));
    }

    // Remove all dead animals from the simulation
    cleanup();
}

void World::cleanup() {
    // The erase-remove idiom is an efficient way to remove elements from a vector
    animals.erase(
        std::remove_if(animals.begin(), animals.end(),
            [](const std::unique_ptr<Animal>& a) {
                return a->isDead();
            }),
        animals.end()
    );
}

void World::draw() const {
    // Create a grid for drawing, initialized with a background character
    std::vector<std::vector<char>> grid(height, std::vector<char>(width, '.'));

    int herbivore_count = 0;
    int carnivore_count = 0;
    int omnivore_count = 0;

    for (const auto& animal : animals) {
        // We only care about living animals
        if (!animal->isDead()) {
            // To avoid clutter, only draw if the spot is empty
            if (grid[animal->getY()][animal->getX()] == '.') {
                 grid[animal->getY()][animal->getX()] = animal->getSymbol();
            }
            // Increment counters based on the animal's actual type, not just symbol
            if (dynamic_cast<Herbivore*>(animal.get())) herbivore_count++;
            else if (dynamic_cast<Carnivore*>(animal.get())) carnivore_count++;
            else if (dynamic_cast<Omnivore*>(animal.get())) omnivore_count++;
        }
    }

    // Clear screen for a clean redraw
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif

    std::cout << "--- Intelligent Agent Simulation ---" << std::endl;
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            std::cout << grid[r][c] << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << "------------------------------------" << std::endl;
    std::cout << "Turn: " << turn_count
              << " | Herbivores (H): " << herbivore_count
              << " | Carnivores (C): " << carnivore_count // Note the updated symbol
              << " | Omnivores (O): " << omnivore_count << std::endl;
}

bool World::isEcosystemCollapsed() const {
    if (animals.empty()) {
        return true;
    }

    int herbivore_count = 0;
    int carnivore_count = 0;
    int omnivore_count = 0;

    for (const auto& animal : animals) {
        if (dynamic_cast<Herbivore*>(animal.get())) herbivore_count++;
        else if (dynamic_cast<Carnivore*>(animal.get())) carnivore_count++;
        else if (dynamic_cast<Omnivore*>(animal.get())) omnivore_count++;
    }

    // The ecosystem is considered collapsed if any one species is completely wiped out.
    // This creates a more dynamic end condition than waiting for all but one to die.
    return (herbivore_count == 0 || carnivore_count == 0 || omnivore_count == 0);
}