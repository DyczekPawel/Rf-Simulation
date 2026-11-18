#pragma once
#include <rf_sim/model/material.hpp>
#include "rf_sim/model/grid2d.hpp"

#include <vector>

namespace RfSim::Model {
    /**
     * @brief Floor plan as a grid of material ids together with the materials they refer to.
     *
     * @note Each grid cell stores an index into the materials vector.
     *          The physical scale is given in pixels per meter.
     */
    class FloorPlan {
    public:
        /**
         * @brief Constructs FloorPlan.
         *
         * @param materialGrid non-empty grid of material ids (indices into \p materials), e.g. built by \c fromImage().
         * @param materials non-empty vector of materials, at most 256 entries (\c MaterialId range).
         * @param pixelsPerMeter how many pixels make up one meter (must be greater than 0).
         *
         * @throws std::invalid_argument on empty material vector.
         * @throws std::invalid_argument if there are more materials than \c MaterialId can index.
         * @throws std::invalid_argument on pixelsPerMeter less or equal 0.
         * @throws std::invalid_argument on empty material grid.
         * @throws std::invalid_argument if the grid references a material id outside \p materials.
         */
        FloorPlan(Grid2D<MaterialId> &&materialGrid, Materials &&materials, float pixelsPerMeter);

        /**
         * @brief Builds FloorPlan from a floor plan image by matching pixel colors to material colors.
         *
         * @note Pixels whose color matches no material are assigned the first material (id 0).
         *
         * @param image floor plan image.
         * @param materials non-empty vector of materials; the first one is the default material.
         * @param pixelsPerMeter how many pixels make up one meter (must be greater than 0).
         *
         * @return Constructed FloorPlan.
         *
         * @throws std::runtime_error on empty material vector.
         * @throws std::invalid_argument on any other condition rejected by the constructor.
         */
        static FloorPlan fromImage(const RgbImage &image, Materials &&materials, float pixelsPerMeter);

        [[nodiscard]] std::size_t width() const noexcept {
            return m_materialGrid.width();
        }

        [[nodiscard]] std::size_t height() const noexcept {
            return m_materialGrid.height();
        }

        [[nodiscard]] const Grid2D<MaterialId> &materialGrid() const noexcept {
            return m_materialGrid;
        }

        [[nodiscard]] float pixelsPerMeter() const noexcept {
            return m_pixelsPerMeter;
        }

        [[nodiscard]] const Materials &materials() const noexcept {
            return m_materials;
        }

    private:
        Grid2D<MaterialId> m_materialGrid; ///< material id of every pixel
        Materials m_materials;             ///< materials indexed by MaterialId
        float m_pixelsPerMeter;            ///< scale of the floor plan
    };
}
