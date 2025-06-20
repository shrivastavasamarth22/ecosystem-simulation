#include "Carnivore.h"
#include "World.h"
#include "Herbivore.h"
#include "Omnivore.h"

// --- Balancing Constants ---
const int CARNIVORE_HP = 80;
const int CARNIVORE_BASE_DMG = 25;
const int CARNIVORE_BASE_SIGHT = 5;
const int CARNIVORE_BASE_SPEED = 2;
const int CARNIVORE_MAX_ENERGY = 100; // New
const int CARNIVORE_STARTING_ENERGY = 70;
const int CARNIVORE_REPRODUCE_ENERGY_COST = 35;
const int ENERGY_FROM_KILL = 40;

// New/Adjusted constants for reproduction
const float CARNIVORE_REPRODUCE_ENERGY_PERCENTAGE = 0.80f; // << NEW: Need 70% of max energy
const int CARNIVORE_MIN_REPRODUCE_AGE = 10;                // Age requirement


const int OMNIVORE_PACK_THREAT_SIZE = 3;
// Energy gains from kills
const int ENERGY_FROM_HERBIVORE_KILL = 40;
const int ENERGY_FROM_OMNIVORE_KILL = 30; // Omnivores are tougher, maybe less net energy or same

Carnivore::Carnivore(int x, int y)
    : Animal(x, y, 'C', CARNIVORE_HP, CARNIVORE_BASE_DMG, CARNIVORE_BASE_SIGHT, CARNIVORE_BASE_SPEED,
             CARNIVORE_MAX_ENERGY, CARNIVORE_STARTING_ENERGY) {}


void Carnivore::applyAgingPenalties() {
    if (age > 50) {
        base_speed = std::max(1, base_speed - 1);
        base_damage = std::max(0, base_damage - 5);
    }

    if (age > 70) {
        base_speed = std::max(1, base_speed - 1);
        base_damage = std::max(0, base_damage - 10);
        base_sight_radius = std::max(1, base_sight_radius - 1);
    }

    base_speed = std::max(1, base_speed);
    base_damage = std::max(0, base_damage);
    base_sight_radius = std::max(1, base_sight_radius);
}

void Carnivore::updateAI(World& world) {
    target = nullptr; // Reset target each turn

    // --- Perception Phase ---
    auto nearby_omnivores = world.getAnimalsNear<Omnivore>(x, y, getCurrentSightRadius());
    auto nearby_herbivores = world.getAnimalsNear<Herbivore>(x, y, getCurrentSightRadius());

    // --- Decision Phase ---

    // Priority 1: Flee from Omnivore packs
    if (nearby_omnivores.size() >= OMNIVORE_PACK_THREAT_SIZE) {
        current_state = AIState::FLEEING;
        target = nearby_omnivores[0]; // Flee from the first one in the pack detected
        return;
    }

    // Priority 2: Hunt Herbivores
    if (!nearby_herbivores.empty()) {
        current_state = AIState::CHASING;
        target = nearby_herbivores[0]; // Target the first herbivore seen
        return;
    }

    // Priority 3: Hunt lone or small groups of Omnivores
    // This condition means there's no large omnivore pack threat, and no herbivores were found.
    // We check if there are *any* omnivores nearby that are NOT a pack threat.
    if (!nearby_omnivores.empty() && nearby_omnivores.size() < OMNIVORE_PACK_THREAT_SIZE) {
        current_state = AIState::CHASING; // Same state as chasing herbivores
        target = nearby_omnivores[0];   // Target the first lone/small group omnivore
        return;
    }

    // Priority 4: If no threats and no prey, wander
    current_state = AIState::WANDERING;
}

void Carnivore::act(World& world) {
    if (target && target->isDead()) {
        target = nullptr;
        current_state = AIState::WANDERING;
    }

    switch (current_state) {
        case AIState::CHASING:
            if (target) {
                int dx = std::abs(x - target->getX());
                int dy = std::abs(y - target->getY());
                if (dx <= 1 && dy <= 1) { // If adjacent, attack
                    target->takeDamage(getCurrentDamage());
                    if (target->isDead()) {
                        // Give energy based on what was killed
                        if (dynamic_cast<Herbivore*>(target)) {
                            energy = std::min(max_energy, energy + ENERGY_FROM_HERBIVORE_KILL);
                        } else if (dynamic_cast<Omnivore*>(target)) {
                            energy = std::min(max_energy, energy + ENERGY_FROM_OMNIVORE_KILL);
                        }
                    }
                } else { // Not adjacent, move towards target
                    moveTowards(world, target->getX(), target->getY());
                }
            }
            break;
        case AIState::FLEEING:
            if (target) {
                moveAwayFrom(world, target->getX(), target->getY());
            }
            break;
        case AIState::WANDERING:
            moveRandom(world);
            break;
        default:
            moveRandom(world);
            break;
    }
}

std::unique_ptr<Animal> Carnivore::reproduce() {
    // Use the new percentage constant for the energy threshold
    if (energy > static_cast<int>(max_energy * CARNIVORE_REPRODUCE_ENERGY_PERCENTAGE) &&
        age > CARNIVORE_MIN_REPRODUCE_AGE) {
        energy -= CARNIVORE_REPRODUCE_ENERGY_COST;
        return std::make_unique<Carnivore>(x, y);
    }
    return nullptr;
}