#include <variant>
#include "health.h"

// ===========================================================================

using Name = const char *;
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


constexpr Wolf hit(Wolf wolf, Weapon, HealthPoints damage)
{
    return { wolf.name, max(HealthPoints{0}, wolf.health - damage) };
}

constexpr Firelord hit(Firelord firelord, Weapon weapon, HealthPoints damage)
{
    switch (weapon) {
    case Weapon::Stick:
        firelord.health = max(HealthPoints{0}, firelord.health - damage / 2);
        break;
    case Weapon::Fireball:
        break;
    default:
        firelord.health = max(HealthPoints{0}, firelord.health - damage);
    }
    return firelord;
}

constexpr Ghost hit(Ghost ghost, Weapon, HealthPoints) { return ghost; }


constexpr bool dead(const Wolf & wolf) { return !wolf.health; }
constexpr bool dead(const Firelord & firelord) { return !firelord.health; }
constexpr bool dead(const Ghost &) { return false; }

// ===========================================================================

using Monster = std::variant<Wolf, Firelord, Ghost>;

constexpr Monster hit(Monster monster, Weapon weapon, HealthPoints damage)
{
    return std::visit([&](auto & value) -> Monster { return hit(value, weapon, damage); }, monster);
}

constexpr bool dead(const Monster & monster) {
    return std::visit([](const auto & value) { return dead(value); }, monster);
}

// ===========================================================================

struct FightResult { Monster monster; int attempts; };

constexpr FightResult fight(Monster monster, Weapon weapon, int attempts = 5)
{
    for (int attempt = 1; attempt <= attempts; ++attempt) {
        monster = hit(monster, weapon, HealthPoints{40});
        if (dead(monster))
            return { monster, attempt };
    }
    return { monster, attempts };
}

// ===========================================================================

namespace WilhelmTheWolfDiesIn3Attempts {
    constexpr Monster wilhelm = Wolf{"Wilhelm", HealthPoints{100}};

    constexpr auto result = fight(wilhelm, Weapon::Stick);

    static_assert(result.attempts == 3, "fighting wilhelm should result in 3 attempts");
    static_assert(dead(result.monster), "wilhelm should be dead");
}

namespace GerhardTheFirelordDiesIn5AttemptsWhenUsingSticks {
    constexpr Monster gerhard = Firelord{"Gerhard", HealthPoints{100}};

    constexpr auto result = fight(gerhard, Weapon::Stick);

    static_assert(result.attempts == 5, "fighting gerhard should result in 5 attempts");
    static_assert(dead(result.monster), "gerhard should be dead");
}

namespace GhostsCannotBeKilles {
    constexpr Monster astrid = Ghost();

    constexpr auto result = fight(astrid, Weapon::Arrow);

    static_assert(result.attempts == 5, "fighting astrid should result in 5 attempts");
    static_assert(!dead(result.monster), "astrid should not be dead");
}
