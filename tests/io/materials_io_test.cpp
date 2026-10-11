#include <rf_sim/io/materials_io.hpp>

#include <common/test_utils.hpp>

#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace {
    using RfSim::Io::loadMaterials;
    using RfSim::Model::Rgb;
    using RfSim::Test::TempDir;

    class LoadMaterialsTest : public ::testing::Test {
    protected:
        [[nodiscard]] std::filesystem::path write(const std::string &json) const {
            return m_dir.writeFile("materials.json", json);
        }

        TempDir m_dir;
    };

    TEST_F(LoadMaterialsTest, LoadsMaterialsInOrder) {
        // Distinct RGB channels and shuffled ITU keys catch swapped or position-based assignments.
        const auto materials = loadMaterials(write(R"([
            {"name": "air",      "rgb": [255, 255, 255], "itu": {"a": 1, "b": 0, "c": 0, "d": 0}},
            {"name": "concrete", "rgb": [10, 20, 30],    "itu": {"d": 0.7822, "c": 0.0462, "b": 0, "a": 5.24}}
        ])"));

        ASSERT_EQ(materials.size(), 2U);
        EXPECT_EQ(materials.at(0).name, "air");

        const auto &[name, rgb, ituModel] = materials.at(1);
        EXPECT_EQ(name, "concrete");
        EXPECT_EQ(rgb, (Rgb{.r = 10, .g = 20, .b = 30}));
        EXPECT_DOUBLE_EQ(ituModel.a, 5.24);
        EXPECT_DOUBLE_EQ(ituModel.b, 0.0);
        EXPECT_DOUBLE_EQ(ituModel.c, 0.0462);
        EXPECT_DOUBLE_EQ(ituModel.d, 0.7822);
    }

    TEST_F(LoadMaterialsTest, ThrowsWhenFileDoesNotExist) {
        EXPECT_THROW((void) loadMaterials(m_dir.file("missing.json")), std::runtime_error);
    }

    TEST_F(LoadMaterialsTest, ThrowsOnInvalidDocument) {
        const std::vector<std::string> documents = {
            "",                  // empty file
            R"([{"name": "x",)", // malformed JSON
            "[]",                // no materials
            R"({"name": "x"})",  // not an array
        };

        for (const auto &json: documents) {
            SCOPED_TRACE(json);
            EXPECT_THROW((void) loadMaterials(write(json)), std::runtime_error);
        }
    }

    TEST_F(LoadMaterialsTest, ThrowsOnInvalidMaterial) {
        // Each case breaks one part of an otherwise valid material.
        const std::vector<std::string> materials = {
            R"({"rgb": [1, 2, 3], "itu": {"a": 1, "b": 0, "c": 0, "d": 0}})",                // no name
            R"({"name": 1, "rgb": [1, 2, 3], "itu": {"a": 1, "b": 0, "c": 0, "d": 0}})",     // name not a string
            R"({"name": "m", "rgb": [1, 2], "itu": {"a": 1, "b": 0, "c": 0, "d": 0}})",      // rgb too short
            R"({"name": "m", "rgb": [1, 2.5, 3], "itu": {"a": 1, "b": 0, "c": 0, "d": 0}})", // rgb not integer
            R"({"name": "m", "rgb": [1, 2, 256], "itu": {"a": 1, "b": 0, "c": 0, "d": 0}})", // rgb out of range
            R"({"name": "m", "rgb": [1, 2, 3], "itu": {"a": 1, "b": 0, "c": 0}})",           // itu key missing
            R"({"name": "m", "rgb": [1, 2, 3], "itu": {"a": 1, "b": "0", "c": 0, "d": 0}})", // itu not a number
            "42",                                                                            // not an object
        };

        for (const auto &material: materials) {
            SCOPED_TRACE(material);
            EXPECT_THROW((void) loadMaterials(write("[" + material + "]")), std::runtime_error);
        }
    }
}
