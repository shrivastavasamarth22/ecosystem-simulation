#ifndef TILE_H
#define TILE_H

#include "Resource.h"
#include <vector>
#include <string>

// Forward declaration for Animal to avoid include loop
class Animal;

struct Tile {
    // --- Animal Information ---
    // Don't store the Animal* here directly because the World owns them.
    // Instead, World will map Animal positions to update drawing/interaction.

    // --- Resource Information ---

    const ResourceType* resource_type = nullptr; // Pointer to the type of resource on this tile
    int resource_amount = 0; // Current amount of resource on this tile

    int regrowth_timer = 0; // How many turns until regrowth

    // Default constructor
    Tile() = default;

    // Constructor to set resource type and initial amount
    Tile(const ResourceType* type, int initial_amount) : resource_type(type), resource_amount(initial_amount) {}

    // --- Resource Managememt Methods ---
    // How much can be consumed this turn
    int getConsumableAmount() const;

    // Consume a given amount, return the actual amount consumed
    int consume(int amount_requested);

    // Regrow the resource by its regrowth rate
    void regrow();

    // Get the symbol for drawing this tile (based on resource state)
    char getSymbol() const;
};

#endif // TILE_H