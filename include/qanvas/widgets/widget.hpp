#ifndef __QANVAS_WIDGET_HPP__
#define __QANVAS_WIDGET_HPP__

// std
#include <cstdint>
#include <vector>

// core
#include <qanvas/core/color.hpp>
#include <qanvas/core/content_rect.hpp>
#include <qanvas/core/size_flag.hpp>
#include <qanvas/core/size.hpp>

namespace Qore::Qanvas {
    class Canvas;
    class CanvasView;
    struct Event;

    class Widget {
        public:
            virtual ~Widget();

            virtual void drawAll(Canvas* canvas, ContentRect canvasSpaceRect);
            virtual void draw(Canvas* canvas, ContentRect canvasSpaceRect);
            virtual bool handle(const Event& e);
            virtual void updateAll(uint32_t deltaMS);
            virtual bool update(uint32_t deltaMS);

            virtual void add(Widget* child);
            virtual void remove(Widget* child);
            void clear();
            bool contains(Widget* w) const;

            Widget* parent() const;
            const std::vector<Widget*>& children() const;

            void setMinimumSize(Size s);
            virtual Size minimumSize() const;

            void setHSizeFlags(SizeFlag flags);
            void setVSizeFlags(SizeFlag flags);
            SizeFlag hSizeFlags() const;
            SizeFlag vSizeFlags() const;

            virtual void setRect(ContentRect r);
            ContentRect rect() const;

            void setVisible(bool visible);
            bool visible() const;

            void setEnabled(bool enabled);
            bool enabled() const;

            CanvasView* canvas() const;
            bool hasCanvas() const;
            void setCanvas(CanvasView* view);
            void setClearColor(Color c);

            virtual bool isFocusStop() const;
            virtual void onEnter(int direction);

            virtual void queueSort();

            static int16_t resolveDeviceOriginX(Widget* parent, ContentRect ownRect);
            static int16_t resolveDeviceOriginY(Widget* parent, ContentRect ownRect);
            static CanvasView* resolveAncestorCanvasView(Widget* parent, ContentRect ownRect, ContentRect* outRect);
            static void collectFocusStops(Widget* root, std::vector<Widget*>* out);
        protected:
            Widget* m_parent = nullptr;
            std::vector<Widget*> m_children;
            CanvasView* m_ownedCanvas = nullptr;
            bool m_needsRedraw = true;

            ContentRect m_rect{0, 0, 0, 0};
            Size m_minSize{0, 0};
            SizeFlag m_hFlags = SizeFlag::SHRINK_BEGIN;
            SizeFlag m_vFlags = SizeFlag::SHRINK_BEGIN;

            bool m_visible = true;
            bool m_enabled = true;

            virtual void requestRedraw();
            void markDirty();
    };
}

#endif