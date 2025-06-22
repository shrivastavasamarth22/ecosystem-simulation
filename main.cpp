#include "includes/World.h"
#include "includes/GraphicsRenderer.h" // <-- Include our new renderer
#include "includes/AnimalConfig.h"     // Needed for constants
#include "includes/EntityManager.h"    // Needed for EntityManager access (later)

#include <iostream>
#include <thread>
#include <chrono>
#include <string> // Needed for window title

int main() {
    // --- Simulation Parameters ---
    const int WORLD_WIDTH = 50;
    const int WORLD_HEIGHT = 25;
    const int SPATIAL_GRID_CELL_SIZE = 15;

    const int INITIAL_HERBIVORES = 25;
    const int INITIAL_OMNIVORES = 15;
    const int INITIAL_CARNIVORES = 8;

    // Simulation speed control
    const int SIMULATION_SPEED_MS = 150;
    const int MAX_TURNS = 2000;

    // --- NEW: Rendering Parameters ---
    const int TILE_SIZE_PIXELS = 20; // Size of each grid tile in pixels
    const std::string WINDOW_TITLE = "Ecosystem Simulation";


    // --- Simulation Setup ---
    World world(WORLD_WIDTH, WORLD_HEIGHT, SPATIAL_GRID_CELL_SIZE);
    world.init(INITIAL_HERBIVORES, INITIAL_CARNIVORES, INITIAL_OMNIVORES);

    // --- NEW: Graphics Setup ---
    GraphicsRenderer renderer;
    renderer.init(WORLD_WIDTH, WORLD_HEIGHT, TILE_SIZE_PIXELS, WINDOW_TITLE);

    // --- Simulation Timing ---
    // Use an SFML clock or similar to manage the simulation update rate
    sf::Clock simulationClock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    sf::Time timePerUpdate = sf::milliseconds(SIMULATION_SPEED_MS);


    std::cout << "Starting Intelligent Agent Simulation..." << std::endl;
    // No longer need the initial pause unless desired before the window appears


    // --- Main SFML Window Loop ---
    while (renderer.isOpen()) {
        // Handle window events (closing, input)
        renderer.handleEvents();

        // Update simulation based on elapsed time
        timeSinceLastUpdate += simulationClock.restart();
        while (timeSinceLastUpdate > timePerUpdate) {
            timeSinceLastUpdate -= timePerUpdate;

            // --- Run one simulation turn ---
                if (!world.isEcosystemCollapsed() && world.getEntityManager().getEntityCount() > 0 && world.getEntityManager().is_alive.size() > 0 ) // Add basic checks for entity manager state
                {
                    world.update();
                }


            // Check for end conditions (optional, can let it run until window closes)
            if (world.isEcosystemCollapsed() || world.getEntityManager().getEntityCount() == 0) {
                std::cout << "Ecosystem collapsed or empty. Simulation finished." << std::endl;
                // Optionally keep window open for a bit or close it
                // renderer.getWindow().close(); // Close the window to end the app
                break; // Exit the while(timeSinceLastUpdate > timePerUpdate) loop
            }
                if (world.getTurnCount() >= MAX_TURNS) {
                    std::cout << "Simulation reached max turns. Finished." << std::endl;
                    // renderer.getWindow().close(); // Close the window
                    break; // Exit the while(timeSinceLastUpdate > timePerUpdate) loop
                }

        } // End while (timeSinceLastUpdate > timePerUpdate)

        // --- Drawing Phase ---
        renderer.clear(sf::Color(100, 149, 237)); // Clear with a sky blue color

        // Draw the world (tiles, animals, UI) here in later phases
        // renderer.drawWorld(world); // Phase 2
        // renderer.drawEntities(world.getEntityManager()); // Phase 3
        // renderer.drawUI(world); // Phase 5

        renderer.display(); // Display the drawn frame

    } // End while (renderer.isOpen())


    // --- Simulation End ---
    std::cout << "SFML Window closed. Exiting simulation." << std::endl;
    // The GraphicsRenderer destructor will be called automatically here.

    return 0;
}