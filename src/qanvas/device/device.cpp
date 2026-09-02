#include <qanvas/device/device.hpp>

// std
#include <cmath>
#include <chrono>
#include <iostream>

// core
#include <qanvas/core/scroll_transform.hpp>

// draw
#include <qanvas/draw/canvas.hpp>
#include <qanvas/draw/canvas_view.hpp>

// command
#include <qanvas/command/exec_context.hpp>
#include <qanvas/command/command_buffer.hpp>

// device
#include <qanvas/device/transport.hpp>

using namespace Qore::Qanvas;

Device::Device(Transport* transport) : m_transport(transport) {}

void Device::submit(CommandBuffer* cmd) {
    ExecContext ctx{this};
    cmd->executeAll(&ctx);
    cmd->clear();
}

void Device::activateScroll(CanvasView* view, int64_t startPosition, ScrollDirection dir) {
    auto* transform = static_cast<ScrollTransform*>(view->transform());

    int16_t visTop; uint16_t visRows;
    if (!view->resolveVisiblePhysicalWindow(&visTop, &visRows)) {
        m_scrollView = nullptr;
        transform->scrollTo(startPosition, dir);
        return;
    }

    m_scrollRegister.configure(m_transport, (uint16_t)view->deviceOriginX(), (uint16_t)visTop, visRows);
    timedReset(view, dir);

    m_scrollView = view;
    transform->scrollTo(startPosition, dir);
    writeFullWindow();
}

void Device::deactivateScroll() {
    if (!m_scrollView) return;

    size_t fullPanelBytes = (size_t)m_transport->height() * m_scrollView->canvas()->width() * 2;
    m_transport->beginTimedWrite(fullPanelBytes);
    flattenActiveScrollView();
    m_transport->setMirrored(false);
    m_transport->defineScrollWindow(0, 0);
    m_transport->setScrollOffset(0);
    m_transport->endTimedWrite();
    m_scrollView = nullptr;
}

bool Device::scrollActive() const {
    return m_scrollView != nullptr;
}

void Device::setPartialBandThreshold(float fraction) {
    m_partialBandThreshold = fraction;
}

void Device::writeClippedToVisibility(CanvasView* view, ContentRect rect, PhysicalRect physical) {
    int16_t visTop; uint16_t visRows;
    if (!view->resolveVisiblePhysicalWindow(&visTop, &visRows)) return;

    int16_t visBottom = (int16_t)(visTop + visRows);
    int16_t writeTop = std::max(physical.y, visTop);
    int16_t writeBottom = std::min((int16_t)(physical.y + physical.h), visBottom);
    if (writeBottom <= writeTop) return;

    int16_t rowsClippedFromTop = writeTop - physical.y;
    ContentRect clippedRect = rect;
    clippedRect.y = (int16_t)(rect.y + rowsClippedFromTop);
    clippedRect.h = writeBottom - writeTop;

    PhysicalRect clippedPhysical = physical;
    clippedPhysical.y = writeTop;
    clippedPhysical.h = writeBottom - writeTop;

    writeDirectRect(view, clippedRect, clippedPhysical);
}

void Device::blit(CanvasView* view, ContentRect rect) {
    if (view == m_scrollView) {
        writeScrollRect(view, rect);
        return;
    }

    PhysicalRect physical[1];
    if (view->mapToPhysical(rect, physical) == 0) return;

    writeClippedToVisibility(view, rect, physical[0]);
}

bool Device::isPartialBand() const {
    return (float)m_scrollRegister.length() < (float)m_transport->height() * m_partialBandThreshold;
}

void Device::writeDirectRect(CanvasView* view, ContentRect rect, PhysicalRect physical) {
    Canvas* canvas = view->canvas();

    if (m_scrollView && view != m_scrollView) {
        int16_t bandTop = (int16_t)m_scrollRegister.top();
        int16_t bandBottom = (int16_t)(bandTop + m_scrollRegister.length());
        int16_t writeTop = physical.y;
        int16_t writeBottom = (int16_t)(physical.y + physical.h);
        int16_t overlapTop = std::max(writeTop, bandTop);
        int16_t overlapBottom = std::min(writeBottom, bandBottom);

        if (overlapBottom > overlapTop) {
            if (writeTop < overlapTop) {
                PhysicalRect above{physical.x, writeTop, physical.w, (int16_t)(overlapTop - writeTop)};
                ContentRect aboveContent = rect;
                aboveContent.h = (int16_t)(overlapTop - writeTop);
                writeDirectRectPlain(view, aboveContent, above);
            }

            int16_t overlapRows = overlapBottom - overlapTop;
            int16_t contentOffset = overlapTop - writeTop;
            std::vector<uint16_t> block((size_t)physical.w * overlapRows);
            for (int16_t i = 0; i < overlapRows; ++i) {
                const uint16_t* src = canvas->row(rect.y + contentOffset + i) + rect.x;
                std::copy(src, src + physical.w, block.data() + (size_t)i * physical.w);
            }
            int16_t bandRelStart = overlapTop - bandTop;
            uint16_t wrappedY = (uint16_t)m_scrollRegister.wrappedAddress(bandRelStart);

            m_scrollRegister.write(m_scrollRegister.direction(), (uint16_t)physical.x, wrappedY, physical.w, (uint16_t)overlapRows, block.data());

            if (writeBottom > overlapBottom) {
                PhysicalRect below{physical.x, overlapBottom, physical.w, (int16_t)(writeBottom - overlapBottom)};
                ContentRect belowContent = rect;
                belowContent.y = (int16_t)(rect.y + (overlapBottom - writeTop));
                belowContent.h = (int16_t)(writeBottom - overlapBottom);
                writeDirectRectPlain(view, belowContent, below);
            }
            return;
        }
    }

    writeDirectRectPlain(view, rect, physical);
}

void Device::writeDirectRectPlain(CanvasView* view, ContentRect rect, PhysicalRect physical) {
    Canvas* canvas = view->canvas();
    bool mirrorActive = m_scrollView && m_scrollRegister.direction() == ScrollDirection::UP;

    if (rect.w == canvas->width() && !mirrorActive) {
        m_transport->writeRect(physical, canvas->row(rect.y));
        return;
    }

    std::vector<uint16_t> block((size_t)rect.w * rect.h);
    for (int16_t i = 0; i < rect.h; ++i) {
        const uint16_t* src = canvas->row(rect.y + i) + rect.x;
        int16_t destRow = mirrorActive ? (rect.h - 1 - i) : i;
        std::copy(src, src + rect.w, block.data() + (size_t)destRow * rect.w);
    }

    PhysicalRect target = physical;
    if (mirrorActive) {
        target.y = (int16_t)(m_transport->height() - physical.y - physical.h);
    }

    m_transport->writeRect(target, block.data());
}

void Device::writeFullWindow() {
    if (!m_scrollView) return;

    int16_t visTop; uint16_t visRows;
    if (!m_scrollView->resolveVisiblePhysicalWindow(&visTop, &visRows)) return;

    Canvas* canvas = m_scrollView->canvas();
    auto* transform = static_cast<ScrollTransform*>(m_scrollView->transform());

    uint16_t w = canvas->width();
    int64_t pos = transform->offset();
    uint16_t rows = (uint16_t)std::min<int64_t>(visRows, canvas->height() - pos);
    if (rows == 0) return;

    ScrollDirection dir = transform->direction();
    std::vector<uint16_t> block((size_t)w * rows);
    for (uint16_t i = 0; i < rows; ++i) {
        int64_t contentRow = (dir == ScrollDirection::DOWN) ? (pos + i) : (pos + rows - 1 - i);
        const uint16_t* row = m_scrollView->sourceRowForContentRow((int16_t)contentRow);
        std::copy(row, row + w, block.data() + (size_t)i * w);
    }
    timedScrollWrite(dir, 0, w, rows, block.data());
}

void Device::writeFullWindowUntimed() {
    if (!m_scrollView) return;

    int16_t visTop; uint16_t visRows;
    if (!m_scrollView->resolveVisiblePhysicalWindow(&visTop, &visRows)) return;

    Canvas* canvas = m_scrollView->canvas();
    auto* transform = static_cast<ScrollTransform*>(m_scrollView->transform());

    uint16_t w = canvas->width();
    int64_t pos = transform->offset();
    uint16_t rows = (uint16_t)std::min<int64_t>(visRows, canvas->height() - pos);
    if (rows == 0) return;

    ScrollDirection dir = transform->direction();
    std::vector<uint16_t> block((size_t)w * rows);
    for (uint16_t i = 0; i < rows; ++i) {
        int64_t contentRow = (dir == ScrollDirection::DOWN) ? (pos + i) : (pos + rows - 1 - i);
        const uint16_t* row = m_scrollView->sourceRowForContentRow((int16_t)contentRow);
        std::copy(row, row + w, block.data() + (size_t)i * w);
    }
    m_scrollRegister.write(dir, 0, w, rows, block.data());
}

void Device::writeScrollRect(CanvasView* view, ContentRect rect) {
    auto* transform = static_cast<ScrollTransform*>(view->transform());
    Canvas* canvas = view->canvas();

    int16_t visTop; uint16_t visRows;
    if (!view->resolveVisiblePhysicalWindow(&visTop, &visRows)) return;

    int64_t winStart = transform->offset();
    int64_t winEnd   = winStart + std::min<uint16_t>(visRows, (uint16_t)m_scrollRegister.length());
    int64_t clipStart = std::max<int64_t>(winStart, rect.y);
    int64_t clipEnd   = std::min<int64_t>(winEnd, rect.y + rect.h);
    int64_t rows = clipEnd - clipStart;
    if (rows <= 0) return;

    uint16_t w = canvas->width();
    std::vector<uint16_t> block((size_t)w * rows);
    ScrollDirection dir = transform->direction();
    uint16_t wrappedY;

    if (dir == ScrollDirection::DOWN) {
        for (int64_t i = 0; i < rows; ++i) {
            const uint16_t* row = view->sourceRowForContentRow((int16_t)(clipStart + i));
            std::copy(row, row + w, block.data() + (size_t)i * w);
        }
        wrappedY = (uint16_t)m_scrollRegister.wrappedAddress(clipStart - winStart);
    } else {
        for (int64_t i = 0; i < rows; ++i) {
            const uint16_t* row = view->sourceRowForContentRow((int16_t)(clipEnd - 1 - i));
            std::copy(row, row + w, block.data() + (size_t)i * w);
        }
        int64_t viewportLocalEnd = clipEnd - winStart;
        wrappedY = (uint16_t)m_scrollRegister.wrappedAddress(m_scrollRegister.length() - viewportLocalEnd);
    }

    if (isPartialBand()) {
        timedScrollWrite(dir, wrappedY, w, (uint16_t)rows, block.data());
    } else {
        m_scrollRegister.write(dir, wrappedY, w, (uint16_t)rows, block.data());
    }
}

void Device::timedScrollWrite(ScrollDirection dir, uint16_t wrappedY, uint16_t w, uint16_t rows, const uint16_t* data) {
    size_t bytes = (size_t)w * rows * 2;
    m_transport->beginTimedWrite(bytes);
    m_scrollRegister.write(dir, wrappedY, w, rows, data);
    m_transport->endTimedWrite();
}

void Device::timedReset(CanvasView* view, ScrollDirection dir) {
    size_t fullPanelBytes = (size_t)m_transport->height() * view->canvas()->width() * 2;
    m_transport->beginTimedWrite(fullPanelBytes);
    m_scrollRegister.reset(dir);
    m_transport->endTimedWrite();
}

void Device::flattenActiveScrollView() {
    if (!m_scrollView) return;

    int16_t visTop; uint16_t visRows;
    if (!m_scrollView->resolveVisiblePhysicalWindow(&visTop, &visRows)) return;

    auto* transform = static_cast<ScrollTransform*>(m_scrollView->transform());
    Canvas* canvas = m_scrollView->canvas();

    uint16_t w = canvas->width();
    int64_t pos = transform->offset();
    uint16_t rows = (uint16_t)std::min<int64_t>(visRows, canvas->height() - pos);
    if (rows == 0) return;

    ScrollDirection dir = transform->direction();
    std::vector<uint16_t> block((size_t)w * rows);
    for (uint16_t i = 0; i < rows; ++i) {
        int64_t contentRow = (dir == ScrollDirection::DOWN) ? (pos + i) : (pos + rows - 1 - i);
        const uint16_t* row = m_scrollView->sourceRowForContentRow((int16_t)contentRow);
        std::copy(row, row + w, block.data() + (size_t)i * w);
    }

    PhysicalRect target{
        m_scrollView->deviceOriginX(),
        visTop,
        (int16_t)w,
        (int16_t)rows
    };

    m_transport->writeRect(target, block.data());
}

// best solution I could come up with for now
void Device::redrawSideStrip(uint16_t x, uint16_t w, const uint16_t* rowMajorData) {
    if (!m_scrollView || w == 0) return;

    uint16_t bandLength = m_scrollRegister.length();
    if (bandLength == 0) return;

    ScrollDirection dir = m_scrollRegister.direction();

    std::vector<uint16_t> block((size_t)w * bandLength);
    for (uint16_t i = 0; i < bandLength; ++i) {
        uint16_t srcRow = (dir == ScrollDirection::DOWN) ? i : (uint16_t)(bandLength - 1 - i);
        const uint16_t* src = rowMajorData + (size_t)srcRow * w;
        std::copy(src, src + w, block.data() + (size_t)i * w);
    }

    uint16_t wrappedY = (uint16_t)m_scrollRegister.wrappedAddress(0);

    m_scrollRegister.write(dir, x, wrappedY, w, bandLength, block.data());
}

void Device::scroll(CanvasView* view, int64_t offset) {
    auto* transform = static_cast<ScrollTransform*>(view->transform());

    int64_t beforeOffset = transform->offset();
    int64_t delta = offset - beforeOffset;
    if (delta == 0) return;

    ScrollDirection dir = delta < 0 ? ScrollDirection::UP : ScrollDirection::DOWN;
    transform->scrollTo(offset, dir);

    int64_t actualDelta = transform->offset() - beforeOffset;
    if (actualDelta == 0) return;

    if (view != m_scrollView) return;

    if (transform->consumeInvalidated()) {
        m_scrollRegister.flip(dir);
    } else {
        int16_t visTop; uint16_t visRows;
        if (!view->resolveVisiblePhysicalWindow(&visTop, &visRows)) {
            return;
        }

        if (visRows != m_scrollRegister.length() || (uint16_t)visTop != m_scrollRegister.top()) {
            m_scrollRegister.configure(m_transport, (uint16_t)view->deviceOriginX(), (uint16_t)visTop, visRows);
            timedReset(view, dir);
            writeFullWindow();
            return;
        }
    }

    uint16_t w = view->canvas()->width();
    uint32_t step = (uint32_t)std::abs(actualDelta);

    int16_t visTop; uint16_t visRows;
    if (!view->resolveVisiblePhysicalWindow(&visTop, &visRows)) return;

    if (step > visRows) {
        timedReset(view, dir);
        writeFullWindow();
        return;
    }

    m_scrollRegister.advance((int64_t)step, dir);

    Canvas* canvas = view->canvas();
    int64_t pos = transform->offset();
    uint16_t leadingEdgeRow = (dir == ScrollDirection::DOWN) ? (uint16_t)(pos + visRows - step) : (uint16_t)pos;

    std::vector<uint16_t> strip((size_t)w * step);
    for (uint32_t i = 0; i < step; ++i) {
        uint16_t contentRow = (dir == ScrollDirection::DOWN) ? (uint16_t)(leadingEdgeRow + i) : (uint16_t)(leadingEdgeRow + step - 1 - i);
        const uint16_t* row = view->sourceRowForContentRow(contentRow);
        std::copy(row, row + w, strip.data() + (size_t)i * w);
    }
    uint16_t wrappedY = (uint16_t)m_scrollRegister.wrappedAddress(visRows - step);

    if (isPartialBand()) {
        timedScrollWrite(dir, wrappedY, w, (uint16_t)step, strip.data());
    } else {
        m_scrollRegister.write(dir, wrappedY, w, (uint16_t)step, strip.data());
    }
}

void Device::mirror(bool mirrored) {
    m_mirrored = mirrored;
    m_transport->setMirrored(mirrored);
}