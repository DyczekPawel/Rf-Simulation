#pragma once
#include <trace_stats.hpp>

#include "rf_sim/solver/api/raytracing_config.hpp"
#include <rf_sim/solver/api/solver.hpp>

namespace RfSim::Solver {


    class RaytracingSolver final : public ISolver {
    public:
        explicit RaytracingSolver(const RaytracingConfig &config);

    private:
        struct TraceContext {
            const Model::Grid2D<Model::MaterialId> &matGrid;
            const std::vector<MaterialPhysics> &matPhys;
            double pixelsPerMeter;
            double txPowerDbm;
            double frequency;
            double wavelength;
            double minPowerDbm;
            unsigned maxBounces;
            Polarization polarization;
        };

        static std::vector<MaterialPhysics> preProcessMats(const Model::Materials &mats, double frequency);

        void traceRay(Ray2D ray,
                      const TraceContext &ctx,
                      Model::Grid2D<double> &out,
                      std::vector<Ray2D> &pending,
                      TraceStats &stats) const;

        static std::vector<Ray2D> getInitialRays(size_t gridWidth, size_t gridHeight, double txCoordX, double txCoordY);

        std::vector<Model::SignalMap>
        doSolve(const Model::Scene &scene, SolverObserver &observer) override;

        [[nodiscard]] std::string doName() const override {
            return "Raytracing CPU Solver";
        }

        unsigned m_bounceLimit;
        double m_minPowerDbm;
        Polarization m_polarization;

    };
}
