#pragma once
#include <cmath>
#include <limits>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <utility>
#include <concepts>

namespace RfSim::Solver {

    enum class Axis: uint8_t {
        X = 0,
        Y = 1,
    };

    template <typename T>
    struct Vec2D {
        T x{0};
        T y{0};

        constexpr Vec2D &operator+=(const Vec2D &o) noexcept {
            x += o.x;
            y += o.y;
            return *this;
        }

        constexpr Vec2D &operator-=(const Vec2D &o) noexcept {
            x -= o.x;
            y -= o.y;
            return *this;
        }

        constexpr Vec2D &operator*=(T s) noexcept {
            x *= s;
            y *= s;
            return *this;
        }

        constexpr Vec2D &operator/=(T s) noexcept requires std::floating_point<T> {
            x /= s;
            y /= s;
            return *this;
        }

        friend constexpr Vec2D operator+(Vec2D a, const Vec2D &b) noexcept {
            return a += b;
        }

        friend constexpr Vec2D operator-(Vec2D a, const Vec2D &b) noexcept {
            return a -= b;
        }

        friend constexpr Vec2D operator-(const Vec2D &v) noexcept {
            return {.x = -v.x, .y = -v.y};
        }

        friend constexpr Vec2D operator*(Vec2D v, T s) noexcept {
            return v *= s;
        }

        friend constexpr Vec2D operator*(T s, Vec2D v) noexcept {
            return v *= s;
        }

        friend constexpr Vec2D operator/(Vec2D v, T s) noexcept requires std::floating_point<T> {
            return v /= s;
        }

        constexpr bool operator==(const Vec2D &) const noexcept = default;

        // ReSharper disable once CppNonExplicitConversionOperator
        operator std::pair<int, int>() const noexcept requires std::same_as<T, int> {
            return {x, y};
        }
    };

    template <typename T>
    [[nodiscard]] constexpr T dot(const Vec2D<T> &a, const Vec2D<T> &b) noexcept {
        return (a.x * b.x) + (a.y * b.y);
    }

    template <std::floating_point T>
    [[nodiscard]] constexpr T length(const Vec2D<T> &v) noexcept {
        return std::sqrt(dot(v, v));
    }

    template <std::floating_point T>
    [[nodiscard]] constexpr Vec2D<T> normalized(const Vec2D<T> &v) noexcept {
        const T len = length(v);
        assert(len > T{0} && "cannot normalize zero vector");
        return v / len;
    }

    template <std::floating_point T>
    [[nodiscard]] constexpr Vec2D<T> reflect(const Vec2D<T> &d, const Vec2D<T> &n) noexcept {
        return d - (T{2} * dot(d, n) * n);
    }

    [[nodiscard]] constexpr Vec2D<float> axisNormal(const Axis axis) noexcept {
        switch (axis) {
        case Axis::X:
            return {.x = 1.0, .y = 0.0};
        case Axis::Y:
            return {.x = 0.0, .y = 1.0};
        }
        std::unreachable();
    }

    struct Ray2D {
        Vec2D<float> origin;
        Vec2D<float> direction;
        Vec2D<int> startCell;
        float distanceOffset{0};
        float lossDb{0};
        unsigned int bounces{0};

        [[nodiscard]] constexpr Vec2D<float> pointAt(const float t) const noexcept {
            return origin + t * direction;
        }
    };

    class RayTraversal2D {
    public:
        RayTraversal2D(const Vec2D<float> origin, const Vec2D<float> direction, const Vec2D<int> startCell) noexcept
            : m_stepX{direction.x > 0 ? 1 : -1},
              m_stepY{direction.y > 0 ? 1 : -1},
              m_positionX(startCell.x),
              m_positionY(startCell.y),
              m_tMaxX(initialTMax(origin.x, m_positionX, m_stepX, direction.x)),
              m_tMaxY(initialTMax(origin.y, m_positionY, m_stepY, direction.y)),
              m_tDeltaX(direction.x == 0 ? std::numeric_limits<float>::infinity() : std::abs(1 / direction.x)),
              m_tDeltaY(direction.y == 0 ? std::numeric_limits<float>::infinity() : std::abs(1 / direction.y)),
              m_tExit(std::min(m_tMaxX, m_tMaxY)) {
            assert(std::abs(dot(direction, direction) - 1.0) < 1e-4 && "direction must be a unit vector");

            assert(origin.x >= static_cast<float>(startCell.x) - 1e-4 &&
                origin.x <= static_cast<float>(startCell.x + 1) + 1e-4 &&
                origin.y >= static_cast<float>(startCell.y) - 1e-4 &&
                origin.y <= static_cast<float>(startCell.y + 1) + 1e-4 &&
                "origin must lie within startCell");
        }

        [[nodiscard]] Vec2D<int> pos() const noexcept {
            return {.x = m_positionX, .y = m_positionY};
        }

        Axis advance() noexcept {
            auto axis = Axis::Y; // default to Y axis
            if (m_tMaxX < m_tMaxY) {
                m_positionX += m_stepX;
                m_tMaxX += m_tDeltaX;
                axis = Axis::X;
            } else {
                m_positionY += m_stepY;
                m_tMaxY += m_tDeltaY;
            }

            m_tEntry = m_tExit;
            m_tExit = std::min(m_tMaxX, m_tMaxY);
            return axis;
        }

        [[nodiscard]] float tExit() const noexcept {
            return m_tExit;
        }

        [[nodiscard]] float tEntry() const noexcept {
            return m_tEntry;
        }

    private:
        static float initialTMax(const float origin,
                                 const int cell,
                                 const int step,
                                 const float direction) noexcept {
            if (direction == 0.0F) return std::numeric_limits<float>::infinity();
            return (static_cast<float>(cell) - origin + static_cast<float>(std::max(step, 0))) / direction;
        }

        // Ray step
        int m_stepX;
        int m_stepY;

        // Ray current in grid position
        int m_positionX;
        int m_positionY;

        // Ray tMax
        float m_tMaxX;
        float m_tMaxY;

        // Ray tDelta
        float m_tDeltaX;
        float m_tDeltaY;

        // Ray distances
        float m_tExit;
        float m_tEntry{0};
    };
}
