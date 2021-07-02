/** C++20 concepts
 *
 * The main drawback of template-based monsters is that you can
 * try to fight() any class.
 * If the class does not actually implement the expected methods,
 * you will get compile errors when fight() tries to call them.
 *
 * This works, but it is ugly and not always easy to debug.
 *
 * C++20 adds concepts that make it possible to clearly specify
 * what you expect from a class.
 * 
 * This example adds a Monster concept to verify. Thanks to that, if you
 * try to pass a wrong class, you get a nice "Monster concept not satisfied".
 * 
 * There are only 2 changes in the file. Search for "SEE HERE".
 */

#include <catch.hpp>
#include <fmt/core.h>
#include <concepts>
#include <iostream>
#include <string>
#include "health.h"
using fmt::format;

// ===========================================================================
// Definitions

using Name = std::string;
using Comment = std::string;
enum class Weapon { Stick, Arrow, Fireball };

/** SEE HERE
 * This defines what a class needs to be considered a valid Monster:
 * It reads like this:
 * - Given an instance of the class named `obj`, a weapon and some healthpoints,
 * - Then calling obj.hit should be valid, and its result can be used as a Comment.
 * - And calling obj.dead should be valid, and its result can be used as a bool.
 */
template <typename T>
concept Monster = requires(T obj, Weapon weapon, HealthPoints hp) {
    { obj.hit(weapon, hp) } -> std::convertible_to<Comment>;
    { obj.dead() } -> std::convertible_to<bool>;
};

// ===========================================================================
// Monsters

class Wolf {
    Name            name_;
    HealthPoints    health_;
public:
    Wolf(Name name, HealthPoints hp)
        : name_(std::move(name)), health_(hp) {}

    Comment hit(Weapon, HealthPoints damage)
    {
        health_ = max(HealthPoints{0}, health_ - damage);
        return format("{} the wolf growls as it takes {} damage from the hit.", name_, damage.value);
    }

    bool dead() const { return !health_; }
};

class Firelord {
    Name            name_;
    HealthPoints    health_;
public:
    Firelord(Name name, HealthPoints hp)
        : name_(std::move(name)), health_(hp) {}

    Comment hit(Weapon weapon, HealthPoints damage)
    {
        switch (weapon) {
        case Weapon::Stick:
            health_ = max(HealthPoints{0}, health_ - damage / 2);
            return format("{} the Firelord resists wooden stick and only takes {} damage.",
                name_, (damage / 2).value);
        case Weapon::Fireball:
            return format("{} the Firelord is immune to fireballs. He laughs at you.");
        default:
            health_ = max(HealthPoints{0}, health_ - damage);
            return format("{} the Firelord roars {} damage from the hit.", name_, damage.value);
        }
    }

    bool dead() const { return !health_; }
};

class Ghost {
public:
    Comment hit(Weapon, HealthPoints)
    {
        return "Ghosts are immortal. You are doomed.";
    }

    bool dead() const { return true and false; }
};

// ===========================================================================
// The actual fighting that uses monsters

/** SEE HERE
 * In c++20, the template <typename T> is no longer required, simply using
 * the keyword `auto` as parameter makes it a template.
 * 
 * We also say it is not any auto, but it has to validate the Monster concept.
 */
int fight(Monster auto & monster, Weapon weapon, int attempts = 5)
{
    for (int attempt = 1; attempt <= attempts; ++attempt) {
        std::cout <<monster.hit(weapon, HealthPoints{40}) <<'\n';
        if (monster.dead())
            return attempt;
    }
    return attempts;
}

// ===========================================================================
// Exercising the code

TEST_CASE("Wilhelm the wolf dies in 3 attempts") {
    auto wilhelm = Wolf("Wilhelm", HealthPoints{100});

    const auto attempts = fight(wilhelm, Weapon::Stick);

    CHECK(attempts == 3);
    CHECK(wilhelm.dead());
}

TEST_CASE("Gerhard the firelord dies in 5 attempts when using sticks") {
    auto gerhard = Firelord("Gerhard", HealthPoints{100});

    const auto attempts = fight(gerhard, Weapon::Stick);

    CHECK(attempts == 5);
    CHECK(gerhard.dead());
}

TEST_CASE("Ghosts cannot be killed") {
    auto astrid = Ghost();
    const auto attempts = fight(astrid, Weapon::Arrow);

    CHECK(attempts == 5);
    CHECK(!astrid.dead());
}
