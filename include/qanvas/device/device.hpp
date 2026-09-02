#ifndef __QANVAS_DEVICE_HPP__
#define __QANVAS_DEVICE_HPP__

// std
#include <cstdint>

// core
#include <qanvas/core/content_rect.hpp>
#include <qanvas/core/physical_rect.hpp>

// device
#include <qanvas/device/scroll_register.hpp>

namespace Qore::Qanvas {
    class Transport;
    class CanvasView;
    class CommandBuffer;

    class Device {
        public:
            explicit Device(Transport* transport);

            void submit(CommandBuffer* cmd);

            void blit(CanvasView* view, ContentRect rect);
            void scroll(CanvasView* view, int64_t offset);
            void mirror(bool mirrored);

            void activateScroll(CanvasView* view, int64_t startPosition, ScrollDirection dir);
            void deactivateScroll();
            bool scrollActive() const;

            void setPartialBandThreshold(float fraction);
            void redrawSideStrip(uint16_t x, uint16_t w, const uint16_t* rowMajorData);
        private:
            bool isPartialBand() const;
            void writeFullWindow();
            void writeFullWindowUntimed();
            void writeScrollRect(CanvasView* view, ContentRect rect);
            void writeDirectRect(CanvasView* view, ContentRect rect, PhysicalRect physical);
            void writeDirectRectPlain(CanvasView* view, ContentRect rect, PhysicalRect physical);
            void timedScrollWrite(ScrollDirection dir, uint16_t wrappedY, uint16_t w, uint16_t rows, const uint16_t* data);
            void timedReset(CanvasView* view, ScrollDirection dir);
            void flattenActiveScrollView();
            void writeClippedToVisibility(CanvasView* view, ContentRect rect, PhysicalRect physical);

            Transport* m_transport;
            bool m_mirrored = false;
            float m_partialBandThreshold = 0.9f;

            ScrollRegister m_scrollRegister;
            CanvasView* m_scrollView = nullptr;
    };  
}

#endif