#include <ray.hpp>
#include <physics.hpp>

#include <rf_sim/solver/raytracing_cpu/raytracing_solver.hpp>

#include <algorithm>
#include <complex>
#include <cmath>
#include <numbers>

namespace RfSim::Solver {
    RaytracingSolver::RaytracingSolver(const RaytracingConfig &config)
        : m_bounceLimit(config.maxBounces),
          m_minPowerDbm(config.minPowerDbm),
          m_polarization(config.polarization) {

    }

    std::vector<MaterialPhysics> RaytracingSolver::preProcessMats(const Model::Materials &mats,
                                                                  const double frequency) {
        std::vector<MaterialPhysics> result;
        result.reserve(mats.size());

        for (const auto &mat: mats) {
            const Model::MaterialEM matEM{mat.ituModel, frequency};
            const MaterialPhysics matPhy{matEM.epsilonR, matEM.sigma, frequency};
            result.push_back(matPhy);
        }

        return result;
    }

    void RaytracingSolver::traceRay(Ray2D ray,
                                    const TraceContext &ctx,
                                    Model::Grid2D<double> &out,
                                    std::vector<Ray2D> &pending,
                                    TraceStats &stats) const {
        ++stats.raysTraced;
        auto rt = RayTraversal2D(ray.origin, ray.direction, ray.startCell);

        auto prevCell = ray.startCell;
        auto currCell = rt.pos();
        auto advanceAxis = Axis::Y; //default to Y axis same as RayTraversal2d::advance()

        Model::MaterialId currMatId = ctx.matGrid[static_cast<size_t>(currCell.x), static_cast<size_t>(currCell.y)];
        Model::MaterialId prevMatId = currMatId;

        const size_t iterLimit = ctx.matGrid.height() + ctx.matGrid.width();
        size_t iter = 0;
        for (;;) {
            if (!ctx.matGrid.contains(currCell)) {
                ++stats.endedOutOfGrid;
                return;
            }
            if (iter++ >= iterLimit) {
                ++stats.endedByIterLimit;
                return;
            }
            ++stats.cellsVisited;

            const double tEntryM = rt.tEntry() / ctx.pixelsPerMeter;
            const double tExitM = rt.tExit() / ctx.pixelsPerMeter;

            currMatId = ctx.matGrid[static_cast<size_t>(currCell.x), static_cast<size_t>(currCell.y)];

            if (prevMatId != currMatId) {
                const auto n = axisNormal(advanceAxis);
                const double cosTheta = std::abs(dot(ray.direction, n));
                const auto newRayDir = reflect(ray.direction, n);

                const double refl = reflectance(ctx.matPhys[prevMatId].eta,
                                                ctx.matPhys[currMatId].eta,
                                                cosTheta,
                                                ctx.polarization);
                const double newRayLossDb = ray.lossDb + fractionToLossDb(refl);

                if (ray.bounces >= m_bounceLimit) {
                    ++stats.reflectionsSkippedByBounces;
                } else {
                    const double childDistM = ray.distanceOffset + tEntryM;
                    const double childPowerDbm = ctx.txPowerDbm
                                                 - fsplDb(std::max(childDistM, ctx.wavelength), ctx.frequency)
                                                 - newRayLossDb;

                    if (childPowerDbm < m_minPowerDbm) {
                        ++stats.reflectionsSkippedByPower;
                    } else {
                        pending.push_back(Ray2D{
                            .origin = ray.pointAt(rt.tEntry()),
                            .direction = newRayDir,
                            .startCell = prevCell,
                            .distanceOffset = childDistM,
                            .lossDb = newRayLossDb,
                            .bounces = ray.bounces + 1,
                        });
                        ++stats.raysSpawned;
                    }
                }

                ray.lossDb += fractionToLossDb(1.0 - refl);
            }

            const double halfMatLossDb = 0.5 * ctx.matPhys[currMatId].attenuationDbPerM * (tExitM - tEntryM);
            ray.lossDb += halfMatLossDb;

            const double distM = std::max(ray.distanceOffset + (0.5 * (tEntryM + tExitM)), ctx.wavelength);
            const double powerDbm = ctx.txPowerDbm - fsplDb(distM, ctx.frequency) - ray.lossDb;

            auto &cellPower = out[static_cast<size_t>(currCell.x), static_cast<size_t>(currCell.y)];
            cellPower = std::max(cellPower, powerDbm);

            ray.lossDb += halfMatLossDb;

            if (powerDbm < m_minPowerDbm) {
                ++stats.endedByPower;
                return;
            }

            prevMatId = currMatId;

            advanceAxis = rt.advance();
            prevCell = currCell;
            currCell = rt.pos();
        }
    }

    std::vector<Ray2D> RaytracingSolver::getInitialRays(const size_t gridWidth,
                                                        const size_t gridHeight,
                                                        const double txCoordX,
                                                        const double txCoordY) {
        const int rayAmount = static_cast<int>(std::ceil(
            std::numbers::pi * 2 * std::sqrt(std::pow(gridWidth, 2) + std::pow(gridHeight, 2))));

        std::vector<Ray2D> initialRays;
        initialRays.reserve(static_cast<size_t>(rayAmount));

        for (int i = 0; i < rayAmount; ++i) {
            const double phi = 2.0 * std::numbers::pi * i / rayAmount;
            initialRays.push_back(Ray2D{
                .origin = {.x = txCoordX, .y = txCoordY},
                .direction = {.x = std::cos(phi), .y = std::sin(phi)},
                .startCell = {.x = static_cast<int>(std::floor(txCoordX)),
                              .y = static_cast<int>(std::floor(txCoordY)),},

            });
        }

        return initialRays;
    }

    std::vector<Model::SignalMap> RaytracingSolver::doSolve(const Model::Scene &scene,
                                                            SolverObserver &observer) {
        std::vector<Model::SignalMap> results;
        results.reserve(scene.transmitters().size());

        const auto &fp = scene.floorPlan();
        const auto &txs = scene.transmitters();

        for (size_t txIdx = 0; txIdx < txs.size(); ++txIdx) {
            observer.onTransmitterStart(txIdx, txs.size());

            const auto context = TraceContext{
                .matGrid = fp.materialGrid(),
                .matPhys = preProcessMats(fp.materials(), txs[txIdx].frequencyHz()),
                .pixelsPerMeter = fp.pixelsPerMeter(),
                .txPowerDbm = txs[txIdx].powerDbm(),
                .frequency = txs[txIdx].frequencyHz(),
                .wavelength = k_speedOfLight / txs[txIdx].frequencyHz(),
                .minPowerDbm = m_minPowerDbm,
                .maxBounces = m_bounceLimit,
                .polarization = m_polarization,
            };

            const auto [txMetersX, txMetersY] = txs[txIdx].positionM();
            const auto initialRays = getInitialRays(fp.width(),
                                                    fp.height(),
                                                    txMetersX * fp.pixelsPerMeter(),
                                                    txMetersY * fp.pixelsPerMeter());

            Model::SignalMap signalMap{fp.width(), fp.height(), fp.pixelsPerMeter()};

            std::vector<Ray2D> pending;
            pending.reserve(m_bounceLimit + 1);

            TraceStats stats;
            for (size_t rayIdx = 0; rayIdx < initialRays.size(); ++rayIdx) {
                pending.push_back(initialRays[rayIdx]);
                while (!pending.empty()) {
                    const Ray2D ray{pending.back()};
                    pending.pop_back();
                    traceRay(ray, context, signalMap.grid(), pending, stats);
                    stats.maxPendingSize = std::max(stats.maxPendingSize, pending.size());
                }

                const auto m = stats.metrics();
                observer.onProgress({.done = rayIdx + 1, .total = initialRays.size(), .metrics = m});
            }

            const auto m = stats.metrics();
            observer.onTransmitterFinish(txIdx, m);
            results.push_back(std::move(signalMap));
        }

        return results;
    }
}
