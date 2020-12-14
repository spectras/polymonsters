#include <catch.hpp>
#include <fmt/core.h>
#include <iostream>
#include <string>
#include <variant>
#include "health.h"
using fmt::format;

// ===========================================================================

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


bool dead(const Wolf & wolf) { return !wolf.health; }
bool dead(const Firelord & firelord) { return !firelord.health; }
bool dead(const Ghost &) { return false; }

// ===========================================================================

using Monster = std::variant<Wolf, Firelord, Ghosts>;

Comment hit(Monster& monster, Weapon weapon, HealthPoints damage)
{
    return std::visit([&](auto & value) { return hit(value, weapon, damage); }, monster);
}

bool dead(const Monster & monster) {
    return std::visit([](const auto & value) { return dead(value); }, monster);
}

// ===========================================================================

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

TEST_CASE("Wilhelm the wolf dies in 3 attempts") {
    Monster wilhelm = Wolf{"Wilhelm", HealthPoints{100}};

    const auto attempts = fight(wilhelm, Weapon::Stick);

    CHECK(attempts == 3);
    CHECK(dead(wilhelm));
}

TEST_CASE("Gerhard the firelord dies in 5 attempts when using sticks") {
    Monster gerhard = Firelord{"Gerhard", HealthPoints{100}};

    const auto attempts = fight(gerhard, Weapon::Stick);

    CHECK(attempts == 5);
    CHECK(dead(gerhard));
}

TEST_CASE("Ghosts cannot be killed") {
    Monster astrid = Ghost();
    const auto attempts = fight(astrid, Weapon::Arrow);

    CHECK(attempts == 5);
    CHECK(!dead(astrid));
}
