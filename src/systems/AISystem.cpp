#include "systems/AISystem.h"
#include "common/AnimalConfig.h"
#include "common/AnimalTypes.h"
#include <algorithm>
#include <cmath>

namespace AISystem {

    void run(EntityManager& data, const World& world) {
        size_t num_entities = data.getEntityCount();

        // --- PHASE 1: Calculate herd bonuses (SINGLE-THREADED to avoid race conditions) ---
        for (size_t i = 0; i < num_entities; ++i) {
            if (!data.is_alive[i] || data.type[i] != AnimalType::HERBIVORE) continue;
            
            // Calculate herd bonus for herbivores
            auto nearby_friends = world.getAnimalsNear(data, data.x[i], data.y[i], HERD_BONUS_RADIUS, AnimalType::HERBIVORE);
            int herd_size = nearby_friends.size();
            
            float hp_bonus = (herd_size > 1) ? (herd_size - 1) * HERD_HP_BONUS_PER_MEMBER : 0.0f;
            float old_max_health = data.max_health[i];
            data.max_health[i] = data.base_max_health[i] + hp_bonus;
            if (data.max_health[i] > old_max_health) data.health[i] += (data.max_health[i] - old_max_health);
            data.health[i] = std::min(data.health[i], data.max_health[i]);
        }

        // --- PHASE 2: AI Decision Making (PARALLELIZED) ---
        // Each thread processes a chunk of entities.
        // Reads (world, data.is_alive, data.type, data.x, data.y, etc.) are safe.
        // Writes (data.state, data.target_id, data.target_x, data.target_y) 
        // must ONLY be to the current entity 'i'. This is true for the AI logic.
        #pragma omp parallel for
        for (size_t i = 0; i < num_entities; ++i) {
                if (!data.is_alive[i]) continue;

                // Clear invalid targets at the start of AI processing
                if (data.target_id[i] != (size_t)-1) {
                    if (data.target_id[i] >= data.getEntityCount() || !data.is_alive[data.target_id[i]]) {
                        data.target_id[i] = (size_t)-1; // Clear invalid target
                    }
                }

                data.target_id[i] = (size_t)-1; data.target_x[i] = -1; data.target_y[i] = -1;

                // --- Variables local to the loop iteration ---
                // Use best_food_energy instead of best_food_amount
                int food_x = -1;
                int food_y = -1;

                switch (data.type[i]) {
                    case AnimalType::HERBIVORE:
                    {
                        // Herd size calculation for herding behavior (read-only, safe in parallel)
                        auto nearby_friends = world.getAnimalsNear(data, data.x[i], data.y[i], HERD_BONUS_RADIUS, AnimalType::HERBIVORE);
                        int herd_size = nearby_friends.size();

                        // Decision Making
                        auto predators = world.getAnimalsNear(data, data.x[i], data.y[i], static_cast<int>(data.current_sight_radius[i]), AnimalType::CARNIVORE);
                        auto omni_predators = world.getAnimalsNear(data, data.x[i], data.y[i], static_cast<int>(data.current_sight_radius[i]), AnimalType::OMNIVORE);
                        predators.insert(predators.end(), omni_predators.begin(), omni_predators.end());
                        if (!predators.empty()) {
                            data.state[i] = AIState::FLEEING; data.target_id[i] = predators[0]; continue;
                        }

                        // Seek Food if hungry - Prioritize by ENERGY-TO-DISTANCE RATIO
                        if (data.energy[i] < data.max_energy[i] * HERBIVORE_FOOD_SEEK_THRESHOLD_PERCENTAGE) {
                            // Use best_food_efficiency (energy per distance)
                            float best_food_efficiency = 0.0f; 
                            food_x = -1; food_y = -1; // Reset for this scan
                            int scan_radius = static_cast<int>(data.current_sight_radius[i]);

                            for (int dy = -scan_radius; dy <= scan_radius; ++dy) { 
                                for (int dx = -scan_radius; dx <= scan_radius; ++dx) {
                                    int check_x = data.x[i] + dx; 
                                    int check_y = data.y[i] + dy;
                                    
                                    // Skip if out of bounds or beyond sight radius
                                    if (check_x < 0 || check_x >= world.getWidth() || 
                                        check_y < 0 || check_y >= world.getHeight() || 
                                        dx * dx + dy * dy > scan_radius * scan_radius) {
                                        continue;
                                    }
                                    
                                    const Tile& tile = world.getTile(check_x, check_y);
                                    // Ensure there is actually consumable food
                                    if (tile.resource_type && tile.getConsumableAmount() > 0.0f) {
                                        float potential_energy = tile.resource_amount * tile.resource_type->nutritional_value;
                                        
                                        // Calculate distance (minimum 1 to avoid division by zero)
                                        float distance = std::max(1.0f, std::sqrt(float(dx * dx + dy * dy)));
                                        
                                        // Calculate efficiency: energy per unit distance
                                        // Add small bonus for very close food to prefer nearby resources
                                        float efficiency = potential_energy / distance;
                                        if (distance <= 2.0f) efficiency *= 1.5f; // 50% bonus for adjacent/very close food
                                        
                                        if (efficiency > best_food_efficiency) {
                                            best_food_efficiency = efficiency;
                                            food_x = check_x; 
                                            food_y = check_y;
                                        }
                                    }
                                }
                            }
                            
                            // Only seek food if we found something reasonably efficient
                            if (best_food_efficiency > 0.0f) {
                                data.state[i] = AIState::SEEKING_FOOD; 
                                data.target_x[i] = food_x; 
                                data.target_y[i] = food_y; 
                                continue;
                            }
                        }

                        // Seek out a herd if not in one - Uses calculated herd_size
                        if (herd_size <= 1) {
                            auto potential_herd = world.getAnimalsNear(data, data.x[i], data.y[i], HERD_DETECTION_RADIUS, AnimalType::HERBIVORE);
                            if (!potential_herd.empty()) {
                                size_t closest_herd_member_id = (size_t)-1;
                                float closest_distance_sq = float(HERD_DETECTION_RADIUS * HERD_DETECTION_RADIUS + 1); // Start with max+1
                                
                                for(size_t potential_target_id : potential_herd) {
                                    if (potential_target_id != i) { // Exclude self
                                        // Calculate distance to this potential herd member
                                        int dx = data.x[i] - data.x[potential_target_id];
                                        int dy = data.y[i] - data.y[potential_target_id];
                                        float distance_sq = float(dx * dx + dy * dy);
                                        
                                        if (distance_sq < closest_distance_sq) {
                                            closest_distance_sq = distance_sq;
                                            closest_herd_member_id = potential_target_id;
                                        }
                                    }
                                }
                                
                                if (closest_herd_member_id != (size_t)-1) {
                                    data.state[i] = AIState::HERDING; 
                                    data.target_id[i] = closest_herd_member_id; 
                                    continue;
                                }
                            }
                        }
                        data.state[i] = AIState::WANDERING;
                    } break;

                    case AnimalType::CARNIVORE:
                    {
                        // Priority 1: Flee from Omnivore packs
                        auto nearby_omnivores_for_pack_check = world.getAnimalsNear(data, data.x[i], data.y[i], static_cast<int>(data.current_sight_radius[i]), AnimalType::OMNIVORE);
                        if(nearby_omnivores_for_pack_check.size() >= OMNIVORE_PACK_THREAT_SIZE) {
                            // Choose the closest omnivore as flee target
                            size_t closest_omnivore_id = (size_t)-1;
                            float closest_distance_sq = float(data.current_sight_radius[i] * data.current_sight_radius[i] + 1);
                            
                            for (size_t potential_omnivore_id : nearby_omnivores_for_pack_check) {
                                // Calculate distance to this omnivore
                                int dx = data.x[i] - data.x[potential_omnivore_id];
                                int dy = data.y[i] - data.y[potential_omnivore_id];
                                float distance_sq = float(dx * dx + dy * dy);
                                
                                if (distance_sq < closest_distance_sq) {
                                    closest_distance_sq = distance_sq;
                                    closest_omnivore_id = potential_omnivore_id;
                                }
                            }
                            
                            if (closest_omnivore_id != (size_t)-1) {
                                data.state[i] = AIState::FLEEING; 
                                data.target_id[i] = closest_omnivore_id; 
                                continue;
                            }
                        }

                        // --- NEW Priority 2: Confront Rival Carnivores ---
                        // Check for *other* carnivores within territorial radius
                        auto nearby_rival_carnivores = world.getAnimalsNear(data, data.x[i], data.y[i], CARNIVORE_TERRITORIAL_RADIUS, AnimalType::CARNIVORE);
                        // Find the closest rival (excluding self)
                        if (!nearby_rival_carnivores.empty()) {
                            size_t closest_rival_id = (size_t)-1;
                            float closest_distance_sq = float(CARNIVORE_TERRITORIAL_RADIUS * CARNIVORE_TERRITORIAL_RADIUS + 1); // Start with max+1
                            
                            for (size_t potential_rival_id : nearby_rival_carnivores) {
                                if (potential_rival_id != i) { // Exclude self
                                    // Calculate distance to this rival
                                    int dx = data.x[i] - data.x[potential_rival_id];
                                    int dy = data.y[i] - data.y[potential_rival_id];
                                    float distance_sq = float(dx * dx + dy * dy);
                                    
                                    if (distance_sq < closest_distance_sq) {
                                        closest_distance_sq = distance_sq;
                                        closest_rival_id = potential_rival_id;
                                    }
                                }
                            }
                            
                            if (closest_rival_id != (size_t)-1) {
                                data.state[i] = AIState::CHASING; // Use CHASING state for combat
                                data.target_id[i] = closest_rival_id;
                                continue; // Decision made
                            }
                        }

                        // Old Priority 2 becomes NEW Priority 3: Hunt Herbivores
                        auto nearby_herbivores = world.getAnimalsNear(data, data.x[i], data.y[i], static_cast<int>(data.current_sight_radius[i]), AnimalType::HERBIVORE);
                        if (!nearby_herbivores.empty()) {
                            data.state[i] = AIState::CHASING; data.target_id[i] = nearby_herbivores[0]; continue;
                        }

                        // Old Priority 3 becomes NEW Priority 4: Hunt lone or small groups of Omnivores
                        auto nearby_omnivores_full_sight = world.getAnimalsNear(data, data.x[i], data.y[i], static_cast<int>(data.current_sight_radius[i]), AnimalType::OMNIVORE);
                        if (!nearby_omnivores_full_sight.empty()) {
                            data.state[i] = AIState::CHASING; data.target_id[i] = nearby_omnivores_full_sight[0]; continue;
                        }

                        // Old Priority 4 becomes NEW Priority 5: If no threats, no prey, no rivals, wander
                        data.state[i] = AIState::WANDERING;
                    } break;

                    case AnimalType::OMNIVORE:
                    {
                        // Priority 1: Flee from Carnivore groups (using full sight radius)
                        auto nearby_carnivores_for_pack_check = world.getAnimalsNear(data, data.x[i], data.y[i], static_cast<int>(data.current_sight_radius[i]), AnimalType::CARNIVORE);
                        if(nearby_carnivores_for_pack_check.size() >= OMNIVORE_PACK_HUNT_SIZE) {
                            // Choose the closest carnivore as flee target
                            size_t closest_carnivore_id = (size_t)-1;
                            float closest_distance_sq = float(data.current_sight_radius[i] * data.current_sight_radius[i] + 1);
                            
                            for (size_t potential_carnivore_id : nearby_carnivores_for_pack_check) {
                                // Calculate distance to this carnivore
                                int dx = data.x[i] - data.x[potential_carnivore_id];
                                int dy = data.y[i] - data.y[potential_carnivore_id];
                                float distance_sq = float(dx * dx + dy * dy);
                                
                                if (distance_sq < closest_distance_sq) {
                                    closest_distance_sq = distance_sq;
                                    closest_carnivore_id = potential_carnivore_id;
                                }
                            }
                            
                            if (closest_carnivore_id != (size_t)-1) {
                                data.state[i] = AIState::FLEEING; 
                                data.target_id[i] = closest_carnivore_id; 
                                continue;
                            }
                        }
                        
                        // Priority 2: Hunt Herbivores
                        auto nearby_herbivores = world.getAnimalsNear(data, data.x[i], data.y[i], static_cast<int>(data.current_sight_radius[i]), AnimalType::HERBIVORE);
                        if (!nearby_herbivores.empty()) {
                            data.state[i] = AIState::CHASING; data.target_id[i] = nearby_herbivores[0]; continue;
                        }

                        // Seek Grass if hungry - Prioritize by ENERGY-TO-DISTANCE RATIO
                        if (data.energy[i] < data.max_energy[i] * OMNIVORE_FOOD_SEEK_THRESHOLD_PERCENTAGE) {
                            // Use best_food_efficiency (energy per distance)
                            float best_food_efficiency = 0.0f; 
                            food_x = -1; food_y = -1; // Reset for this scan
                            int scan_radius = static_cast<int>(data.current_sight_radius[i]);

                            for (int dy = -scan_radius; dy <= scan_radius; ++dy) { 
                                for (int dx = -scan_radius; dx <= scan_radius; ++dx) {
                                    int check_x = data.x[i] + dx; 
                                    int check_y = data.y[i] + dy;
                                    
                                    // Skip if out of bounds or beyond sight radius
                                    if (check_x < 0 || check_x >= world.getWidth() || 
                                        check_y < 0 || check_y >= world.getHeight() || 
                                        dx * dx + dy * dy > scan_radius * scan_radius) {
                                        continue;
                                    }
                                    
                                    const Tile& tile = world.getTile(check_x, check_y);
                                    // Ensure there is actually consumable food
                                    if (tile.resource_type && tile.getConsumableAmount() > 0.0f) {
                                        float potential_energy = tile.resource_amount * tile.resource_type->nutritional_value;
                                        
                                        // Calculate distance (minimum 1 to avoid division by zero)
                                        float distance = std::max(1.0f, std::sqrt(float(dx * dx + dy * dy)));
                                        
                                        // Calculate efficiency: energy per unit distance
                                        // Add small bonus for very close food to prefer nearby resources
                                        float efficiency = potential_energy / distance;
                                        if (distance <= 2.0f) efficiency *= 1.5f; // 50% bonus for adjacent/very close food
                                        
                                        if (efficiency > best_food_efficiency) {
                                            best_food_efficiency = efficiency;
                                            food_x = check_x; 
                                            food_y = check_y;
                                        }
                                    }
                                }
                            }
                            
                            // Only seek food if we found something reasonably efficient
                            if (best_food_efficiency > 0.0f) {
                                data.state[i] = AIState::SEEKING_FOOD; 
                                data.target_x[i] = food_x; 
                                data.target_y[i] = food_y; 
                                continue;
                            }
                        }

                        // Priority 4: Pack hunt Carnivores (if we have enough allies)
                        auto nearby_carnivores_for_hunt = world.getAnimalsNear(data, data.x[i], data.y[i], static_cast<int>(data.current_sight_radius[i]), AnimalType::CARNIVORE);
                        if (!nearby_carnivores_for_hunt.empty()) {
                            size_t potential_carnivore_target_id = nearby_carnivores_for_hunt[0];
                            // Validate target is alive and within reasonable range
                            if (potential_carnivore_target_id != (size_t)-1 && 
                                potential_carnivore_target_id < data.getEntityCount() && 
                                data.is_alive[potential_carnivore_target_id]) {
                                
                                // Check if we have enough allies near OURSELVES (the hunter) for pack hunting
                                // Use a smaller radius for pack coordination (allies need to be close)
                                auto allies_near_hunter = world.getAnimalsNear(data, data.x[i], data.y[i], 3, AnimalType::OMNIVORE);
                                if(allies_near_hunter.size() >= OMNIVORE_PACK_HUNT_SIZE) {
                                    data.state[i] = AIState::PACK_HUNTING; 
                                    data.target_id[i] = potential_carnivore_target_id; 
                                    continue;
                                }
                            }
                        }
                        data.state[i] = AIState::WANDERING;
                    } break;
                    default: data.state[i] = AIState::WANDERING; break;
                }
        }
    }
}
