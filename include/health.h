#ifndef HEALTH_H
#define HEALTH_H

#include <algorithm>

struct HealthPoints {
    int value;

    explicit constexpr operator bool() const { return value > 0; }
};

constexpr HealthPoints operator+(HealthPoints lhs, HealthPoints rhs) { return { lhs.value + rhs.value }; }
constexpr HealthPoints operator-(HealthPoints lhs, HealthPoints rhs) { return { lhs.value - rhs.value }; }
template <typename Scalar> constexpr HealthPoints operator*(HealthPoints lhs, Scalar rhs) { return { lhs.value * rhs }; }
template <typename Scalar> constexpr HealthPoints operator*(Scalar lhs, HealthPoints rhs) { return { lhs * rhs.value }; }
template <typename Scalar> constexpr HealthPoints operator/(HealthPoints lhs, Scalar rhs) { return { lhs.value / rhs }; }

constexpr HealthPoints max(HealthPoints lhs, HealthPoints rhs) { return { std::max(lhs.value, rhs.value) }; };
constexpr HealthPoints min(HealthPoints lhs, HealthPoints rhs) { return { std::min(lhs.value, rhs.value) }; };


#endif