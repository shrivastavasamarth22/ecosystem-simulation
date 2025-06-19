#include "World.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
  const int WORLD_WIDTH = 40;
  const int WORLD_HEIGHT = 20;
  const int INITIAL_HERBIVORES = 25;
  const int INITIAL_CARNIVORES = 5;
  const int INITIAL_OMNIVORES = 10; // Add omnivores
  const int SIMULATION_SPEED_MS = 500;

  World world(WORLD_WIDTH, WORLD_HEIGHT);
  world.init(INITIAL_HERBIVORES, INITIAL_CARNIVORES, INITIAL_OMNIVORES);

  int max_turns = 1000;
  for (int i = 0; i < max_turns; ++i) {
    world.draw();
    
    if (world.isEcosystemCollapsed()) {
      std::cout << "The ecosystem has collapsed. Simulation over" << std::endl;
      break;
    }
    
    world.update();

    std::this_thread::sleep_for(std::chrono::milliseconds(SIMULATION_SPEED_MS));
  }

  world.draw();
  if (!world.isEcosystemCollapsed()) {
    std::cout << "Simulation reached max turns." << std::endl; 
  }

  return 0;

}