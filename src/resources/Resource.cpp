#include "resources/Resource.h"

const ResourceType RESOURCE_GRASS = {
    "Grass",
    10.0f, // Max amount
    1.0f, // Regrowth rate per turn
    1.0f, // Nutritional value
    ',' // Symbol to draw on the grid
};

const ResourceType RESOURCE_BERRIES = {
    "Berries",
    5.0f, // Max amount
    0.5f, // Regrowth rate per turn
    3.0f, // Nutritional value
    'b' // Symbol to draw on the grid
};