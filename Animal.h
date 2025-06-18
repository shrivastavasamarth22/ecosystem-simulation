#ifndef ANIMAL_H
#define ANIMAL_H

#include <memory>

// Forward declaration to avoid circular include with World.
class World;

class Animal {
  protected:
    int x, y;
    int energy;
    int age;
    bool is_alive;
    char symbol;

  public:
    // Constructor 
    Animal(int start_x, int start_y, int start_energy, char sym);

    // Virtual destructor
    virtual ~Animal() = default;

    // Pure virtual functions
    virtual void update(World& world) = 0;
    virtual std::unique_ptr<Animal> reproduce() = 0;


    // Common functions for all animals
    void move(const World& world);
    void gainEnergy(int amount);
    bool isDead() const;
    void kill();

    // Getters
    int getX() const {
      return x;
    };
    int getY() const {
      return y;
    };
    int getSymbol() const {
      return symbol;
    };

};

#endif // ANIMAL_H