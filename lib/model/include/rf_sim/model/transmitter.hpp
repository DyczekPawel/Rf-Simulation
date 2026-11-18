#pragma once
#include <string>
#include <utility>

namespace RfSim::Model {
    struct Point2D {
        float x{0}; //[m]
        float y{0}; //[m]
    };

    class Transmitter {
    public:
        Transmitter(std::string name, const Point2D position, const float frequencyHz, const float powerDbm)
            : m_name(std::move(name)),
              m_position(position),
              m_frequencyHz(frequencyHz),
              m_powerDbm(powerDbm) {
        };

        [[nodiscard]] const std::string &name() const noexcept {
            return m_name;
        }

        [[nodiscard]] Point2D positionM() const noexcept {
            return m_position;
        }

        [[nodiscard]] float frequencyHz() const noexcept {
            return m_frequencyHz;
        }

        [[nodiscard]] float powerDbm() const noexcept {
            return m_powerDbm;
        }

    private:
        std::string m_name;
        Point2D m_position;
        float m_frequencyHz;
        float m_powerDbm;
    };
}
