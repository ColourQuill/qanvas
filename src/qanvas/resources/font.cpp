#include <qanvas/resources/font.hpp>

// std
#include <algorithm>
#include <cmath>

using namespace Qore::Qanvas;

bool Font::load(const std::string& path) {
    m_atlas = qtxt::MSDF::load(path);
    m_loaded = m_atlas.atlas.width() > 0 && m_atlas.atlas.height() > 0;
    if (m_loaded) {
        m_letterSpacingRatio = estimateLetterSpacingRatio();
    }
    return m_loaded;
}

const qtxt::GlyphRecord* Font::getGlyph(uint32_t codepoint) const {
    return m_atlas.getGlyph(codepoint);
}

double Font::getKerning(int codepointA, int codepointB) const {
    return m_atlas.getKerning(codepointA, codepointB);
}
double Font::getGeometryScale() const {
    return m_atlas.geometryScale;
}
double Font::getEmSize() const {
    return m_atlas.metrics.emSize;
}
qtxt::FontMetrics Font::getMetrics() const {
    return m_atlas.metrics;
}

const GlyphBitmap* Font::getGlyphBitmap(uint32_t codepoint, double size) const {
    uint32_t sizeUnits = (uint32_t)std::round(size * 4.0);
    uint64_t key = (uint64_t(codepoint) << 32) | sizeUnits;

    auto it = m_glyphCache.find(key);
    if (it != m_glyphCache.end()) return &it->second;

    const qtxt::GlyphRecord* g = getGlyph(codepoint);
    if (!g) return nullptr;

    if (m_glyphCache.size() >= MAX_CACHE_ENTRIES) m_glyphCache.clear();

    GlyphBitmap bmp;
    double planeW = g->planeR - g->planeL;
    double planeH = g->planeT - g->planeB;
    if (planeW <= 0 || planeH <= 0) {
        return &(m_glyphCache[key] = bmp);
    }

    int dstW = (int)std::ceil(planeW * size + getPxRange() * 0.5);
    int dstH = (int)std::ceil(planeH * size + getPxRange() * 0.5);

    bmp.planeL = g->planeL * size;
    bmp.planeT = g->planeT * size;
    bmp.w = (uint16_t)dstW;
    bmp.h = (uint16_t)dstH;
    bmp.alpha.resize((size_t)dstW * dstH);

    double atlasW = g->atlasR - g->atlasL;
    double atlasH = g->atlasT - g->atlasB;
    double scaleX = dstW > 0 ? (double)dstW / atlasW : 0.0;
    double scaleY = dstH > 0 ? (double)dstH / atlasH : 0.0;
    double screenPxRange = std::max(getPxRange() * 0.5 * (scaleX + scaleY), 1.0);

    for (int dy = 0; dy < dstH; ++dy) {
        for (int dx = 0; dx < dstW; ++dx) {
            double u = dstW > 0 ? (dx + 0.5) / dstW : 0.0;
            double v = dstH > 0 ? (dy + 0.5) / dstH : 0.0;
            double atlasX = g->atlasL + u * atlasW;
            double atlasY = g->atlasB + (1.0 - v) * atlasH;

            Color sample = sampleBilinear(atlasX, atlasY);
            double med = std::max(std::min((double)sample.r, (double)sample.g), std::min(std::max((double)sample.r, (double)sample.g), (double)sample.b));
            double sd = (med / 255.0) - 0.5;
            double alpha = std::clamp(screenPxRange * sd + 0.5, 0.0, 1.0);
            bmp.alpha[dy * dstW + dx] = (uint8_t)(alpha * 255.0);
        }
    }
    return &(m_glyphCache[key] = std::move(bmp));
}

double Font::getAscender() const {
    return m_atlas.metrics.ascenderY;
}
double Font::getDescender() const {
    return m_atlas.metrics.descenderY;
}
double Font::getLineHeight() const {
    return m_atlas.metrics.lineHeight;
}
double Font::getPxRange() const {
    return m_atlas.pxRange;
}
double Font::getLetterSpacingRatio() const {
    return m_letterSpacingRatio;
}

int Font::getAtlasWidth() const {
    return m_atlas.atlas.width();
}
int Font::getAtlasHeight() const {
    return m_atlas.atlas.height();
}

Color Font::sample(int x, int y) const {
    int w = m_atlas.atlas.width(), h = m_atlas.atlas.height();
    if (x < 0) x = 0; if (x >= w) x = w - 1;
    if (y < 0) y = 0; if (y >= h) y = h - 1;
    const uint8_t* px = m_atlas.atlas(x, y);
    return Color(px[0], px[1], px[2]);
}

Color Font::sampleBilinear(double x, double y) const {
    x -= 0.5;
    y -= 0.5;

    int x0 = (int)std::floor(x);
    int y0 = (int)std::floor(y);
    double tx = x - x0;
    double ty = y - y0;

    Color c00 = sample(x0, y0);
    Color c10 = sample(x0 + 1, y0);
    Color c01 = sample(x0, y0 + 1);
    Color c11 = sample(x0 + 1, y0 + 1);

    double r = (c00.r * (1 - tx) + c10.r * tx) * (1 - ty) + (c01.r * (1 - tx) + c11.r * tx) * ty;
    double g = (c00.g * (1 - tx) + c10.g * tx) * (1 - ty) + (c01.g * (1 - tx) + c11.g * tx) * ty;
    double b = (c00.b * (1 - tx) + c10.b * tx) * (1 - ty) + (c01.b * (1 - tx) + c11.b * tx) * ty;

    return Color((uint8_t)r, (uint8_t)g, (uint8_t)b);
}

double Font::estimateLetterSpacingRatio() const {
    static const char* samplePairs[] = {
        "ac", "ce", "ao", "cn", "ea", "oa", "na", "en", "sp", "ck"
    };
    constexpr double TEST_SIZE = 32.0;
    double worstOverlapPx = 0.0;

    for (const char* pair : samplePairs) {
        char a = pair[0], b = pair[1];
        const qtxt::GlyphRecord* ga = getGlyph((unsigned char)a);
        const qtxt::GlyphRecord* gb = getGlyph((unsigned char)b);
        if (!ga || !gb) continue;

        const GlyphBitmap* bmpA = getGlyphBitmap((unsigned char)a, TEST_SIZE);
        const GlyphBitmap* bmpB = getGlyphBitmap((unsigned char)b, TEST_SIZE);
        if (!bmpA || !bmpB) continue;

        double aRightInk = ga->planeR * TEST_SIZE;
        double bLeftInk  = (ga->advance * TEST_SIZE) + (gb->planeL * TEST_SIZE);
        double overlapPx = aRightInk - bLeftInk;
        worstOverlapPx = std::max(worstOverlapPx, overlapPx);
    }

    double bleedAllowancePx = getPxRange() * 0.5;
    double effectiveWorst = worstOverlapPx + bleedAllowancePx;

    if (effectiveWorst <= 0.0) return 0.0;

    double ratio = effectiveWorst / TEST_SIZE;
    return ratio;
}