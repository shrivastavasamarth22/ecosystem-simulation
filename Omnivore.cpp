#include "Omnivore.h"
#include "World.h"
#include "Herbivore.h"
#include "Carnivore.h"

// --- Balancing Constants ---
const int OMNIVORE_BASE_HP = 60;
const int OMNIVORE_BASE_DMG = 15;
const int OMNIVORE_BASE_SIGHT = 6;
const int OMNIVORE_BASE_SPEED = 1;
const int OMNIVORE_MAX_ENERGY = 80; // New
const int OMNIVORE_STARTING_ENERGY = 50;
const int OMNIVORE_REPRODUCE_ENERGY_COST = 25;
const int OMNIVORE_PACK_HUNT_SIZE = 3;

// New/Adjusted constants for reproduction
const float OMNIVORE_REPRODUCE_ENERGY_PERCENTAGE = 0.85f; // << NEW: Need 65% of max energy
const int OMNIVORE_MIN_REPRODUCE_AGE = 8;                 // Age requirement

// NEW: How much energy an Omnivore needs to be below max before actively seeking food (meat or grass)
const float OMNIVORE_FOOD_SEEK_THRESHOLD_PERCENTAGE = 0.80f; // Omnivores get hungry sooner?


Omnivore::Omnivore(int x, int y)
    : Animal(x, y, 'O', OMNIVORE_BASE_HP, OMNIVORE_BASE_DMG, OMNIVORE_BASE_SIGHT, OMNIVORE_BASE_SPEED,
             OMNIVORE_MAX_ENERGY, OMNIVORE_STARTING_ENERGY) {}


void Omnivore::applyAgingPenalties() {
    if (age > 45) {
        base_speed = std::max(1, base_speed - 1);
        base_damage = std::max(0, base_damage - 3);
    }
    if (age > 60) {
        base_sight_radius = std::max(1, base_sight_radius - 1);
        base_damage = std::max(0, base_damage - 2);
    }

    base_speed = std::max(1, base_speed);
    base_damage = std::max(0, base_damage);
    base_sight_radius = std::max(1, base_sight_radius);
}


void Omnivore::updateAI(World& world) {
    target = nullptr; // Reset animal target
    target_x = -1;    // <-- Use member variable
    target_y = -1;    // <-- Use member variable

    // --- AI Decision Making ---

    // Priority 1: Flee from large Carnivore packs (remains the same)
    auto nearby_carnivores_for_pack_check = world.getAnimalsNear<Carnivore>(x,y, 2); // Check for pack threat within a small radius
    if(nearby_carnivores_for_pack_check.size() >= OMNIVORE_PACK_HUNT_SIZE) {
        current_state = AIState::FLEEING;
        target = nearby_carnivores_for_pack_check[0];
        // Clear coordinate target when targeting an animal
        target_x = -1;
        target_y = -1;
        return; // Fleeing overrides all
    }

     // Priority 2: Seek Herbivore
    auto nearby_herbivores = world.getAnimalsNear<Herbivore>(x, y, getCurrentSightRadius());
    if (!nearby_herbivores.empty()) {
        current_state = AIState::CHASING; // Chasing Herbivores
        target = nearby_herbivores[0];
        // Clear coordinate target when targeting an animal
        target_x = -1;
        target_y = -1;
        return;
    }

    // Priority 3: Seek Grass if hungry (and no Herbivores found)
    if (energy < static_cast<int>(max_energy * OMNIVORE_FOOD_SEEK_THRESHOLD_PERCENTAGE)) {
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
            current_state = AIState::SEEKING_FOOD; // Seeking Grass
            target_x = food_x; // <-- Use member variable
            target_y = food_y; // <-- Use member variable
            target = nullptr; // No animal target
            return;
        }
    }

    // Priority 4: Pack Hunt Carnivore (if conditions met and not fleeing/seeking food)
    auto nearby_carnivores_for_hunt = world.getAnimalsNear<Carnivore>(x, y, getCurrentSightRadius()); // Wider radius for hunting
    if (!nearby_carnivores_for_hunt.empty()) {
        Animal* potential_carnivore_target = nearby_carnivores_for_hunt[0];
        // Check if enough allies are near the TARGET carnivore to form a pack
        auto allies_near_target = world.getAnimalsNear<Omnivore>(potential_carnivore_target->getX(), potential_carnivore_target->getY(), 2); // Radius 2 to gather
        if(allies_near_target.size() >= OMNIVORE_PACK_HUNT_SIZE) {
            current_state = AIState::PACK_HUNTING; // Form a pack to hunt the carnivore
            target = potential_carnivore_target;
            // Clear coordinate target when targeting an animal
            target_x = -1;
            target_y = -1;
            return;
        }
    }

    // Priority 5: Wander (if none of the above)
    current_state = AIState::WANDERING;
    // Clear coordinate target when wandering
    target_x = -1;
    target_y = -1;
    
}

void Omnivore::act(World& world) {
    // Check if animal target died or disappeared
    if (target && target->isDead()) {
        target = nullptr;
        current_state = AIState::WANDERING; // Revert to wandering to re-evaluate next turn
    }
    // If seeking coordinates and somehow reached invalid ones, revert state
    if (current_state == AIState::SEEKING_FOOD && (target_x < 0 || target_y < 0)) {
        current_state = AIState::WANDERING;
        target_x = -1; target_y = -1;
    }
    // If targeting an animal and somehow has coordinate target, clear coord target
    if ((current_state == AIState::CHASING || current_state == AIState::PACK_HUNTING || current_state == AIState::FLEEING) && target_x != -1) {
    target_x = -1; target_y = -1;
    }


    // --- Attempt to Consume Resource if Present and Hungry ---
    if (energy < max_energy) {
        int energy_gained = tryConsumeResource(world, 5); // Try to consume up to 5 units of grass/resource
        if (energy_gained > 0) {
            // Debug: cout << "O ate and gained " << energy_gained << " energy!" << endl;
            // Check if they reached target food tile *and* ate
            if (current_state == AIState::SEEKING_FOOD && x == target_x && y == target_y) {
                // Reached the food tile and ate - switch state
                // Decide next state (wander, maybe seek herd/pack if needed?)
                // For simplicity, revert to wandering and let updateAI sort it next turn.
                current_state = AIState::WANDERING;
                target_x = -1; // Clear coordinate target once reached
                target_y = -1;
            }
        }
    }


    // --- Execute State-Based Movement/Action ---
    switch (current_state) {
        case AIState::FLEEING:
            if (target) {
                moveAwayFrom(world, target->getX(), target->getY());
            } else {
                moveRandom(world);
            }
            break;
        case AIState::CHASING:      // Chasing Herbivores
        case AIState::PACK_HUNTING: // Pack Hunting Carnivores
            if (target) {
                int dx = std::abs(x - target->getX());
                int dy = std::abs(y - target->getY());
                if (dx <= 1 && dy <= 1) { // If adjacent, attack
                    target->takeDamage(getCurrentDamage());
                    if (target->isDead()) {
                        int energy_gained = target->getNutritionalValue();
                        energy = std::min(max_energy, energy + energy_gained);
                    }
                } else { // Not adjacent, move towards target
                    moveTowards(world, target->getX(), target->getY()); // Move towards animal target
                }
            } else {
                 // Target disappeared, re-evaluate state next turn
                current_state = AIState::WANDERING;
                moveRandom(world);
            }
            break;
        case AIState::SEEKING_FOOD: // Seeking Grass
             // target_x and target_y are member variables
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
    // Clearing coordinate target logic is now mostly handled in updateAI and when reaching the tile.
}

int Omnivore::getNutritionalValue() const {
    const int BASE_NUTRITIONAL_VALUE = 35;
    const int PRIME_AGE = 15;
    const int PENALTY_PER_YEAR = 1;
    const int MINIMUM_VALUE = 10;

    int age_penalty = (age > PRIME_AGE) ? (age - PRIME_AGE) * PENALTY_PER_YEAR : 0;
    return std::max(MINIMUM_VALUE, BASE_NUTRITIONAL_VALUE - age_penalty);
}

std::unique_ptr<Animal> Omnivore::reproduce() {
    // Use the new percentage constant for the energy threshold
    if (energy > static_cast<int>(max_energy * OMNIVORE_REPRODUCE_ENERGY_PERCENTAGE) &&
        age > OMNIVORE_MIN_REPRODUCE_AGE) {
        energy -= OMNIVORE_REPRODUCE_ENERGY_COST;
        return std::make_unique<Omnivore>(x, y);
    }
    return nullptr;
}