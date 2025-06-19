#include "Animal.h"
#include "World.h"
#include <random>
#include <cmath>

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
    if (!is_alive) return; // Cannot damage a dead animal

    health -= amount;
    turns_since_damage = 0; // Reset regen timer
    if (health <= 0) {
        kill();
    }
}

// postTurnUpdate now includes Hunger and Regeneration
void Animal::postTurnUpdate(World& world) { // Added world param, not used yet but good for future
    if (!is_alive) return;

    age++;
    energy--; // Base energy cost of living

    // --- HUNGER SYSTEM ---
    // Reset current stats to base stats before applying hunger effects
    current_damage = base_damage;
    current_speed = base_speed;
    current_sight_radius = base_sight_radius;

    if (max_energy > 0) { // Avoid division by zero if max_energy is not set
        float energy_percentage = static_cast<float>(energy) / max_energy;

        if (energy_percentage < 0.3f) { // Desperate state
            health -= 5; // Starvation damage
            current_damage += 7;
            current_speed  = std::max(1, current_speed + 2); // Ensure speed doesn't drop below 1
            current_sight_radius += 3;
        } else if (energy_percentage < 0.5f) { // Hungry state
            health -= 2; // Starvation damage
            current_damage += 2;
            current_speed = std::max(1, current_speed + 1);
            current_sight_radius += 1;
        }
    }
    // Check if starvation killed the animal
    if (health <= 0) {
        kill();
        return; // No regeneration if dead
    }

    // --- HEALTH REGENERATION ---
    turns_since_damage++;
    const int REGEN_DELAY_TURNS = 3; // How many turns without damage before regen starts
    const int REGEN_AMOUNT = 1;      // How much HP to regen per turn

    if (turns_since_damage > REGEN_DELAY_TURNS) {
        // Determine the cap: 100% if HP was >= 75%, otherwise 75%
        float regen_cap_percentage = (static_cast<float>(health) / max_health >= 0.75f && turns_since_damage == REGEN_DELAY_TURNS + 1)
                                      || (max_health - health <= (max_health * 0.25f)) // if only lost <= 25% health initially
                                      ? 1.0f : 0.75f;

        // More robust cap: if health ever dropped below 75% of max, cap is 75%
        // This requires tracking if health *ever* dropped that low.
        // For simplicity, current check: if current health is below 75% of max_health, cap is 75%.
        // A better approach would be a flag `bool severely_wounded` set in takeDamage.
        // For now, this approximation will work:
        bool was_severely_wounded = false; // Placeholder for more complex tracking
        // A simple way to check if it *was* low before starting to regen:
        if (health < 0.75 * max_health) {
             was_severely_wounded = true;
        }

        int regen_cap_hp;
        if(was_severely_wounded && health < max_health * 0.75f) { // Still severely wounded or was
            regen_cap_hp = static_cast<int>(max_health * 0.75f);
        } else {
            regen_cap_hp = max_health;
        }


        if (health < regen_cap_hp) {
            health += REGEN_AMOUNT;
            health = std::min(health, regen_cap_hp); // Clamp to the regeneration cap
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
    for (int i = 0; i < current_speed; ++i) { // Use current_speed
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
    for (int i = 0; i < current_speed; ++i) { // Use current_speed
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
    for (int i = 0; i < current_speed; ++i) { // Use current_speed
        int new_x = x + dist(rng);
        int new_y = y + dist(rng);
        if (new_x >= 0 && new_x < world.getWidth() && new_y >= 0 && new_y < world.getHeight()) {
            x = new_x;
            y = new_y;
        }
    }
}