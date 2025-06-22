#include "Herbivore.h"
#include "World.h"
#include "Carnivore.h"
#include "Omnivore.h"
#include "Tile.h"

// --- Balancing Constants ---
const int HERBIVORE_BASE_HP = 30; // Renamed from HERBIVORE_HP for clarity
const int HERBIVORE_BASE_DMG = 2;
const int HERBIVORE_BASE_SIGHT = 7;
const int HERBIVORE_BASE_SPEED = 1;
const int HERBIVORE_MAX_ENERGY = 50;
const int HERBIVORE_STARTING_ENERGY = 30;
const int HERBIVORE_REPRODUCE_ENERGY_COST = 15;
const float HERBIVORE_REPRODUCE_ENERGY_PERCENTAGE = 0.75f;
const int HERBIVORE_MIN_REPRODUCE_AGE = 3;

// --- NEW Herding Constants ---
const int HERD_DETECTION_RADIUS = 10;   // How far a Herbivore can see to find a herd
const int HERD_BONUS_RADIUS = 3;        // How close friends must be to count for HP bonus
const int HERD_HP_BONUS_PER_MEMBER = 5; // +5 max HP for each nearby friend

// NEW: How much energy a Herbivore needs to be below max before actively seeking food
const float HERBIVORE_FOOD_SEEK_THRESHOLD_PERCENTAGE = 0.75f;


Herbivore::Herbivore(int x, int y)
    : Animal(x, y, 'H', HERBIVORE_BASE_HP, HERBIVORE_BASE_DMG, HERBIVORE_BASE_SIGHT, HERBIVORE_BASE_SPEED,
             HERBIVORE_MAX_ENERGY, HERBIVORE_STARTING_ENERGY) {}


void Herbivore::updateAI(World& world) {
    target = nullptr; // Reset animal target each turn
    target_x = -1;    // <-- Use member variable
    target_y = -1;    // <-- Use member variable

    // --- Herd Buff Calculation ---
    // First, determine buffs from the current situation before making decisions.
    auto nearby_friends = world.getAnimalsNear<Herbivore>(x, y, HERD_BONUS_RADIUS);
    int herd_size = nearby_friends.size();

    // Calculate the HP bonus from the herd
    int hp_bonus = (herd_size > 1) ? (herd_size - 1) * HERD_HP_BONUS_PER_MEMBER : 0;
    int old_max_health = max_health;
    max_health = base_max_health + hp_bonus;

    // If max health increased, give the bonus current health.
    if (max_health > old_max_health) {
        health += (max_health - old_max_health);
    }
    // Crucially, clamp current health to the new max_health.
    // This handles both gaining and losing herd members.
    health = std::min(health, max_health);


    // --- AI Decision Making ---
    // Priority 1: Flee from immediate threats
    auto predators = world.getAnimalsNear<Carnivore>(x, y, getCurrentSightRadius());
    auto omni_predators = world.getAnimalsNear<Omnivore>(x, y, getCurrentSightRadius());
    predators.insert(predators.end(), omni_predators.begin(), omni_predators.end());

    if (!predators.empty()) {
        current_state = AIState::FLEEING;
        target = predators[0]; // Flee from the first one seen
        // Clear coordinate target when targeting an animal
        target_x = -1;
        target_y = -1;
        return;
    }

    // Priority 2: Seek food if hungry
    if (energy < static_cast<int>(max_energy * HERBIVORE_FOOD_SEEK_THRESHOLD_PERCENTAGE)) {
         int best_food_amount = 0;
         int food_x = -1, food_y = -1;

         int scan_radius = getCurrentSightRadius();
         for (int dy = -scan_radius; dy <= scan_radius; ++dy) {
             for (int dx = -scan_radius; dx <= scan_radius; ++dx) {
                 int check_x = x + dx;
                 int check_y = y + dy;

                 if (check_x >= 0 && check_x < world.getWidth() &&
                     check_y >= 0 && check_y < world.getHeight() &&
                     dx * dx + dy * dy <= scan_radius * scan_radius)
                 {
                     const Tile& tile = world.getTile(check_x, check_y);
                     if (tile.resource_type && tile.resource_amount > best_food_amount) {
                         best_food_amount = tile.resource_amount;
                         food_x = check_x;
                         food_y = check_y;
                     }
                 }
             }
         }

         if (best_food_amount > 0) {
             current_state = AIState::SEEKING_FOOD;
             target_x = food_x; // <-- Use member variable
             target_y = food_y; // <-- Use member variable
             target = nullptr; // No animal target when seeking resource
             return;
         }
         // If hungry but no food found in sight, proceed to seek herd/wander
    }

    // Priority 3: Seek out a herd if not in one (modified slightly)
    if (herd_size <= 1) {
        auto potential_herd = world.getAnimalsNear<Herbivore>(x, y, HERD_DETECTION_RADIUS);
        if (potential_herd.size() > 1) {
            current_state = AIState::HERDING;
            target = potential_herd[0] == this ? potential_herd[1] : potential_herd[0];
            // Clear coordinate target when targeting an animal
            target_x = -1;
            target_y = -1;
            return;
        }
    }

     // Priority 4: If safe, not actively seeking food or herd, wander and graze (implicitly)
    current_state = AIState::WANDERING;
    // Clear coordinate target when wandering
    target_x = -1;
    target_y = -1;
}


void Herbivore::act(World& world) {
    // Check if animal target died or disappeared (only relevant for FLEEING/HERDING)
    if (target && target->isDead()) {
        target = nullptr;
        current_state = AIState::WANDERING; // Revert to wandering to re-evaluate state next turn
    }
    // If seeking coordinates and somehow reached invalid ones, revert state
     if (current_state == AIState::SEEKING_FOOD && (target_x < 0 || target_y < 0)) {
         current_state = AIState::WANDERING;
     }
     if (current_state == AIState::HERDING && target_x != -1) { // If herding and somehow has coord target
          target_x = -1; target_y = -1; // Clear coord target
     }


    // --- Attempt to Consume Resource if Present and Hungry ---
     if (energy < max_energy) {
        int energy_gained = tryConsumeResource(world, 5); // Try to consume up to 5 units
        // You can adjust the amount requested here.
        if (energy_gained > 0) {
             // maybe check if they reached target food tile?
             if (current_state == AIState::SEEKING_FOOD && x == target_x && y == target_y) {
                 // Reached the food tile, switch state back to wandering/herding
                 // Decide whether to join nearby herd or just wander
                 auto nearby_friends = world.getAnimalsNear<Herbivore>(x, y, HERD_BONUS_RADIUS);
                 if (nearby_friends.size() > 1) {
                     current_state = AIState::HERDING;
                     // Need to find another friend to target again next turn, simpler to just wander and let updateAI sort it
                     target = nullptr; // Clear animal target
                 } else {
                    current_state = AIState::WANDERING;
                 }
                 // Crucially, clear coordinate target once reached
                 target_x = -1;
                 target_y = -1;
             }
             // Debug: cout << "H ate and gained " << energy_gained << " energy!" << endl;
        }
     }


    // --- Execute State-Based Movement ---
    switch (current_state) {
        case AIState::FLEEING:
            if (target) {
                moveAwayFrom(world, target->getX(), target->getY());
            } else {
                moveRandom(world);
            }
            break;
        case AIState::HERDING:
            if (target) {
                moveTowards(world, target->getX(), target->getY()); // Move towards animal target
            } else {
                 // Target disappeared, re-evaluate state next turn
                current_state = AIState::WANDERING;
                moveRandom(world);
            }
            break;
        case AIState::SEEKING_FOOD:
            // target_x and target_y were set in updateAI and are now member variables
            if (target_x != -1 && target_y != -1) { // Check if target coords are valid
                 // If we reached the target tile, the state will be handled by the consumption logic above
                 // If not reached yet, move towards it
                 if (!(x == target_x && y == target_y)) { // Only move if not already there
                      moveTowards(world, target_x, target_y); // Move towards coordinate target
                 }
            } else {
                 // Target food disappeared or invalid, re-evaluate state next turn
                 current_state = AIState::WANDERING;
                 moveRandom(world);
            }
            break;
        case AIState::WANDERING:
            moveRandom(world);
            break;
        default: // Should not reach here
            moveRandom(world);
            break;
    }

    // Clearing target_x/y after action phase might interfere if the animal didn't move
    // Let's rely on updateAI to set/clear them explicitly based on decision making.
    // However, clearing them when reaching the target tile in act() is necessary.
    // Also clearing them when the state changes away from SEEKING_FOOD/HERDING (towards animal) is needed in updateAI.

    // Let's revisit the clearing logic in act() after movement.
    // If we were SEEKING_FOOD and *didn't* reach the tile this turn, the target_x/y should persist.
    // If we were HERDING or FLEEING and moved, the animal target persists.
    // Clearing logic should only happen in updateAI (when deciding state/target)
    // OR in act() specifically when a target (animal or coordinate) is *successfully reached* or becomes invalid.

    // The check for reaching target food and changing state/clearing target_x/y is now inside the consumption block.
    // This is okay, but might miss the case where food isn't consumed (e.g. already full energy).
    // A better check is outside the consumption logic:
    if (current_state == AIState::SEEKING_FOOD && x == target_x && y == target_y) {
        // Reached target food tile - decide next state (wander/herd) and clear target_x/y
        auto nearby_friends = world.getAnimalsNear<Herbivore>(x, y, HERD_BONUS_RADIUS);
        if (nearby_friends.size() > 1) {
            current_state = AIState::HERDING;
             target = nullptr; // Clear animal target
        } else {
           current_state = AIState::WANDERING;
        }
        target_x = -1; // Clear coordinate target once reached
        target_y = -1;
    }
}

// applyAgingPenalties() and reproduce() methods
void Herbivore::applyAgingPenalties() {
    if (age > 40) {
        base_speed = std::max(1, base_speed - 1);
    }
    base_speed = std::max(1, base_speed);
}

int Herbivore::getNutritionalValue() const {
    const int BASE_NUTRITIONAL_VALUE = 45;
    const int PRIME_AGE = 8;
    const int PENALTY_PER_YEAR = 2;
    const int MINIMUM_VALUE = 10;
    int age_penalty = (age > PRIME_AGE) ? (age - PRIME_AGE) * PENALTY_PER_YEAR : 0;
    return std::max(MINIMUM_VALUE, BASE_NUTRITIONAL_VALUE - age_penalty);
}

std::unique_ptr<Animal> Herbivore::reproduce() {
    if (energy > static_cast<int>(max_energy * HERBIVORE_REPRODUCE_ENERGY_PERCENTAGE) &&
        age > HERBIVORE_MIN_REPRODUCE_AGE) {
        energy -= HERBIVORE_REPRODUCE_ENERGY_COST;
        return std::make_unique<Herbivore>(x, y);
    }
    return nullptr;
}