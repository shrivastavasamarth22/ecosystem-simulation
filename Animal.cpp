#include "Animal.h"
#include "World.h"
#include <random>
#include <cmath>

// Global rng
std::mt19937 rng(std::random_device{}());

Animal::Animal(int start_x, int start_y, char sym, int max_hp, int dmg, int sight, int spd, int nrg)
    : x(start_x), y(start_y), age(0), is_alive(true), symbol(sym), health(max_hp), max_health(max_hp),
      damage(dmg), sight_radius(sight), speed(spd), energy(nrg), current_state(AIState::WANDERING), target(nullptr) {}

void Animal::takeDamage(int amount) {
    health -= amount;
    if (health <= 0) {
        kill();
    }
}

void Animal::postTurnUpdate() {
    if (!is_alive) return;
    age++;
    energy--; // Energy cost of living
}

bool Animal::isDead() const {
    return !is_alive;
}

void Animal::kill() {
    health = 0;
    is_alive = false;
}

void Animal::moveTowards(int target_x, int target_y) {
    for (int i = 0; i < speed; ++i) {
        int dx = target_x - x;
        int dy = target_y - y;

        // Move on the axis with the greater distance
        if (std::abs(dx) > std::abs(dy)) {
            x += (dx > 0) ? 1 : -1;
        } else if (dy != 0) {
            y += (dy > 0) ? 1 : -1;
        } else if (dx != 0) { // Handle moving on a straight line
            x += (dx > 0) ? 1 : -1;
        }
    }
}

void Animal::moveAwayFrom(int target_x, int target_y) {
    for (int i = 0; i < speed; ++i) {
        int dx = x - target_x;
        int dy = y - target_y;

        if (std::abs(dx) > std::abs(dy)) {
            x += (dx > 0) ? 1 : -1;
        } else if (dy != 0) {
            y += (dy > 0) ? 1 : -1;
        } else if (dx != 0){
            x += (dx > 0) ? 1 : -1;
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