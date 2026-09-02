#include <qanvas/resources/image.hpp>

// std
#include <algorithm>

// stb
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

using namespace Qore::Qanvas;

bool Image::load(const std::string& path) {
    int w, h, c;
    unsigned char* raw = stbi_load(path.c_str(), &w, &h, &c, 4);
    if (!raw) return false;

    m_width = (uint32_t)w;
    m_height = (uint32_t)h;

    m_data.clear();
    m_data.resize((size_t)m_width * m_height);
    for (size_t i = 0; i < m_data.size(); ++i) {
        m_data[i] = Color(raw[i * 4 + 0], raw[i * 4 + 1], raw[i * 4 + 2], raw[i * 4 + 3]);
    }

    stbi_image_free(raw);
    repack();
    return true;
}

void Image::repack() {
    m_packed.resize(m_data.size());
    for (size_t i = 0; i < m_data.size(); ++i) {
        m_packed[i] = m_data[i].pack();
    }
}

void Image::composite(const Image& overlay, int32_t startX, int32_t startY) {
    for (uint32_t y = 0; y < overlay.m_height; ++y) {
        for (uint32_t x = 0; x < overlay.m_width; ++x) {
            int32_t targetX = startX + (int32_t)x;
            int32_t targetY = startY + (int32_t)y;
            if (targetX < 0 || (uint32_t)targetX >= m_width || targetY < 0 || (uint32_t)targetY >= m_height) continue;

            size_t overlayIdx = (size_t)y * overlay.m_width + x;
            size_t targetIdx = (size_t)targetY * m_width + (size_t)targetX;
            m_data[targetIdx] = overlay.m_data[overlayIdx].blend(m_data[targetIdx]);
        }
    }
    repack();
}
std::vector<uint16_t> Image::blend(const Image& overlay, int32_t startX, int32_t startY) {
    uint32_t w = overlay.width();
    uint32_t h = overlay.height();
    std::vector<uint16_t> output((size_t)w * h);

    for (uint32_t y = 0; y < h; ++y) {
        for (uint32_t x = 0; x < w; ++x) {
            int32_t bgX = startX + (int32_t)x;
            int32_t bgY = startY + (int32_t)y;

            Color bg = Color(0, 0, 0);
            if (bgX >= 0 && (uint32_t)bgX < m_width && bgY >= 0 && (uint32_t)bgY < m_height) {
                bg = m_data[(size_t)bgY * m_width + (size_t)bgX];
            }

            Color fg = overlay.at(x, y);
            output[(size_t)y * w + x] = fg.blend(bg).pack();
        }
    }
    return output;
}
std::vector<uint16_t> Image::slice(int32_t srcY) const {
    std::vector<uint16_t> result(m_width);
    int32_t cleanY = std::clamp(srcY, 0, (int32_t)m_height - 1);
    const uint16_t* row = m_packed.data() + (size_t)cleanY * m_width;
    std::copy(row, row + m_width, result.begin());
    return result;
}

void Image::tint(Color color) {
    if (m_data.empty()) return;
    for (auto& px : m_data) px = px * color;
    repack();
}

const Color& Image::at(uint32_t x, uint32_t y) const {
    return m_data[(size_t)y * m_width + x];
}
uint32_t Image::width() const {
    return m_width;
}
uint32_t Image::height() const {
    return m_height;
}
const uint16_t* Image::data() const {
    return m_packed.data();
}