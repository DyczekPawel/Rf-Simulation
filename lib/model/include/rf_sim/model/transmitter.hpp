#pragma once
#include <string>
#include <utility>

namespace RfSim::Model {
    struct Point2D {
        double x{0}; //[m]
        double y{0}; //[m]
    };

    class Transmitter {
    public:
        Transmitter(std::string name, const Point2D position, const double frequencyHz, const double powerDbm)
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

        [[nodiscard]] double frequencyHz() const noexcept {
            return m_frequencyHz;
        }

        [[nodiscard]] double powerDbm() const noexcept {
            return m_powerDbm;
        }

    private:
        std::string m_name;
        Point2D m_position;
        double m_frequencyHz;
        double m_powerDbm;
    };
}
