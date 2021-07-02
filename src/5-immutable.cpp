/** This version extends the functional idea.
 *
 * Having made our fighting functional allow us something interesting:
 *   - as pure functions only convert input into output without side-effects,
 *     it is possible to make them constexpr, so they are evaluated
 *     during compilation.
 *
 * This makes it possible to do fancy optimization, and compile-time tests.
 * 
 * Achieving that requires small changes in the API.
 */
#include <variant>
#include "health.h"

// ===========================================================================
// Definitions

/** SEE HERE
 * 1. We cannot use std::string at compile time, so we will return const char*.
 * 2. To simplify the code, we no longer return a Comment. It could be done,
 *    but it makes this example more difficult to explain.
 */

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

// ===========================================================================
// Functions

/** SEE HERE
 * We move to a fully functional approach. This means that functions are no
 * longer allowed to modify their input. Instead, they return the result.
 * 
 * For instance, the result of hiting a wolf for 42 is a wolf with 42 less
 * health points.
 */
constexpr Wolf hit(Wolf wolf, Weapon, HealthPoints damage)
{
    return { wolf.name, max(HealthPoints{0}, wolf.health - damage) };
}

constexpr Firelord hit(Firelord firelord, Weapon weapon, HealthPoints damage)
{
    switch (weapon) {
    case Weapon::Stick:
        return { firelord.name, max(HealthPoints{0}, firelord.health - damage / 2) };
    case Weapon::Fireball:
        return { firelord.name, firelord.health };
    }
    return { firelord.name, max(HealthPoints{0}, firelord.health - damage) };
}

constexpr Ghost hit(Ghost ghost, Weapon, HealthPoints) { return ghost; }


template <typename DefaultMonster>
constexpr bool dead(const DefaultMonster & monster) { return !monster.health; }
constexpr bool dead(const Ghost &) { return false; }

// ===========================================================================
// The actual fighting that uses monsters

/** SEE HERE
 * Same thing for the fight: it is not allowed to modify the monster.
 * So we need to return both the modified monster and how many attempts
 * we did. Therefore, we need to return a struct.
 */
template <typename Monster>
constexpr auto fight(Monster monster, Weapon weapon, int attempts = 5)
{
    struct Result { Monster monster; int attempts; };

    for (int attempt = 1; attempt <= attempts; ++attempt) {
        monster = hit(monster, weapon, HealthPoints{40});
        if (dead(monster))
            return Result{ monster, attempt };
    }
    return Result{ monster, attempts };
}

// ===========================================================================
// Exercising the code

/** SEE HERE
 * Demonstration time!
 * 
 * To show that this really happens at compile time, I removed the functions
 * and replaced them by global constants.
 *
 * If the static_assert succeeds, then it generates no code at all in the 
 * resulting binary.
 *
 * If the static_assert fails, then the code will not compile.
 * Even better, Visual Studio will underline it in red. Try it yourself!
 */
namespace WilhelmTheWolfDiesIn3Attempts {
    constexpr auto wilhelm = Wolf{"Wilhelm", HealthPoints{100}};

    constexpr auto result = fight(wilhelm, Weapon::Stick);

    static_assert(result.attempts == 3);
    static_assert(dead(result.monster));
}

namespace GerhardTheFirelordDiesIn5AttemptsWhenUsingSticks {
    constexpr auto gerhard = Firelord{"Gerhard", HealthPoints{100}};

    constexpr auto result = fight(gerhard, Weapon::Stick);

    static_assert(result.attempts == 5);
    static_assert(dead(result.monster));
}

namespace GhostsCannotBeKilled {
    constexpr auto astrid = Ghost();

    constexpr auto result = fight(astrid, Weapon::Arrow, 1000);

    static_assert(result.attempts == 1000);
    static_assert(!dead(result.monster));
}


/** SEE HERE
 * To be clear, in real life you would not write tests like this. You would
 * do that instead:
 */
#include <catch.hpp>

TEST_CASE("Ghosts cannot be killed")
{
    constexpr auto astrid = Ghost();
    constexpr auto result = fight(astrid, Weapon::Arrow, 1000);

    STATIC_REQUIRE(result.attempts == 1000);    // same as static_assert, but registers it
    STATIC_REQUIRE(!dead(result.monster));      // with Catch2 so it shows in statistics :-)
}
