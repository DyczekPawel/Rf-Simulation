#pragma once
#include <span>
#include <stdexcept>
#include <vector>

namespace RfSim::Model {
    /**
     * @brief Dense 2D grid stored row-major in a contiguous buffer.
     */
    template <typename T>
    class Grid2D {
    public:
        /**
         * @brief Constructs an empty 0x0 grid.
         */
        Grid2D() = default;

        /**
         * @brief Constructs a grid with every element set to the same value.
         *
         * @param width number of columns.
         * @param height number of rows.
         * @param fill value copied into every element.
         */
        Grid2D(const size_t width, const size_t height, const T &fill = T{})
            : m_width(width),
              m_height(height),
              m_data(width * height, fill) {
        }

        /**
         * @brief Constructs a grid that takes ownership of existing row-major data.
         *
         * @param width number of columns.
         * @param height number of rows.
         * @param data row-major elements; element (x, y) is at index y * width + x.
         *
         * @throws std::invalid_argument if data size is not equal to width * height.
         */
        Grid2D(const size_t width, const size_t height, std::vector<T> data)
            : m_width(width),
              m_height(height),
              m_data(std::move(data)) {
            if (m_data.size() != width * height) {
                throw std::invalid_argument("Grid2D: data size does not match width * height");
            }
        }

        /**
         * @return Number of columns.
         */
        [[nodiscard]] size_t width() const noexcept {
            return m_width;
        }

        /**
         * @return Number of rows.
         */
        [[nodiscard]] size_t height() const noexcept {
            return m_height;
        }

        /**
         * @return Total number of elements (width * height).
         */
        [[nodiscard]] size_t size() const noexcept {
            return m_data.size();
        }

        /**
         * @return \c true if the grid has no elements.
         */
        [[nodiscard]] bool empty() const noexcept {
            return size() == 0;
        }

        /**
         * @brief Access grid field.
         *
         * @param x grid x axis coordinate.
         * @param y grid y axis coordinate.
         *
         * @return Reference to stored grid element.
         *
         * @warning Does not check if coordinates are out of bound; use \c at() if not in hot loop.
         */
        T &operator[](const size_t x, const size_t y) noexcept {
            return m_data[(y * m_width) + x];
        }

        const T &operator[](const size_t x, const size_t y) const noexcept {
            return m_data[(y * m_width) + x];
        }

        /**
         * @brief Access grid field with bounds checking.
         *
         * @param x grid x axis coordinate.
         * @param y grid y axis coordinate.
         *
         * @return Reference to stored grid element.
         *
         * @throws std::out_of_range if coordinates are outside the grid.
         */
        T &at(size_t x, size_t y) {
            checkBounds(x, y);
            return (*this)[x, y];
        }

        const T &at(size_t x, size_t y) const {
            checkBounds(x, y);
            return (*this)[x, y];
        }

        /**
         * @brief Checks whether coordinates lie inside the grid.
         *
         * @param x grid x axis coordinate.
         * @param y grid y axis coordinate.
         *
         * @return \c true if (x, y) is a valid element position.
         */
        [[nodiscard]] bool contains(const size_t x, const size_t y) const noexcept {
            return x < m_width && y < m_height;
        }

        /**
         * @return Pointer to the first element of the row-major buffer.
         */
        T *data() noexcept {
            return m_data.data();
        }

        const T *data() const noexcept {
            return m_data.data();
        }

        /**
         * @brief Flat view of all elements in row-major order.
         *
         * @return Span over the whole buffer, useful for iterating without coordinates.
         */
        std::span<T> values() noexcept {
            return m_data;
        }

        std::span<const T> values() const noexcept {
            return m_data;
        }

    private:
        /**
         * @throws std::out_of_range if (x, y) is outside the grid.
         */
        void checkBounds(const size_t x, const size_t y) const {
            if (!contains(x, y)) {
                throw std::out_of_range("Grid2D: coordinates out of range");
            }
        }

        size_t m_width{0};
        size_t m_height{0};
        std::vector<T> m_data;

    };
}
