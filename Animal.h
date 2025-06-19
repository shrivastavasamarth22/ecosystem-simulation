#ifndef ANIMAL_H
#define ANIMAL_H

#include <memory>

class World; 

enum class AIState {
    WANDERING,
    FLEEING,
    CHASING,
    PACK_HUNTING
};

class Animal {
protected:
    int x, y;
    int age;
    bool is_alive;
    char symbol;

    int health;
    int max_health;
    int damage;
    int sight_radius;
    int speed;
    int energy;
    AIState current_state;
    Animal* target;

public:
    Animal(int start_x, int start_y, char sym, int max_hp, int dmg, int sight, int spd, int nrg);
    virtual ~Animal() = default;

    virtual void updateAI(World& world) = 0;
    virtual void act(World& world) = 0;
    virtual std::unique_ptr<Animal> reproduce() = 0;

    void takeDamage(int amount);
    void postTurnUpdate();
    bool isDead() const;
    void kill();

    // --- MODIFIED FUNCTION SIGNATURES ---
    // The movement functions now need to know about the world to check boundaries.
    void moveTowards(const World& world, int target_x, int target_y);
    void moveAwayFrom(const World& world, int target_x, int target_y);
    void moveRandom(const World& world);

    // Getters
    int getX() const { return x; }
    int getY() const { return y; }
    int getSightRadius() const { return sight_radius; }
    char getSymbol() const { return symbol; }
};

#endif // ANIMAL_H