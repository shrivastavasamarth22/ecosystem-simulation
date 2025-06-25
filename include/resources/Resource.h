#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>

// Struct to define a type of resource (like Grass)
struct ResourceType {
    std::string name;
    float max_amount; // Maximum amount of resource on a tile
    float regrowth_rate; // Amount that regrows per turn
    float nutritional_value; // Nutritional value of the resource
    char symbol; // Symbol to draw on the grid
};

extern const ResourceType RESOURCE_GRASS;
extern const ResourceType RESOURCE_BERRIES;

#endif // RESOURCE_H
