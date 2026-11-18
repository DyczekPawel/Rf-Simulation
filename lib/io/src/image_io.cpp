#include "json_utils.hpp"

#include <rf_sim/io/image_io.hpp>
#include <rf_sim/model/opencv.hpp>
#include <rf_sim/profiling.hpp>

#include <algorithm>
#include <fstream>
#include <iterator>

#include <opencv2/opencv.hpp>

namespace RfSim::Io {
    namespace {
        /**
         * @brief Reads the whole file into memory.
         *
         * @throws std::runtime_error if the file cannot be read or is empty.
         */
        std::vector<char> readFile(const std::filesystem::path &path) {
            RF_SIM_PROFILE_FUNCTION();

            std::error_code ec;
            const auto size = std::filesystem::file_size(path, ec);
            if (ec) throw std::runtime_error("loadImage: cannot read " + path.string() + ": " + ec.message());
            if (size == 0) throw std::runtime_error("loadImage: file is empty: " + path.string());

            std::ifstream file(path, std::ios::binary);
            std::vector<char> buffer(size);
            if (!file.read(buffer.data(), static_cast<std::streamsize>(size)))
                throw std::runtime_error("loadImage: cannot read " + path.string());
            return buffer;
        }
    }


    Model::RgbImage loadImage(const std::filesystem::path &path) {
        RF_SIM_PROFILE_FUNCTION();

        const auto buffer = readFile(path);
        const auto mat = cv::imdecode(buffer, cv::IMREAD_COLOR);

        if (mat.empty()) throw std::runtime_error("loadImage: cannot decode " + path.string());

        Model::RgbImage image;
        try {
            image = Model::cv2grid(mat);
        } catch (const std::exception &e) {
            throw std::runtime_error("loadImage: floor plan image conversion to grid failed: " + std::string(e.what()));
        }

        return image;
    }

    void saveImage(const std::filesystem::path &path, const Model::RgbImage &image) {
        RF_SIM_PROFILE_FUNCTION();

        const auto save = [&] {
            const auto extension = path.extension().string();
            if (extension.empty()) throw std::runtime_error("path has no extension, cannot choose format");

            // Encode in memory and write with std::ofstream, mirroring loadImage:
            // cv::imwrite takes a narrow string path and breaks on non-ASCII paths on Windows.
            const auto mat = Model::grid2cv(image);
            std::vector<uchar> buffer;
            try {
                if (!cv::imencode(extension, mat, buffer)) throw std::runtime_error("encoding failed");
            } catch (const cv::Exception &e) {
                throw std::runtime_error("unsupported format '" + extension + "': " + e.what());
            }

            std::ofstream file(path, std::ios::binary);
            if (!file) throw std::runtime_error("cannot open file for writing");

            // Byte-wise copy through the stream buffer instead of ostream::write, which needs const char*
            const auto result = std::ranges::copy(buffer, std::ostreambuf_iterator<char>(file));

            // close() flushes the buffer, so write errors may only show up here
            file.close();
            if (result.out.failed() || !file) throw std::runtime_error("write failed");
        };

        Detail::withContext("saveImage: " + path.string(), save);
    }
}
