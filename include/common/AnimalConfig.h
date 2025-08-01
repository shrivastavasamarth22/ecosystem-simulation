#ifndef ANIMAL_CONFIG_H
#define ANIMAL_CONFIG_H

// --- HERBIVORE CONSTANTS ---
const float HERBIVORE_BASE_HP = 30.0f;
const float HERBIVORE_BASE_DMG = 2.0f;
const float HERBIVORE_BASE_SIGHT = 7.0f;
const float HERBIVORE_BASE_SPEED = 1.0f;
const float HERBIVORE_MAX_ENERGY = 50.0f;
const float HERBIVORE_STARTING_ENERGY = 30.0f;
const float HERBIVORE_REPRODUCE_ENERGY_COST = 15.0f;
const float HERBIVORE_REPRODUCE_ENERGY_PERCENTAGE = 0.75f;
const int HERBIVORE_MIN_REPRODUCE_AGE = 3;
const int HERD_DETECTION_RADIUS = 10;
const int HERD_BONUS_RADIUS = 3;
const float HERD_AGING_REDUCTION_PER_MEMBER = 0.08f; // 8% aging reduction per herd member
const float MAX_HERD_AGING_REDUCTION = 0.5f;         // Maximum 50% aging reduction
const float HERBIVORE_FOOD_SEEK_THRESHOLD_PERCENTAGE = 0.75f;

// Herbivore nutritional value constants
const float HERBIVORE_BASE_NUTRITIONAL_VALUE = 45.0f;
const int HERBIVORE_PRIME_AGE = 8;
const float HERBIVORE_PENALTY_PER_YEAR = 2.0f;
const float HERBIVORE_MINIMUM_VALUE = 10.0f;

// --- CARNIVORE CONSTANTS ---
const float CARNIVORE_BASE_HP = 80.0f;
const float CARNIVORE_BASE_DMG = 25.0f;
const float CARNIVORE_BASE_SIGHT = 5.0f;
const float CARNIVORE_BASE_SPEED = 2.0f;
const float CARNIVORE_MAX_ENERGY = 100.0f;
const float CARNIVORE_STARTING_ENERGY = 70.0f;
const float CARNIVORE_REPRODUCE_ENERGY_COST = 35.0f;
const float CARNIVORE_REPRODUCE_ENERGY_PERCENTAGE = 0.85f;
const int CARNIVORE_MIN_REPRODUCE_AGE = 10;
const int OMNIVORE_PACK_THREAT_SIZE = 3;

// Carnivore nutritional value constants
const float CARNIVORE_BASE_NUTRITIONAL_VALUE = 50.0f;
const int CARNIVORE_PRIME_AGE = 25;
const float CARNIVORE_PENALTY_PER_YEAR = 1.0f;
const float CARNIVORE_MINIMUM_VALUE = 15.0f;

// New: Territorial behavior
const int CARNIVORE_TERRITORIAL_RADIUS = 3; // <-- NEW: Radius to detect rival Carnivores

// --- OMNIVORE CONSTANTS ---
const float OMNIVORE_BASE_HP = 60.0f;
const float OMNIVORE_BASE_DMG = 15.0f;
const float OMNIVORE_BASE_SIGHT = 6.0f;
const float OMNIVORE_BASE_SPEED = 1.0f;
const float OMNIVORE_MAX_ENERGY = 80.0f;
const float OMNIVORE_STARTING_ENERGY = 50.0f;
const float OMNIVORE_REPRODUCE_ENERGY_COST = 25.0f;
const int OMNIVORE_PACK_HUNT_SIZE = 3;
const float OMNIVORE_REPRODUCE_ENERGY_PERCENTAGE = 0.85f;
const int OMNIVORE_MIN_REPRODUCE_AGE = 8;
const float OMNIVORE_FOOD_SEEK_THRESHOLD_PERCENTAGE = 0.80f;

// Omnivore nutritional value constants
const float OMNIVORE_BASE_NUTRITIONAL_VALUE = 35.0f;
const int OMNIVORE_PRIME_AGE = 15;
const float OMNIVORE_PENALTY_PER_YEAR = 1.0f;
const float OMNIVORE_MINIMUM_VALUE = 10.0f;

// --- CARNIVORE FAMILY PROTECTION ---
const int CARNIVORE_INDEPENDENCE_AGE = 8;  // Young carnivores are protected from parents for 8 turns

#endif // ANIMAL_CONFIG_H
