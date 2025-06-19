#ifndef CARNIVORE_H
#define CARNIVORE_H

#include "Animal.h"

class Carnivore : public Animal {
public:
    // Constructor: Creates a Carnivore at a specific location.
    Carnivore(int x, int y);

    // --- Overridden AI Functions ---

    // Decides whether to chase prey or flee from an Omnivore pack.
    void updateAI(World& world) override;

    // Executes the action: chases, attacks, or flees.
    void act(World& world) override;

    // Creates a new Carnivore if energy is sufficient.
    std::unique_ptr<Animal> reproduce() override;
};

#endif // CARNIVORE_H