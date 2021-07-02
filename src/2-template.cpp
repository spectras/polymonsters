/** Compile-time polymorphic monsters
 *
 * The type of each monster will be resolved by the compiler.
 * Of course this implies they are known at compile time.
 *
 * In practice this happens frequently in real code: there are
 * multiple subtypes of a class for various use cases, but in each
 * use we know at compile time which we want.
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

// There is no base class, each monster is totally independent.

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

// Note how the implementation is exactly identical to the inheritance case.
// The only change it the template<> declaration before the function.
template <typename Monster>
int fight(Monster& monster, Weapon weapon, int attempts = 5)
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
