#include <rf_sim/io/image_io.hpp>
#include <rf_sim/model/opencv.hpp>

#include <fstream>

#include <opencv2/opencv.hpp>

namespace RfSim::Io {
    namespace {
        /**
         * @brief Reads the whole file into memory.
         *
         * @throws std::runtime_error if the file cannot be read or is empty.
         */
        std::vector<char> readFile(const std::filesystem::path &path) {
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
}
