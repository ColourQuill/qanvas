#ifndef __QANVAS_RENDERER_HPP__
#define __QANVAS_RENDERER_HPP__

// std
#include <cstdint>

// core
#include <qanvas/core/content_rect.hpp>
#include <qanvas/core/color.hpp>
#include <qanvas/core/background.hpp>

// resources
#include <qanvas/resources/font.hpp>
#include <qanvas/resources/text.hpp>
#include <qanvas/resources/image.hpp>

namespace Qore::Qanvas {
    class Canvas;

    class Renderer {
        public:
            static void drawRect(Canvas* canvas, ContentRect r, Color color);
            static void drawText(Canvas* canvas, const Text& text, ContentRect box);
            static void drawBackground(Canvas* canvas, ContentRect r, const Background& bg);
            static void drawImageFrame(Canvas* canvas, ContentRect destRect, Image* image, ContentRect srcRect, Color tint = Color::WHITE);
        private:
            static void blendGlyph(Canvas* canvas, const GlyphBitmap& bmp, int16_t x, int16_t y, Color textColor);
    };
}

#endif