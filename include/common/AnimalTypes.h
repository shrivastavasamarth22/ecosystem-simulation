#ifndef ANIMAL_TYPES_H
#define ANIMAL_TYPES_H

enum class AnimalType {
    HERBIVORE,
    CARNIVORE,
    OMNIVORE,
};

enum class AIState {
    WANDERING,
    FLEEING,
    CHASING,
    PACK_HUNTING,
    HERDING,
    SEEKING_FOOD,
};

#endif // ANIMAL_TYPES_H
