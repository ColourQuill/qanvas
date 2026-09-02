#ifndef __QANVAS_IMAGE_HPP__
#define __QANVAS_IMAGE_HPP__

// std
#include <cstdint>
#include <string>
#include <vector>

// core
#include <qanvas/core/color.hpp>

namespace Qore::Qanvas {
    class Image {
        public:
            bool load(const std::string& path);

            void composite(const Image& overlay, int32_t startX, int32_t startY);
            std::vector<uint16_t> blend(const Image& overlay, int32_t startX, int32_t startY);
            std::vector<uint16_t> slice(int32_t srcY) const;

            void tint(Color color);

            const Color& at(uint32_t x, uint32_t y) const;
            uint32_t width() const;
            uint32_t height() const;
            const uint16_t* data() const;
        private:
            void repack();

            uint32_t m_width = 0;
            uint32_t m_height = 0;
            std::vector<Color> m_data;
            std::vector<uint16_t> m_packed;
    };
}

#endif