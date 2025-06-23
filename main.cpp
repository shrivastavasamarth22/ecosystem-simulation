#include "includes/World.h"
#include "includes/GraphicsRenderer.h" // <-- Include our new renderer
#include "includes/AnimalConfig.h"     // Needed for constants
#include "includes/EntityManager.h"    // Needed for EntityManager access (later)

#include <iostream>
#include <thread>
#include <chrono>
#include <string> // Needed for window title
#include <SFML/System.hpp> // Needed for sf::Clock and sf::Time

int main() {
    // ... (simulation parameters) ...
    const int WORLD_WIDTH = 50;
    const int WORLD_HEIGHT = 25;
    const int SPATIAL_GRID_CELL_SIZE = 15;
    const int INITIAL_HERBIVORES = 25;
    const int INITIAL_OMNIVORES = 15;
    const int INITIAL_CARNIVORES = 8;
    const int SIMULATION_SPEED_MS = 500;
    const int MAX_TURNS = 2000;
    const int TILE_SIZE_PIXELS = 20;
    const std::string WINDOW_TITLE = "Ecosystem Simulation";


    // --- Simulation Setup ---
    World world(WORLD_WIDTH, WORLD_HEIGHT, SPATIAL_GRID_CELL_SIZE);
    world.init(INITIAL_HERBIVORES, INITIAL_CARNIVORES, INITIAL_OMNIVORES);

    // --- Graphics Setup ---
    GraphicsRenderer renderer;
    renderer.init(WORLD_WIDTH, WORLD_HEIGHT, TILE_SIZE_PIXELS, WINDOW_TITLE);

    // --- Simulation Timing ---
    sf::Clock simulationClock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    sf::Time timePerUpdate = sf::milliseconds(SIMULATION_SPEED_MS);


    std::cout << "Starting Intelligent Agent Simulation..." << std::endl;

    // --- Main SFML Window Loop ---
    while (renderer.isOpen()) {
        // Handle window events (closing, input)
        renderer.handleEvents();

        // Update simulation based on elapsed time
        timeSinceLastUpdate += simulationClock.restart();
        // Use a while loop to catch up if rendering is slow
        while (timeSinceLastUpdate > timePerUpdate) {
            timeSinceLastUpdate -= timePerUpdate;

            // --- Run one simulation turn ---
             if (!world.isEcosystemCollapsed() && world.getEntityManager().getEntityCount() > 0 && world.getEntityManager().is_alive.size() > 0 )
             {
                 world.update();
             }

            // Check for end conditions
            if (world.isEcosystemCollapsed() || world.getEntityManager().getEntityCount() == 0) {
                std::cout << "Ecosystem collapsed or empty. Simulation finished." << std::endl;
                // You can choose to close the window automatically or leave it open
                // renderer.getWindow().close(); // Close the window
                break; // Exit the simulation update loop
            }
             if (world.getTurnCount() >= MAX_TURNS) {
                 std::cout << "Simulation reached max turns. Finished." << std::endl;
                 // renderer.getWindow().close(); // Close the window
                 break; // Exit the simulation update loop
             }

        } // End while (timeSinceLastUpdate > timePerUpdate)

        // --- Drawing Phase ---
        renderer.clear(sf::Color(100, 149, 237)); // Clear with a sky blue color

        // --- NEW: Draw the world grid ---
        renderer.drawWorld(world); // <-- Call the new drawing function

        // Draw entities and UI here in later phases
        // renderer.drawEntities(world.getEntityManager()); // Phase 3
        // renderer.drawUI(world); // Phase 5

        renderer.display(); // Display the drawn frame

    } // End while (renderer.isOpen())


    // --- Simulation End ---
    std::cout << "SFML Window closed. Exiting simulation." << std::endl;

    return 0;
}