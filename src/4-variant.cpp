/** So far, we used templates and compile-time polymorphism.
 *
 * Those are very powerful, but require that the type of the monster be known
 * at compile time.
 * What if we do not? For instance, if we loaded monsters from a data file?
 *
 * We can use std::variant to make a collection of types we can pick from
 * at runtime.
 */

#include <catch.hpp>
#include <fmt/core.h>
#include <iostream>
#include <string>
#include <variant>
#include "health.h"
using fmt::format;

// NO CHANGE AT ALL UNTIL YOU SEE "CHANGES START HERE"


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


template <typename DefaultMonster>
bool dead(const DefaultMonster & monster) { return !monster.health; }
bool dead(const Ghost &) { return false; }

// ===========================================================================
// The actual fighting that uses monsters

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






// CHANGES START HERE

// ===========================================================================
// Runtime monsters

/* Note how we did not touch anything so far.
 * What we will do is not really a separate idea, but an additional feature
 * that can be added **on top** of any template-based approach.
 * 
 * I chose to keep the functional one, but it would work with 2-template.cpp too.
 */

// First, We create a type named Monster that can be one of a finite set of
// monster types. This set cannot be modified, so to add new monsters,
// this line must be updated.
// Then we add functions for this type:

using Monster = std::variant<Wolf, Firelord, Ghost>;

Comment hit(Monster& monster, Weapon weapon, HealthPoints damage)
{
    return std::visit([&](auto & value) { return hit(value, weapon, damage); }, monster);
}

bool dead(const Monster & monster) {
    return std::visit([](const auto & value) { return dead(value); }, monster);
}


/** Explanation
 * The std::visit function will test all possible types and call the lambda.
 * It is equivalent to this:
 */
#if 0
bool dead(Monster& monster)
{
    if (auto wolf = std::get_if<Wolf>(&monster)) { return dead<Wolf>(*wolf); }
    if (auto firelord = std::get_if<Firelord>(&monster)) { return dead<Firelord>(*firelord); }
    if (auto ghost = std::get_if<Ghost>(&monster)) { return dead<Ghost>(*ghost); }
    assert(false);
}
#endif

// THAT'S ALL, we now have a Monster type that can dynamically store different
// classes of monsters.

// Note that we did not need to implement fight(). The one we already had will
// work fine, and use our hit() and dead() implementations for our variant.







// ===========================================================================
// Exercising the code

TEST_CASE("Wilhelm the wolf dies in 3 attempts") {
    Monster wilhelm = Wolf{"Wilhelm", HealthPoints{100}};

    const auto attempts = fight(wilhelm, Weapon::Stick);

    CHECK(attempts == 3);
    CHECK(dead(std::get<Wolf>(wilhelm)));
}

TEST_CASE("Gerhard the firelord dies in 5 attempts when using sticks") {
    Monster gerhard = Firelord{"Gerhard", HealthPoints{100}};

    const auto attempts = fight(gerhard, Weapon::Stick);

    CHECK(attempts == 5);
    CHECK(dead(std::get<Firelord>(gerhard)));
}

TEST_CASE("Ghosts cannot be killed") {
    Monster astrid = Ghost();
    const auto attempts = fight(astrid, Weapon::Arrow);

    CHECK(attempts == 5);
    CHECK(!dead(std::get<Ghost>(astrid)));
}
