#include <qanvas/widgets/animation.hpp>

// std
#include <algorithm>

// draw
#include <qanvas/draw/renderer.hpp>

using namespace Qore::Qanvas;

Animation::Animation(Image* image, uint16_t frameWidth, uint16_t frameHeight, uint16_t frameCount, uint16_t frameDurationMS, bool loop) : m_image(image), m_frameWidth(frameWidth), m_frameHeight(frameHeight), m_frameCount(frameCount), m_frameDurationMS(frameDurationMS), m_loop(loop) {}

bool Animation::update(uint32_t deltaMS) {
    if (!m_playing || m_frameCount <= 1 || m_frameDurationMS == 0) return false;

    m_elapsedMS += deltaMS;
    if (m_elapsedMS < m_frameDurationMS) return false;

    bool changed = false;
    bool justCompleted = false;

    while (m_elapsedMS >= m_frameDurationMS && m_playing) {
        m_elapsedMS -= m_frameDurationMS;

        if ((uint16_t)(m_currentFrame + 1) >= m_frameCount) {
            if (m_loop) {
                m_currentFrame = 0;
                changed = true;
            } else {
                m_playing = false;
                m_elapsedMS = 0;
                justCompleted = true;
            }
        } else {
            m_currentFrame++;
            changed = true;
        }
    }

    if (changed) requestRedraw();
    if (justCompleted && m_onComplete) m_onComplete();
    return changed;
}

void Animation::draw(Canvas* canvas, ContentRect canvasSpaceRect) {
    if (!m_image) return;
    ContentRect src{
        (int16_t)(m_currentFrame * m_frameWidth),
        0,
        (int16_t)m_frameWidth,
        (int16_t)m_frameHeight
    };
    Renderer::drawImageFrame(canvas, canvasSpaceRect, m_image, src);
}

Size Animation::minimumSize() const {
    Size custom = Widget::minimumSize();
    return {std::max(custom.w, m_frameWidth), std::max(custom.h, m_frameHeight)};
}

void Animation::play() {
    m_playing = true;
}
void Animation::pause() {
    m_playing = false;
}
void Animation::stop() {
    m_playing = false;
    if (m_currentFrame != 0) requestRedraw();
    m_currentFrame = 0;
    m_elapsedMS = 0;
}
bool Animation::playing() const {
    return m_playing;
}

uint16_t Animation::currentFrame() const {
    return m_currentFrame;
}
void Animation::setFrame(uint16_t index) {
    if (m_frameCount == 0) return;
    if (index >= m_frameCount) index = (uint16_t)(m_frameCount - 1);
    if (index == m_currentFrame) return;
    m_currentFrame = index;
    m_elapsedMS = 0;
    requestRedraw();
}

void Animation::setLoop(bool loop) {
    m_loop = loop;
}
bool Animation::loop() const {
    return m_loop;
}

void Animation::setFrameDuration(uint16_t ms) {
    m_frameDurationMS = ms;
}

void Animation::setOnComplete(std::function<void()> cb) {
    m_onComplete = std::move(cb);
}