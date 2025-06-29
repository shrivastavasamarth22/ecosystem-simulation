#include "resources/Tile.h"
#include "resources/Biome.h" // Include the full Biome definition
#include "resources/Terrain.h" // Include the full Terrain definition

float Tile::getConsumableAmount() const {
    // You can consume up to the current amount, up to a certain limit per turn if needed
    // For now, allow just consuming up to the current amount
    return resource_amount;
}

float Tile::consume(float amount_requested) {
    float amount_consumed = std::min(amount_requested, resource_amount);
    resource_amount -= amount_consumed;

    return amount_consumed;
}

void Tile::setBiome(const BiomeType* type) {
    biome_type = type;
}

const BiomeType* Tile::getBiome() const {
    return biome_type;
}

void Tile::setResource(const ResourceType* type, float amount) {
    resource_type = type;
    resource_amount = amount;
}

void Tile::setTerrain(const TerrainType* type) {
    terrain_type = type;
}

const TerrainType* Tile::getTerrain() const {
    return terrain_type;
}

bool Tile::canMove(AnimalType animal_type) const {
    if (!terrain_type) {
        return true; // No terrain restriction
    }
    
    // If allowed_types is empty, all animals can move
    if (terrain_type->allowed_types.empty()) {
        return true;
    }
    
    // Check if this animal type is in the allowed set
    return terrain_type->allowed_types.find(animal_type) != terrain_type->allowed_types.end();
}

float Tile::getSpeedModifier() const {
    return terrain_type ? terrain_type->speed_modifier : 1.0f;
}

float Tile::getSightModifier() const {
    return terrain_type ? terrain_type->sight_modifier : 1.0f;
}

void Tile::regrow() {
    if (resource_type) { // Only regrow if there's a resource type on this tile
        resource_amount += resource_type->regrowth_rate;
        resource_amount = std::min(resource_amount, resource_type->max_amount); // Don't exceed max
    }
}

char Tile::getSymbol() const {
    if (!resource_type || resource_amount <= 0.0f) {
        return '.'; // Draw empty tile if no resource or amount is zero
    }

    // Draw symbol based on resource type and potentially amount
    // We can add tiers later (e.g., different symbols for low, medium, high grass)
    // For now, just show the resource symbol if any is present
    return resource_type->symbol;
}