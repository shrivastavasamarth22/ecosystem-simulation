#include "World.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
  const int WORLD_WIDTH = 40;
  const int WORLD_HEIGHT = 20;
  const int INITIAL_HERBIVORES = 50;
  const int INITIAL_CARNIVORES = 20;
  const int SIMULATION_SPEED_MS = 200; // 200ms per turn

  World world(WORLD_WIDTH, WORLD_HEIGHT);
  world.init(INITIAL_HERBIVORES, INITIAL_CARNIVORES);

  int max_turns = 500;
  for (int i = 0; i < max_turns; ++i) {
    world.draw();
    
    if (world.isExtinct()) {
      std::cout << "A population has gone extinct. Simulation over." << std::endl;
      break;
    }
    
    world.update();

    std::this_thread::sleep_for(std::chrono::milliseconds(SIMULATION_SPEED_MS));
  }
  
  if (!world.isExtinct()) {
    std::cout << "Simulation reached max turns." << std::endl;
  }

  return 0;
}