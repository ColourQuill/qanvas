#ifndef __QANVAS_FONT_HPP__
#define __QANVAS_FONT_HPP__

// std
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

// qontext
#include <msdf/msdf.hpp>

// core
#include <qanvas/core/color.hpp>

namespace Qore::Qanvas {
    struct GlyphBitmap {
        uint16_t w = 0;
        uint16_t h = 0;
        double planeL = 0;
        double planeT = 0;
        std::vector<uint8_t> alpha;
    };

    class Font {
        public:
            bool load(const std::string& path);

            const qtxt::GlyphRecord* getGlyph(uint32_t codepoint) const;
            double getKerning(int codepointA, int codepointB) const;
            double getGeometryScale() const;
            double getEmSize() const;
            qtxt::FontMetrics getMetrics() const;

            const GlyphBitmap* getGlyphBitmap(uint32_t codepoint, double size) const;

            double getAscender() const;
            double getDescender() const;
            double getLineHeight() const;
            double getPxRange() const;
            double getLetterSpacingRatio() const;

            int getAtlasWidth() const;
            int getAtlasHeight() const;
            Color sample(int x, int y) const;
            Color sampleBilinear(double x, double y) const;
        private:
            qtxt::MSDFAtlas m_atlas;
            bool m_loaded = false;

            mutable std::unordered_map<uint64_t, GlyphBitmap> m_glyphCache;
            static constexpr size_t MAX_CACHE_ENTRIES = 8192;
            double m_letterSpacingRatio = 0.0;

            double estimateLetterSpacingRatio() const;
    };
}

#endif