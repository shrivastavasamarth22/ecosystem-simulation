#include "includes/Animal.h"
#include "includes/World.h" // Still needed for movement function calls
#include <random>
#include <cmath>
#include <algorithm> // For std::min/max

// Global RNG
std::mt19937 rng(std::random_device{}());

// Constructor updated
Animal::Animal(int start_x, int start_y, char sym,
               int base_m_hp, int b_dmg, int b_sight, int b_spd, int m_energy, int start_nrg)
    : x(start_x), y(start_y), age(0), is_alive(true), symbol(sym),
      health(base_m_hp), base_max_health(base_m_hp), max_health(base_m_hp),
      turns_since_damage(0),
      base_damage(b_dmg), base_sight_radius(b_sight), base_speed(b_spd),
      current_damage(b_dmg), current_sight_radius(b_sight), current_speed(b_spd),
      energy(start_nrg), max_energy(m_energy),
      current_state(AIState::WANDERING), target(nullptr),
      target_x(-1), target_y(-1) // <-- NEW: Initialize target coordinates
{}

void Animal::takeDamage(int amount) {
    if (!is_alive) return;

    health -= amount;
    turns_since_damage = 0;
    if (health <= 0) {
        kill();
    }
}

int Animal::getNutritionalValue() const {
    // Generic base implementation. Assumes a default value and a penalty.
    // It's expected that derived classes will provide a more specific override.
    const int BASE_NUTRITIONAL_VALUE = 20;
    const int PRIME_AGE = 15;
    const int PENALTY_PER_YEAR = 1;
    const int MINIMUM_VALUE = 5;

    int age_penalty = (age > PRIME_AGE) ? (age - PRIME_AGE) * PENALTY_PER_YEAR : 0;
    return std::max(MINIMUM_VALUE, BASE_NUTRITIONAL_VALUE - age_penalty);
}

// --- NEW Base Aging Penalties ---
// This is a generic aging effect. Derived classes will refine this.
void Animal::applyAgingPenalties() {
    // TODO: Apply aging penalties here
    // Age thresholds should reduce base stats - each species will override this
    // Consider: health penalties, stat reductions, maybe max_health changes?
    // 
    // Base implementation: no penalties (let derived classes handle specifics)
    // This way each animal type can have their own aging curve
    // without weird generic penalties that don't fit
}

bool Animal::isDead() const {
    return !is_alive;
}

void Animal::kill() {
    health = 0;
    is_alive = false;
}

// Movement functions (moveTowards, moveAwayFrom, moveRandom) remain unchanged
void Animal::moveTowards(const World& world, int target_x_coord, int target_y_coord) {
    for (int i = 0; i < current_speed; ++i) { 
        int dx = target_x_coord - x; // Use parameter name
        int dy = target_y_coord - y; // Use parameter name
        int move_dx = 0;
        int move_dy = 0;
        if (std::abs(dx) > std::abs(dy)) {
            move_dx = (dx > 0) ? 1 : -1;
        } else if (dy != 0) {
            move_dy = (dy > 0) ? 1 : -1;
        } else if (dx != 0) {
            move_dx = (dx > 0) ? 1 : -1;
        }
        int new_x = x + move_dx;
        int new_y = y + move_dy;
        if (new_x >= 0 && new_x < world.getWidth()) x = new_x;
        if (new_y >= 0 && new_y < world.getHeight()) y = new_y;
    }
}

void Animal::moveAwayFrom(const World& world, int target_x_coord, int target_y_coord) {
    for (int i = 0; i < current_speed; ++i) { 
        int dx = x - target_x_coord; // Use parameter name
        int dy = y - target_y_coord; // Use parameter name
        int move_dx = 0;
        int move_dy = 0;
        if (std::abs(dx) > std::abs(dy)) {
            move_dx = (dx > 0) ? 1 : -1;
        } else if (dy != 0) {
            move_dy = (dy > 0) ? 1 : -1;
        } else if (dx != 0) {
            move_dx = (dx > 0) ? 1 : -1;
        }
        int new_x = x + move_dx;
        int new_y = y + move_dy;
        if (new_x >= 0 && new_x < world.getWidth()) x = new_x;
        if (new_y >= 0 && new_y < world.getHeight()) y = new_y;
    }
}

void Animal::moveRandom(const World& world) {
    std::uniform_int_distribution<int> dist(-1, 1);
    for (int i = 0; i < current_speed; ++i) {
        int new_x = x + dist(rng);
        int new_y = y + dist(rng);
        if (new_x >= 0 && new_x < world.getWidth() && new_y >= 0 && new_y < world.getHeight()) {
            x = new_x;
            y = new_y;
        }
    }
}

int Animal::tryConsumeResource(World& world, int amount_requested) {
    if (!is_alive || amount_requested <= 0 || energy >= max_energy) {
        return 0; // Can't consume if dead, requesting <= 0, or already full
    }

    // Get the tile at the animal's current location
    // Assume that animal's coordinates (x, y) are valid
    Tile& current_tile = world.getTile(x, y);
    
    // Check if the tile has a resource and if it's consumable
    if (!current_tile.resource_type || current_tile.resource_amount <= 0) {
        return 0; // No resource to consume
    }

    // Consume the resource from the tile
    int amount_consumed = current_tile.consume(amount_requested);

    // Calculate energy gained based on the amount consumed and resource type
    int energy_gained = amount_consumed * current_tile.resource_type->nutritional_value;

    // Add energy, clamping to max_energy
    energy = std::min(energy + energy_gained, max_energy);

    return energy_gained; // Return the actual energy gained
}