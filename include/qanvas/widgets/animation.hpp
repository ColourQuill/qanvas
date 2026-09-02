#ifndef __QANVAS_ANIMATION_HPP__
#define __QANVAS_ANIMATION_HPP__

// std
#include <cstdint>
#include <functional>

// widgets
#include <qanvas/widgets/widget.hpp>

namespace Qore::Qanvas {
    class Image;

    class Animation : public Widget {
        public:
            Animation(Image* image, uint16_t frameWidth, uint16_t frameHeight, uint16_t frameCount, uint16_t frameDurationMS, bool loop = true);

            bool update(uint32_t deltaMS) override;
            void draw(Canvas* canvas, ContentRect canvasSpaceRect) override;
            Size minimumSize() const override;

            void play();
            void pause();
            void stop();
            bool playing() const;

            uint16_t currentFrame() const;
            void setFrame(uint16_t index);

            void setLoop(bool loop);
            bool loop() const;

            void setFrameDuration(uint16_t ms);

            void setOnComplete(std::function<void()> cb);
        private:
            Image* m_image;
            uint16_t m_frameWidth;
            uint16_t m_frameHeight;
            uint16_t m_frameCount;
            uint16_t m_frameDurationMS;
            bool m_loop;

            uint16_t m_currentFrame = 0;
            uint32_t m_elapsedMS = 0;
            bool m_playing = true;

            std::function<void()> m_onComplete;
    };
}

#endif