#ifndef TILE_H
#define TILE_H

#include "Resource.h"
#include <vector>
#include <string>

// Forward declaration for Animal to avoid include loop
class Animal;
// NEW: Forward declare BiomeType to avoid include loop with Biome.h
struct BiomeType;

struct Tile {
    // --- Animal Information ---
    // Don't store the Animal* here directly because the World owns them.
    // Instead, World will map Animal positions to update drawing/interaction.

    // --- Resource Information ---

    const ResourceType* resource_type = nullptr; // Pointer to the type of resource on this tile
    float resource_amount = 0.0f; // Current amount of resource on this tile

    // --- NEW: Biome Information ---
    const BiomeType* biome_type = nullptr;

    int regrowth_timer = 0; // How many turns until regrowth

    // Default constructor
    Tile() = default;

    // Constructor to set resource type and initial amount
    Tile(const ResourceType* type, float initial_amount) : resource_type(type), resource_amount(initial_amount) {}

    // --- NEW: Biome Management Methods ---
    void setBiome(const BiomeType* type);
    const BiomeType* getBiome() const;

    // --- NEW: Resource Management ---
    void setResource(const ResourceType* type, float amount);

    // --- Resource Managememt Methods ---
    // How much can be consumed this turn
    float getConsumableAmount() const;

    // Consume a given amount, return the actual amount consumed
    float consume(float amount_requested);

    // Regrow the resource by its regrowth rate
    void regrow();

    // Get the symbol for drawing this tile (based on resource state)
    char getSymbol() const;
};

#endif // TILE_H
