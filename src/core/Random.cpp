#include "core/Random.h"
#include <random> // Needed for the definition

// Define the global random number generator.
// This line should appear in ONLY ONE .cpp file in the entire project.
std::mt19937 rng(std::random_device{}());