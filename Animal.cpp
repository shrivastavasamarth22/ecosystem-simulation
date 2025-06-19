#include "Animal.h"
#include "World.h"
#include <random>
#include <cmath>

// Global RNG
std::mt19937 rng(std::random_device{}());

Animal::Animal(int start_x, int start_y, char sym, int max_hp, int dmg, int sight, int spd, int nrg)
    : x(start_x), y(start_y), age(0), is_alive(true), symbol(sym),
      health(max_hp), max_health(max_hp), damage(dmg), sight_radius(sight),
      speed(spd), energy(nrg), current_state(AIState::WANDERING), target(nullptr) {}

void Animal::takeDamage(int amount) {
    health -= amount;
    if (health <= 0) {
        kill();
    }
}

void Animal::postTurnUpdate() {
    if (!is_alive) return;
    age++;
    energy--;
}

bool Animal::isDead() const {
    return !is_alive;
}

void Animal::kill() {
    health = 0;
    is_alive = false;
}

// --- CORRECTED MOVEMENT FUNCTIONS ---

void Animal::moveTowards(const World& world, int target_x, int target_y) {
    for (int i = 0; i < speed; ++i) {
        int dx = target_x - x;
        int dy = target_y - y;

        int move_dx = 0;
        int move_dy = 0;

        // Determine direction of movement
        if (std::abs(dx) > std::abs(dy)) {
            move_dx = (dx > 0) ? 1 : -1;
        } else if (dy != 0) {
            move_dy = (dy > 0) ? 1 : -1;
        } else if (dx != 0) {
            move_dx = (dx > 0) ? 1 : -1;
        }

        int new_x = x + move_dx;
        int new_y = y + move_dy;

        // Check world boundaries before updating position
        if (new_x >= 0 && new_x < world.getWidth()) {
            x = new_x;
        }
        if (new_y >= 0 && new_y < world.getHeight()) {
            y = new_y;
        }
    }
}

void Animal::moveAwayFrom(const World& world, int target_x, int target_y) {
    for (int i = 0; i < speed; ++i) {
        int dx = x - target_x;
        int dy = y - target_y;

        int move_dx = 0;
        int move_dy = 0;

        // Determine direction of movement
        if (std::abs(dx) > std::abs(dy)) {
            move_dx = (dx > 0) ? 1 : -1;
        } else if (dy != 0) {
            move_dy = (dy > 0) ? 1 : -1;
        } else if (dx != 0) {
            move_dx = (dx > 0) ? 1 : -1;
        }

        int new_x = x + move_dx;
        int new_y = y + move_dy;

        // Check world boundaries before updating position
        if (new_x >= 0 && new_x < world.getWidth()) {
            x = new_x;
        }
        if (new_y >= 0 && new_y < world.getHeight()) {
            y = new_y;
        }
    }
}

void Animal::moveRandom(const World& world) {
    std::uniform_int_distribution<int> dist(-1, 1);
    for (int i = 0; i < speed; ++i) {
        int new_x = x + dist(rng);
        int new_y = y + dist(rng);

        if (new_x >= 0 && new_x < world.getWidth() && new_y >= 0 && new_y < world.getHeight()) {
            x = new_x;
            y = new_y;
        }
    }
}