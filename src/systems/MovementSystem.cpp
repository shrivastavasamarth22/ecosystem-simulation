#include "systems/MovementSystem.h"
#include "core/Random.h"
#include "common/AnimalTypes.h"
#include <random>

namespace MovementSystem {

    // Helper function to move towards a coordinate target
    void moveTowards(EntityManager& data, size_t entity_id, const World& world, int target_x_coord, int target_y_coord) {
        if (!data.is_alive[entity_id]) return;

        // Check if already at target
        if (data.x[entity_id] == target_x_coord && data.y[entity_id] == target_y_coord) {
            return; // Already at the target coordinates, no movement needed.
        }

        for (int i = 0; i < data.current_speed[entity_id]; ++i) {
            int dx = target_x_coord - data.x[entity_id];
            int dy = target_y_coord - data.y[entity_id];

            int move_dx = 0;
            int move_dy = 0;

            // --- REVISED DIAGONAL MOVEMENT LOGIC ---
            // Move horizontally if there's a horizontal distance to cover
            if (dx != 0) {
                move_dx = (dx > 0) ? 1 : -1;
            }
            // Move vertically if there's a vertical distance to cover
            if (dy != 0) {
                move_dy = (dy > 0) ? 1 : -1;
            }
            // This allows for simultaneous movement in both x and y (diagonal).

            int new_x = data.x[entity_id] + move_dx;
            int new_y = data.y[entity_id] + move_dy;

            // Check world boundaries and only update if both coordinates are valid
            // This prevents partial updates that can cause "teleporting" behavior
            bool x_valid = (new_x >= 0 && new_x < world.getWidth());
            bool y_valid = (new_y >= 0 && new_y < world.getHeight());
            
            if (x_valid) {
                data.x[entity_id] = new_x;
            }
            if (y_valid) {
                data.y[entity_id] = new_y;
            }
            
            // If we can't move in either direction due to boundaries, stop trying
            if (!x_valid && !y_valid) {
                break;
            }

            // If we reached the target *during* this speed step, we can stop early
            if (data.x[entity_id] == target_x_coord && data.y[entity_id] == target_y_coord) {
                break; // Stop moving for this turn if target reached
            }
        }
    }

    // Helper function to move away from a coordinate target
    void moveAwayFrom(EntityManager& data, size_t entity_id, const World& world, int target_x_coord, int target_y_coord) {
        if (!data.is_alive[entity_id]) return;

        // If trying to move away from the exact spot they are on, move randomly.
        if (data.x[entity_id] == target_x_coord && data.y[entity_id] == target_y_coord) {
            moveRandom(data, entity_id, world);
            return;
        }

        for (int i = 0; i < data.current_speed[entity_id]; ++i) {
            int dx = data.x[entity_id] - target_x_coord; // Direction *away* from target
            int dy = data.y[entity_id] - target_y_coord; // Direction *away* from target

            int move_dx = 0;
            int move_dy = 0;

            // --- REVISED DIAGONAL MOVEMENT LOGIC ---
            if (dx != 0) {
                move_dx = (dx > 0) ? 1 : -1;
            }
            if (dy != 0) {
                move_dy = (dy > 0) ? 1 : -1;
            }

            // If for some reason dx/dy calculation results in no move (e.g. target is far but aligned),
            // this could stall. A random nudge can help, but the current logic should prevent this.
            // As a fallback, if no move is calculated, nudge randomly.
            if (move_dx == 0 && move_dy == 0) {
                if (rng() % 2 == 0) {
                    move_dx = (rng() % 2 == 0) ? 1 : -1;
                } else {
                    move_dy = (rng() % 2 == 0) ? 1 : -1;
                }
            }

            int new_x = data.x[entity_id] + move_dx;
            int new_y = data.y[entity_id] + move_dy;

            // Check world boundaries before updating position
             if (new_x >= 0 && new_x < world.getWidth()) {
                data.x[entity_id] = new_x;
            }
            if (new_y >= 0 && new_y < world.getHeight()) {
                data.y[entity_id] = new_y;
            }
        }
    }

    // Helper function for random movement
    void moveRandom(EntityManager& data, size_t entity_id, const World& world) {
        if (!data.is_alive[entity_id]) return;

        std::uniform_int_distribution<int> dist(-1, 1);
        for (int i = 0; i < data.current_speed[entity_id]; ++i) {
            int new_x = data.x[entity_id] + dist(rng);
            int new_y = data.y[entity_id] + dist(rng);

            if (new_x >= 0 && new_x < world.getWidth() && new_y >= 0 && new_y < world.getHeight()) {
                data.x[entity_id] = new_x;
                data.y[entity_id] = new_y;
            }
        }
    }

    // Main Movement System run function
    void run(EntityManager& data, const World& world) {
        size_t num_entities = data.getEntityCount();

        // --- Parallelize the loop using OpenMP ---
        // Each thread processes a chunk of entities.
        // Reads (data.state, data.target_id, etc.) are safe.
        // Writes (data.x[i], data.y[i]) must ONLY be to the current entity 'i'. This is true for movement.
        #pragma omp parallel for
        for (size_t i = 0; i < num_entities; ++i) {
             if (!data.is_alive[i]) continue;

             AIState current_state = data.state[i];

             switch (current_state) {
                 case AIState::FLEEING:
                 case AIState::CHASING:
                 case AIState::PACK_HUNTING:
                 case AIState::HERDING:
                 {
                     size_t target_entity_id = data.target_id[i];
                     // Check if target is valid and alive.
                     if (target_entity_id != (size_t)-1 && target_entity_id < data.getEntityCount() && data.is_alive[target_entity_id]) {
                         int target_x = data.x[target_entity_id];
                         int target_y = data.y[target_entity_id];

                         if (current_state == AIState::FLEEING) {
                              moveAwayFrom(data, i, world, target_x, target_y);
                         } else { // CHASING, PACK_HUNTING, HERDING move TOWARDS animal target
                             moveTowards(data, i, world, target_x, target_y);
                         }
                     } else {
                          // Target invalid or dead - clear target but don't change state
                          // Let AI system handle state changes to avoid race conditions
                         data.target_id[i] = (size_t)-1;
                         moveRandom(data, i, world);
                     }
                 }
                 break;

                 case AIState::SEEKING_FOOD:
                 {
                     int target_x_coord = data.target_x[i];
                     int target_y_coord = data.target_y[i];

                     // Check if coordinate target is valid.
                     if (target_x_coord != -1 && target_y_coord != -1) {
                         // Only move if not already there
                         if (data.x[i] != target_x_coord || data.y[i] != target_y_coord) {
                              moveTowards(data, i, world, target_x_coord, target_y_coord);
                         }
                     } else {
                         // Target coordinates invalid - default to random movement. AI should fix state next turn.
                         moveRandom(data, i, world);
                     }
                 }
                 break;

                 case AIState::WANDERING:
                     moveRandom(data, i, world);
                     break;

                 default: // Should not happen
                     moveRandom(data, i, world);
                     break;
             }
        } // End loop over entities
    } // End run function

} // End namespace MovementSystem
