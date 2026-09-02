#include <qanvas/widgets/grid_keyboard.hpp>

// std
#include <cstring>
#include <cctype>

// widgets
#include <qanvas/widgets/grid_keyboard.hpp>
#include <qanvas/widgets/box_container.hpp>
#include <qanvas/widgets/button.hpp>
#include <qanvas/widgets/label.hpp>

using namespace Qore::Qanvas;

int16_t GridKeyboard::contentHeight(int16_t keyH, int16_t rowGap) {
    return (int16_t)(8 + keyH * 6 + rowGap * 5);
}

GridKeyboard::GridKeyboard(Compositor* compositor, Device* device, PhysicalRect area, Navigator* navigator, Widget* appRoot, Font* font, double keySize, Color textColor, int z, ScrollSuspendPolicy scrollPolicy) : Keyboard(compositor, device, area, navigator, appRoot, z, scrollPolicy), m_font(font), m_keySize(keySize), m_textColor(textColor) {
    int16_t keyH = 22;

    VBoxContainer* root = new VBoxContainer(4);
    root->setHSizeFlags(SizeFlag::FILL);
    add(root);
    root->setRect(ContentRect{0, 0, rect().w, rect().h});

    buildDigitRow(root, keyH);
    buildLetterRow(root, "qwertyuiop", keyH);
    buildLetterRow(root, "asdfghjkl", keyH);
    buildLetterRow(root, "zxcvbnm", keyH);
    buildPunctuationRow(root, keyH);
    buildBottomRow(root, keyH);
}

void GridKeyboard::reset() {
    if (m_altMode) {
        m_altMode = false;
        refreshLabels();
        m_modeButton->setIdleColor(m_modeIdleColor);
    }
}

void GridKeyboard::buildDigitRow(VBoxContainer* root, int16_t keyH) {
    HBoxContainer* row = new HBoxContainer(2);
    row->setHSizeFlags(SizeFlag::FILL);

    static const char* digits = "0123456789";
    for (int i = 0; i < 10; ++i) {
        char c = digits[i];
        Button* key = new Button();
        key->setMinimumSize({0, (uint16_t)keyH});
        key->setHSizeFlags(SizeFlag::EXPAND | SizeFlag::FILL);
        key->setVSizeFlags(SizeFlag::FILL);
        key->setCallback([this, c]() { emitCharacter(c); });
        row->add(key);

        Label* label = new Label(std::string(1, c), m_font, m_keySize, m_textColor);
        label->text().setAlignment(HorizontalAlignment::CENTER);
        label->text().setAlignment(VerticalAlignment::CENTER);
        key->add(label);
    }

    root->add(row);
}

void GridKeyboard::buildLetterRow(VBoxContainer* root, const char* letters, int16_t keyH) {
    HBoxContainer* row = new HBoxContainer(2);
    row->setHSizeFlags(SizeFlag::FILL);

    size_t len = std::strlen(letters);
    for (size_t i = 0; i < len; ++i) {
        char lower = letters[i];
        char upper = (char)std::toupper((unsigned char)lower);

        Button* key = new Button();
        key->setMinimumSize({0, (uint16_t)keyH});
        key->setHSizeFlags(SizeFlag::EXPAND | SizeFlag::FILL);
        key->setVSizeFlags(SizeFlag::FILL);
        row->add(key);

        Label* label = new Label(std::string(1, lower), m_font, m_keySize, m_textColor);
        label->text().setAlignment(HorizontalAlignment::CENTER);
        label->text().setAlignment(VerticalAlignment::CENTER);
        key->add(label);

        size_t idx = m_letterKeys.size();
        m_letterKeys.push_back({label, lower, upper});
        key->setCallback([this, idx]() {
            const LetterKey& k = m_letterKeys[idx];
            emitCharacter(m_altMode ? k.upper : k.lower);
        });
    }

    root->add(row);
}

void GridKeyboard::buildPunctuationRow(VBoxContainer* root, int16_t keyH) {
    HBoxContainer* row = new HBoxContainer(2);
    row->setHSizeFlags(SizeFlag::FILL);

    struct Pair { char primary; char secondary; };
    static const Pair pairs[] = {
        {'.', ';'}, {',', ':'}, {'!', '\''}, {'?', '"'},
        {'-', '_'}, {'/', '@'}
    };

    for (const auto& p : pairs) {
        Button* key = new Button();
        key->setMinimumSize({0, (uint16_t)keyH});
        key->setHSizeFlags(SizeFlag::EXPAND | SizeFlag::FILL);
        key->setVSizeFlags(SizeFlag::FILL);
        row->add(key);

        Label* label = new Label(std::string(1, p.primary), m_font, m_keySize, m_textColor);
        label->text().setAlignment(HorizontalAlignment::CENTER);
        label->text().setAlignment(VerticalAlignment::CENTER);
        key->add(label);

        size_t idx = m_symbolKeys.size();
        m_symbolKeys.push_back({label, p.primary, p.secondary});
        key->setCallback([this, idx]() {
            const SymbolKey& k = m_symbolKeys[idx];
            emitCharacter(m_altMode ? k.secondary : k.primary);
        });
    }

    root->add(row);
}

void GridKeyboard::buildBottomRow(VBoxContainer* root, int16_t keyH) {
    HBoxContainer* row = new HBoxContainer(2);
    row->setHSizeFlags(SizeFlag::FILL);

    m_modeButton = new Button();
    m_modeButton->setMinimumSize({34, (uint16_t)keyH});
    m_modeButton->setHSizeFlags(SizeFlag::EXPAND | SizeFlag::FILL);
    m_modeButton->setVSizeFlags(SizeFlag::FILL);
    m_modeButton->setIdleColor(m_modeIdleColor);
    m_modeButton->setCallback([this]() { toggleMode(); });
    row->add(m_modeButton);
    Label* modeLabel = new Label("shift", m_font, m_keySize, m_textColor);
    modeLabel->text().setAlignment(HorizontalAlignment::CENTER);
    modeLabel->text().setAlignment(VerticalAlignment::CENTER);
    m_modeButton->add(modeLabel);

    Button* space = new Button();
    space->setMinimumSize({60, (uint16_t)keyH});
    space->setHSizeFlags(SizeFlag::EXPAND | SizeFlag::FILL);
    space->setVSizeFlags(SizeFlag::FILL);
    space->setCallback([this]() { emitCharacter(' '); });
    row->add(space);
    Label* spaceLabel = new Label("space", m_font, m_keySize, m_textColor);
    spaceLabel->text().setAlignment(HorizontalAlignment::CENTER);
    spaceLabel->text().setAlignment(VerticalAlignment::CENTER);
    space->add(spaceLabel);

    Button* back = new Button();
    back->setMinimumSize({28, (uint16_t)keyH});
    back->setHSizeFlags(SizeFlag::EXPAND | SizeFlag::FILL);
    back->setVSizeFlags(SizeFlag::FILL);
    back->setCallback([this]() { emitBackspace(); });
    row->add(back);
    Label* backLabel = new Label("back", m_font, m_keySize, m_textColor);
    backLabel->text().setAlignment(HorizontalAlignment::CENTER);
    backLabel->text().setAlignment(VerticalAlignment::CENTER);
    back->add(backLabel);

    Button* done = new Button();
    done->setMinimumSize({28, (uint16_t)keyH});
    done->setHSizeFlags(SizeFlag::EXPAND | SizeFlag::FILL);
    done->setVSizeFlags(SizeFlag::FILL);
    done->setCallback([this]() { emitDone(); });
    row->add(done);
    Label* doneLabel = new Label("done", m_font, m_keySize, m_textColor);
    doneLabel->text().setAlignment(HorizontalAlignment::CENTER);
    doneLabel->text().setAlignment(VerticalAlignment::CENTER);
    done->add(doneLabel);

    root->add(row);
}

void GridKeyboard::toggleMode() {
    m_altMode = !m_altMode;
    refreshLabels();
    m_modeButton->setIdleColor(m_altMode ? m_modeActiveColor : m_modeIdleColor);
}

void GridKeyboard::refreshLabels() {
    for (auto& k : m_letterKeys) {
        k.label->setTextQuiet(std::string(1, m_altMode ? k.upper : k.lower));
    }
    for (auto& k : m_symbolKeys) {
        k.label->setTextQuiet(std::string(1, m_altMode ? k.secondary : k.primary));
    }
    queueSort();
}