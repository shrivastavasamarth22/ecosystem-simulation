#include "Animal.h"
#include "World.h" // Still needed for movement function calls
#include <random>
#include <cmath>
#include <algorithm> // For std::min/max

// Global RNG
std::mt19937 rng(std::random_device{}());

// Constructor updated
Animal::Animal(int start_x, int start_y, char sym,
               int m_hp, int b_dmg, int b_sight, int b_spd, int m_energy, int start_nrg)
    : x(start_x), y(start_y), age(0), is_alive(true), symbol(sym),
      health(m_hp), max_health(m_hp), turns_since_damage(0),
      base_damage(b_dmg), base_sight_radius(b_sight), base_speed(b_spd),
      current_damage(b_dmg), current_sight_radius(b_sight), current_speed(b_spd),
      energy(start_nrg), max_energy(m_energy),
      current_state(AIState::WANDERING), target(nullptr) {}

void Animal::takeDamage(int amount) {
    if (!is_alive) return;

    health -= amount;
    turns_since_damage = 0;
    if (health <= 0) {
        kill();
    }
}

// --- NEW Base Aging Penalties ---
// This is a generic aging effect. Derived classes will refine this.
void Animal::applyAgingPenalties() {
    // Example: At certain age thresholds, apply penalties
    // These are base penalties; derived classes will define their own specific thresholds and magnitudes
    // For simplicity, we'll focus on reducing base stats, which then affect current stats.
    // Making max_health decrease is also an option but more complex to balance with regeneration.

    // Let's define some generic age stages for the base class,
    // but the real impact will come from derived class overrides.
    // For now, the base class won't apply penalties directly,
    // forcing derived classes to implement their specific aging.
    // Alternatively, we can have very mild base penalties.

    // For now, let's make this base function do nothing,
    // and all aging logic will be in the derived classes' overrides.
    // This is often cleaner than having base penalties that might not fit all species.
}

void Animal::postTurnUpdate() {
    if (!is_alive) return;

    age++;
    energy--;

    // Apply aging penalties BEFORE hunger calculations,
    // so hunger buffs apply to the already aged stats.
    applyAgingPenalties();

    // --- HUNGER SYSTEM ---
    current_damage = base_damage;
    current_speed = base_speed;
    current_sight_radius = base_sight_radius;

    if (max_energy > 0) {
        float energy_percentage = static_cast<float>(energy) / max_energy;

        if (energy_percentage < 0.3f) {
            health -= 5;
            current_damage += 7;
            current_speed  = std::max(1, current_speed + 2);
            current_sight_radius += 3;
        } else if (energy_percentage < 0.5f) {
            health -= 2;
            current_damage += 2;
            current_speed = std::max(1, current_speed + 1);
            current_sight_radius += 1;
        }
    }
    if (health <= 0) {
        kill();
        return;
    }

    // --- HEALTH REGENERATION ---
    turns_since_damage++;
    const int REGEN_DELAY_TURNS = 3;
    const int REGEN_AMOUNT = 1;

    if (turns_since_damage > REGEN_DELAY_TURNS) {
        int actual_regen_cap;
        float current_health_percentage = static_cast<float>(health) / max_health;

        if (current_health_percentage >= 0.90f) {
            // Tier 1: Minor or no significant injury, can heal fully
            actual_regen_cap = max_health;
        } else if (current_health_percentage >= 0.75f) {
            // Tier 2: Moderate injury, caps at 90%
            actual_regen_cap = static_cast<int>(max_health * 0.90f);
        } else if (current_health_percentage >= 0.50f) {
            // Tier 3: Serious injury, caps at 75%
            actual_regen_cap = static_cast<int>(max_health * 0.75f);
        } else {
            // Tier 4: Critical injury, caps at 60%
            actual_regen_cap = static_cast<int>(max_health * 0.60f);
        }

        // Apply regeneration if below the determined cap
        if (health < actual_regen_cap) {
            health += REGEN_AMOUNT;
            health = std::min(health, actual_regen_cap); // Ensure health doesn't exceed the cap
        }
    }
}

bool Animal::isDead() const {
    return !is_alive;
}

void Animal::kill() {
    health = 0;
    is_alive = false;
}

// Movement functions (moveTowards, moveAwayFrom, moveRandom) remain unchanged
void Animal::moveTowards(const World& world, int target_x, int target_y) {
    for (int i = 0; i < current_speed; ++i) { 
        int dx = target_x - x;
        int dy = target_y - y;
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

void Animal::moveAwayFrom(const World& world, int target_x, int target_y) {
    for (int i = 0; i < current_speed; ++i) { 
        int dx = x - target_x;
        int dy = y - target_y;
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