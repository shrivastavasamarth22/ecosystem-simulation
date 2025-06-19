#include "World.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    // --- Simulation Parameters ---
    // You can tweak these values to see how the ecosystem changes.
    const int WORLD_WIDTH = 50;
    const int WORLD_HEIGHT = 25;

    // A higher number of herbivores provides a stable food source.
    const int INITIAL_HERBIVORES = 40;
    // Omnivores are versatile but need numbers to be effective hunters.
    const int INITIAL_OMNIVORES = 15;
    // Carnivores are powerful but few in number, as they require a lot of food.
    const int INITIAL_CARNIVORES = 5;

    // Controls the speed of the simulation (milliseconds per turn).
    const int SIMULATION_SPEED_MS = 150;
    const int MAX_TURNS = 2000; // A higher turn limit for the more complex simulation.


    // --- Simulation Setup ---
    World world(WORLD_WIDTH, WORLD_HEIGHT);
    world.init(INITIAL_HERBIVORES, INITIAL_CARNIVORES, INITIAL_OMNIVORES);

    std::cout << "Starting Intelligent Agent Simulation..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Pause to read start message


    // --- Main Simulation Loop ---
    for (int i = 0; i < MAX_TURNS; ++i) {
        // Redraw the world state at the beginning of each turn.
        world.draw();

        // Check for end conditions.
        if (world.isEcosystemCollapsed()) {
            std::cout << "An entire species has been wiped out. The ecosystem has collapsed." << std::endl;
            break;
        }

        // Run one turn of the simulation.
        world.update();

        // Pause to make the simulation viewable.
        std::this_thread::sleep_for(std::chrono::milliseconds(SIMULATION_SPEED_MS));
    }

    // --- Simulation End ---
    world.draw(); // Draw the final state of the world.
    if (!world.isEcosystemCollapsed()) {
        std::cout << "Simulation reached the maximum turn limit of " << MAX_TURNS << "." << std::endl;
    }

    return 0;
}