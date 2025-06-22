#include "Tile.h"
#include "Animal.h"

int Tile::getConsumableAmount() const {
    // You can consume up to the current amount, up to a certain limit per turn if needed
    // For now, allow just consuming up to the current amount
    return resource_amount;
}

int Tile::consume(int amount_requested) {
    int amount_consumed = std::min(amount_requested, resource_amount);
    resource_amount -= amount_consumed;

    return amount_consumed;
}

void Tile::regrow() {
    if (resource_type) { // Only regrow if there's a resource type on this tile
        resource_amount += resource_type->regrowth_rate;
        resource_amount = std::min(resource_amount, resource_type->max_amount); // Don't exceed max
    }
}

char Tile::getSymbol() const {
    if (!resource_type || resource_amount <= 0) {
        return '.'; // Draw empty tile if no resource or amount is zero
    }

    // Draw symbol based on resource type and potentially amount
    // We can add tiers later (e.g., different symbols for low, medium, high grass)
    // For now, just show the resource symbol if any is present
    return resource_type->symbol;
}