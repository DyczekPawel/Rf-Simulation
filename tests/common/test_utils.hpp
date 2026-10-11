#pragma once
#pragma once
#include <rf_sim/model/rgb.hpp>

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <ostream>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>

namespace RfSim::Model {
    /// Readable gtest output for Rgb (instead of a raw byte dump).
    inline void PrintTo(const Rgb &rgb, std::ostream *os) {
        *os << "Rgb{" << static_cast<int>(rgb.r) << ", " << static_cast<int>(rgb.g) << ", "
            << static_cast<int>(rgb.b) << "}";
    }
}

namespace RfSim::Test {
    /**
     * @brief RAII temporary directory, removed with its contents on destruction.
     */
    class TempDir {
    public:
        TempDir() {
            const auto *info = ::testing::UnitTest::GetInstance()->current_test_info();
            std::string name = "rf_sim_test";
            if (info != nullptr) {
                name += "_" + sanitize(info->test_suite_name()) + "_" + sanitize(info->name());
            }

            m_path = std::filesystem::temp_directory_path() / name;
            std::filesystem::remove_all(m_path);
            std::filesystem::create_directories(m_path);
        }

        ~TempDir() {
            std::error_code ec;
            std::filesystem::remove_all(m_path, ec);
        }

        TempDir(const TempDir &) = delete;
        TempDir &operator=(const TempDir &) = delete;
        TempDir(TempDir &&) = delete;
        TempDir &operator=(TempDir &&) = delete;

        /// Path getter.
        [[nodiscard]] const std::filesystem::path &path() const {
            return m_path;
        }

        /// File path getter.
        [[nodiscard]] std::filesystem::path file(const std::string_view filename) const {
            return m_path / filename;
        }

        /**
         * @brief Writes \p content to a file in this directory.
         *
         * @param filename name of the new file.
         * @param content content to be written to the new file.
         *
         * @return File path of the new file.
         */
        std::filesystem::path writeFile(const std::string_view filename, const std::string_view content) const {
            auto filePath = file(filename);
            std::ofstream out(filePath, std::ios::binary);
            if (!out) throw std::runtime_error("TempDir: cannot create " + filePath.string());
            out << content;
            return filePath;
        }

    private:
        /**
         * @brief Sanitizes file name.
         *
         * @details Parameterized test names contain '/', which is not valid in a file name.
         *
         * @param text file name to be sanitized.
         *
         * @return Sanitized file name.
         */
        static std::string sanitize(std::string text) {
            for (auto &c: text)
                if (c == '/' || c == '\\') c = '_';
            return text;
        }

        std::filesystem::path m_path;
    };
}
