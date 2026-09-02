#include <qanvas/draw/renderer.hpp>

// std
#include <algorithm>
#include <cmath>
#include <iostream>

// draw
#include <qanvas/draw/canvas.hpp>

// resources
#include <qanvas/resources/font.hpp>
#include <qanvas/resources/image.hpp>

using namespace Qore::Qanvas;

void Renderer::drawRect(Canvas* canvas, ContentRect r, Color color) {
    uint16_t packed = color.pack();
    for (int16_t y = 0; y < r.h; ++y) {
        uint16_t* row = canvas->row(r.y + y) + r.x;
        std::fill(row, row + r.w, packed);
    }
}

void Renderer::blendGlyph(Canvas* canvas, const GlyphBitmap& bmp, int16_t x, int16_t y, Color textColor) {
    for (uint16_t r = 0; r < bmp.h; ++r) {
        int16_t py = (int16_t)(y + r);
        if (py < 0 || py >= (int16_t)canvas->height()) continue;

        uint16_t* rowPtr = canvas->row(py);
        for (uint16_t col = 0; col < bmp.w; ++col) {
            int16_t px = (int16_t)(x + col);
            if (px < 0 || px >= (int16_t)canvas->width()) continue;

            uint8_t a = bmp.alpha[(size_t)r * bmp.w + col];
            if (a == 0) continue;

            if (a == 255) {
                rowPtr[px] = textColor.pack();
                continue;
            }

            Color bg = Color::unpack(rowPtr[px]);
            Color fg = textColor;
            fg.a = a;
            rowPtr[px] = fg.blend(bg).pack();
        }
    }
}

void Renderer::drawText(Canvas* canvas, const Text& text, ContentRect box) {
    Font* font = text.font();
    if (!font) return;

    double size = text.size();
    double lineHeight = font->getLineHeight() * size * text.lineSpacing();
    double ascender = font->getAscender() * size;

    double startY = box.y;
    if (text.vAlign() == VerticalAlignment::CENTER) startY = box.y + (box.h - text.height()) / 2.0;
    else if (text.vAlign() == VerticalAlignment::BOTTOM) startY = box.y + box.h - text.height();

    double lineTop = startY;
    for (const TextLine& line : text.lines()) {
        double lineXRaw = box.x;
        if (text.hAlign() == HorizontalAlignment::CENTER) lineXRaw = box.x + (box.w - (int)line.width) / 2.0;
        else if (text.hAlign() == HorizontalAlignment::RIGHT) lineXRaw = box.x + box.w - (int)line.width;
        double lineX = std::round(lineXRaw);

        double baselineY = std::round(lineTop + ascender);

        double cursor = 0;
        for (size_t i = 0; i < line.text.size(); ++i) {
            unsigned char c = (unsigned char)line.text[i];

            if (c == ' ') {
                const qtxt::GlyphRecord* spaceGlyph = font->getGlyph(' ');
                cursor += spaceGlyph ? spaceGlyph->advance * size : size * 0.4;
                continue;
            }

            const qtxt::GlyphRecord* g = font->getGlyph(c);
            if (!g) { cursor += size * 0.4; continue; }

            const GlyphBitmap* bmp = font->getGlyphBitmap(c, size);
            if (bmp && bmp->w > 0 && bmp->h > 0) {
                double glyphXf = lineX + cursor + bmp->planeL;
                double glyphYf = baselineY - bmp->planeT;
                int16_t glyphX = (int16_t)std::ceil(glyphXf);
                int16_t glyphY = (int16_t)std::floor(glyphYf);
                blendGlyph(canvas, *bmp, glyphX, glyphY, text.color());
            }

            cursor += g->advance * size;
            cursor += font->getLetterSpacingRatio() * size;

            if (i + 1 < line.text.size()) {
                unsigned char next = (unsigned char)line.text[i + 1];
                cursor += font->getKerning(c, next) * size;
            }
        }

        lineTop = lineTop + std::ceil(lineHeight);
    }
}

void Renderer::drawBackground(Canvas* canvas, ContentRect r, const Background& bg) {
    if (r.w <= 0 || r.h <= 0) return;

    Image* img = bg.image();
    if (!img || img->width() == 0 || img->height() == 0) {
        drawRect(canvas, r, bg.color());
        return;
    }

    uint32_t imgW = img->width();
    uint32_t imgH = img->height();
    Color tint = bg.color();

    auto blitPixel = [&](int16_t dx, int16_t dy, uint32_t sx, uint32_t sy) {
        if (dx < 0 || dy < 0 || dx >= r.w || dy >= r.h) return;
        int16_t px = (int16_t)(r.x + dx);
        int16_t py = (int16_t)(r.y + dy);
        if (px < 0 || py < 0 || px >= (int16_t)canvas->width() || py >= (int16_t)canvas->height()) return;

        Color fg = img->at(sx, sy) * tint;

        if (fg.a == 0) return;
        if (fg.a == 255) {
            canvas->row((uint16_t)py)[px] = fg.pack();
            return;
        }
        Color background = Color::unpack(canvas->row((uint16_t)py)[px]);
        canvas->row((uint16_t)py)[px] = fg.blend(background).pack();
    };

    switch (bg.fit()) {
        case FitMode::STRETCH: {
            for (int16_t dy = 0; dy < r.h; ++dy) {
                uint32_t sy = (uint32_t)std::min<int64_t>((int64_t)imgH - 1, ((int64_t)dy * imgH) / r.h);
                for (int16_t dx = 0; dx < r.w; ++dx) {
                    uint32_t sx = (uint32_t)std::min<int64_t>((int64_t)imgW - 1, ((int64_t)dx * imgW) / r.w);
                    blitPixel(dx, dy, sx, sy);
                }
            }
            break;
        }
        case FitMode::TILE: {
            for (int16_t dy = 0; dy < r.h; ++dy) {
                uint32_t sy = (uint32_t)dy % imgH;
                for (int16_t dx = 0; dx < r.w; ++dx) {
                    uint32_t sx = (uint32_t)dx % imgW;
                    blitPixel(dx, dy, sx, sy);
                }
            }
            break;
        }
        case FitMode::CENTER: {
            int16_t offX = (int16_t)(((int32_t)r.w - (int32_t)imgW) / 2);
            int16_t offY = (int16_t)(((int32_t)r.h - (int32_t)imgH) / 2);
            for (uint32_t sy = 0; sy < imgH; ++sy) {
                int16_t dy = (int16_t)(offY + sy);
                if (dy < 0 || dy >= r.h) continue;
                for (uint32_t sx = 0; sx < imgW; ++sx) {
                    int16_t dx = (int16_t)(offX + sx);
                    if (dx < 0 || dx >= r.w) continue;
                    blitPixel(dx, dy, sx, sy);
                }
            }
            break;
        }
        case FitMode::CONTAIN:
        case FitMode::COVER: {
            double scaleX = (double)r.w / (double)imgW;
            double scaleY = (double)r.h / (double)imgH;
            double scale = (bg.fit() == FitMode::CONTAIN) ? std::min(scaleX, scaleY) : std::max(scaleX, scaleY);

            int16_t scaledW = (int16_t)std::round(imgW * scale);
            int16_t scaledH = (int16_t)std::round(imgH * scale);
            int16_t offX = (int16_t)((r.w - scaledW) / 2);
            int16_t offY = (int16_t)((r.h - scaledH) / 2);

            for (int16_t dy = 0; dy < scaledH; ++dy) {
                int16_t destY = (int16_t)(offY + dy);
                if (destY < 0 || destY >= r.h) continue;
                uint32_t sy = (uint32_t)std::min<int64_t>((int64_t)imgH - 1, (int64_t)(dy / scale));
                for (int16_t dx = 0; dx < scaledW; ++dx) {
                    int16_t destX = (int16_t)(offX + dx);
                    if (destX < 0 || destX >= r.w) continue;
                    uint32_t sx = (uint32_t)std::min<int64_t>((int64_t)imgW - 1, (int64_t)(dx / scale));
                    blitPixel(destX, destY, sx, sy);
                }
            }
            break;
        }
    }
}

void Renderer::drawImageFrame(Canvas* canvas, ContentRect destRect, Image* image, ContentRect srcRect, Color tint) {
    if (!image || destRect.w <= 0 || destRect.h <= 0 || srcRect.w <= 0 || srcRect.h <= 0) return;

    for (int16_t dy = 0; dy < destRect.h; ++dy) {
        int16_t py = (int16_t)(destRect.y + dy);
        if (py < 0 || py >= (int16_t)canvas->height()) continue;
        uint32_t sy = (uint32_t)(srcRect.y + std::min<int>(srcRect.h - 1, (dy * srcRect.h) / destRect.h));

        for (int16_t dx = 0; dx < destRect.w; ++dx) {
            int16_t px = (int16_t)(destRect.x + dx);
            if (px < 0 || px >= (int16_t)canvas->width()) continue;
            uint32_t sx = (uint32_t)(srcRect.x + std::min<int>(srcRect.w - 1, (dx * srcRect.w) / destRect.w));

            Color fg = image->at(sx, sy) * tint;
            if (fg.a == 0) continue;
            if (fg.a == 255) {
                canvas->row((uint16_t)py)[px] = fg.pack();
                continue;
            }
            Color bg = Color::unpack(canvas->row((uint16_t)py)[px]);
            canvas->row((uint16_t)py)[px] = fg.blend(bg).pack();
        }
    }
}