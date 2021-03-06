/** Functional approach to fighting.
 *
 * Notice how we shifted the focus:
 *    => the program is no longer about monsters that we fight.
 *    => the program is now is now about fighting that involves monster.
 *
 * We give more importance to what happens than to what exists.
 */
#include <catch.hpp>
#include <fmt/core.h>
#include <iostream>
#include <string>
#include "health.h"
using fmt::format;

// ===========================================================================
// Definitions

using Name = std::string;
using Comment = std::string;
enum class Weapon { Stick, Arrow, Fireball };

struct Wolf {
    Name            name;
    HealthPoints    health;
};

struct Firelord {
    Name            name;
    HealthPoints    health;
};

struct Ghost {};

// ===========================================================================
// Functions

// The hit function, with its different implementations depending on what gets hit

Comment hit(Wolf & wolf, Weapon, HealthPoints damage)
{
    wolf.health = max(HealthPoints{0}, wolf.health - damage);
    return format("{} the wolf growls as it takes {} damage from the hit.", wolf.name, damage.value);
}

Comment hit(Firelord & firelord, Weapon weapon, HealthPoints damage)
{
    switch (weapon) {
    case Weapon::Stick:
        firelord.health = max(HealthPoints{0}, firelord.health - damage / 2);
        return format("{} the Firelord resists wooden stick and only takes {} damage.",
                      firelord.name, (damage / 2).value);
    case Weapon::Fireball:
        return format("{} the Firelord is immune to fireballs. He laughs at you.");
    default:
        firelord.health = max(HealthPoints{0}, firelord.health - damage);
        return format("{} the Firelord roars {} damage from the hit.", firelord.name, damage.value);
    }
}

Comment hit(Ghost&, Weapon, HealthPoints)
{
    return "Ghosts are immortal. You are doomed.";
}


// The dead function, with its different implementations depending on what we test
// Most monsters behave the same, so we make a template for the usual behavior.
template <typename DefaultMonster>
bool dead(const DefaultMonster & monster) { return !monster.health; }

// ...and a special case for ghosts.
bool dead(const Ghost &) { return false; }


// ===========================================================================
// The actual fighting that uses monsters

// Again the implementation does not change.
// We just reflect the importance of what happens rather than monsters.
// So we replace monster.hit(...) with hit(monster, ....)
template <typename Monster>
int fight(Monster& monster, Weapon weapon, int attempts = 5)
{
    for (int attempt = 1; attempt <= attempts; ++attempt) {
        std::cout <<hit(monster, weapon, HealthPoints{40}) <<'\n';
        if (dead(monster))
            return attempt;
    }
    return attempts;
}

// ===========================================================================
// Exercising the code

TEST_CASE("Wilhelm the wolf dies in 3 attempts") {
    auto wilhelm = Wolf{"Wilhelm", HealthPoints{100}};

    const auto attempts = fight(wilhelm, Weapon::Stick);

    CHECK(attempts == 3);
    CHECK(dead(wilhelm));
}

TEST_CASE("Gerhard the firelord dies in 5 attempts when using sticks") {
    auto gerhard = Firelord{"Gerhard", HealthPoints{100}};

    const auto attempts = fight(gerhard, Weapon::Stick);

    CHECK(attempts == 5);
    CHECK(dead(gerhard));
}

TEST_CASE("Ghosts cannot be killed") {
    auto astrid = Ghost();
    const auto attempts = fight(astrid, Weapon::Arrow);

    CHECK(attempts == 5);
    CHECK(!dead(astrid));
}
