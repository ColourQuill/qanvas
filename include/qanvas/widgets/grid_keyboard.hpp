#ifndef __QANVAS_GRID_KEYBOARD_HPP__
#define __QANVAS_GRID_KEYBOARD_HPP__

// std
#include <vector>

// core
#include <qanvas/core/color.hpp>

// widgets
#include <qanvas/widgets/keyboard.hpp>

namespace Qore::Qanvas {
    class Font;
    class Button;
    class Label;
    class VBoxContainer;

    class GridKeyboard : public Keyboard {
        public:
            GridKeyboard(Compositor* compositor, Device* device, PhysicalRect area, Navigator* navigator, Widget* appRoot, Font* font, double keySize = 12.0, Color textColor = Color::WHITE, int z = 1, ScrollSuspendPolicy scrollPolicy = ScrollSuspendPolicy::SUSPEND_INTERSECTING);
            static int16_t contentHeight(int16_t keyH = 22, int16_t rowGap = 4);

            void reset() override;
        private:
            struct LetterKey {
                Label* label;
                char lower;
                char upper;
            };
            struct SymbolKey {
                Label* label;
                char primary;
                char secondary;
            };
            void buildDigitRow(VBoxContainer* root, int16_t keyH);
            void buildLetterRow(VBoxContainer* root, const char* letters, int16_t keyH);
            void buildPunctuationRow(VBoxContainer* root, int16_t keyH);
            void buildBottomRow(VBoxContainer* root, int16_t keyH);
            void toggleMode();
            void refreshLabels();

            Font* m_font;
            double m_keySize;
            Color m_textColor;

            std::vector<LetterKey> m_letterKeys;
            std::vector<SymbolKey> m_symbolKeys;

            Button* m_modeButton = nullptr;
            bool m_altMode = false;
            Color m_modeIdleColor{45, 45, 48};
            Color m_modeActiveColor{0, 122, 204};
    };
}

#endif