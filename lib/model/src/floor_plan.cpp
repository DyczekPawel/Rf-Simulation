#include <rf_sim/model/floor_plan.hpp>
#include <rf_sim/profiling.hpp>

#include <stdexcept>
#include <limits>

namespace RfSim::Model {
    namespace {
        /**
         * @brief Maps every image pixel to the id of the material with the same color.
         *
         * @param image floor plan image.
         * @param materials non-empty vector of materials.
         *
         * @return Grid of material ids with the same size as \p image.
         *
         * @throws std::runtime_error on empty material vector.
         */
        Grid2D<MaterialId> buildMaterialGrid(const RgbImage &image, const Materials &materials) {
            RF_SIM_PROFILE_FUNCTION();

            if (materials.empty()) throw std::runtime_error("buildMaterialGrid: materials is empty");

            std::vector<uint8_t> materialIdGridFlattened;
            materialIdGridFlattened.reserve(image.size());

            for (const auto &pixel: image.values()) {
                bool pushed = false;
                MaterialId id = 0;
                for (const auto &material: materials) {
                    if (material.rgb == pixel) {
                        materialIdGridFlattened.push_back(id);
                        pushed = true;
                        break;
                    }
                    id++;
                }
                // Treat undefined materials as default material (first in JSON config)
                if (!pushed) materialIdGridFlattened.push_back(0);
            }

            return {image.width(), image.height(), std::move(materialIdGridFlattened)};

        }
    }

    FloorPlan::FloorPlan(Grid2D<MaterialId> &&materialGrid,
                         Materials &&materials,
                         const double pixelsPerMeter)
        : m_materialGrid(std::move(materialGrid)),
          m_materials(std::move(materials)),
          m_pixelsPerMeter(pixelsPerMeter) {
        RF_SIM_PROFILE_FUNCTION();

        if (m_materials.empty())
            throw std::invalid_argument("FloorPlan: Material list is empty");
        if (m_materials.size() > static_cast<std::size_t>(std::numeric_limits<MaterialId>::max()) + 1)
            throw std::invalid_argument("FloorPlan: too many materials for MaterialId type");

        if (m_pixelsPerMeter <= 0)
            throw std::invalid_argument("FloorPlan: pixels per meter must be greater than 0");

        if (m_materialGrid.empty())
            throw std::invalid_argument("FloorPlan: MaterialMap is empty");
        {
            RF_SIM_PROFILE_SCOPE("Validate material grid");
            for (const MaterialId id: m_materialGrid.values()) {
                if (id >= m_materials.size())
                    throw std::invalid_argument("FloorPlan: material grid references unknown material id");
            }
        }
    }

    FloorPlan FloorPlan::fromImage(const RgbImage &image, Materials &&materials, double pixelsPerMeter) {
        RF_SIM_PROFILE_FUNCTION();

        auto grid = buildMaterialGrid(image, materials);
        return {std::move(grid), std::move(materials), pixelsPerMeter};
    }
}
