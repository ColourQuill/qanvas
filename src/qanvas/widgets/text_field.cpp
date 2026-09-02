#include <qanvas/widgets/text_field.hpp>

// events
#include <qanvas/events/event.hpp>
#include <qanvas/events/widget_event.hpp>
#include <qanvas/events/input_event.hpp>

// draw
#include <qanvas/draw/renderer.hpp>

// widgets
#include <qanvas/widgets/keyboard.hpp>
#include <qanvas/widgets/navigator.hpp>
#include <qanvas/widgets/scroll_container.hpp>

using namespace Qore::Qanvas;

TextField::TextField(Keyboard* keyboard, Font* font, double size, Color color) : m_keyboard(keyboard), m_text("", font, size, color), m_placeholderText("", font, size, m_placeholderColor) {}

const std::string& TextField::value() const {
    return m_value;
}

void TextField::setValue(const std::string& v) {
    m_value = v;
    m_text.setContent(v);
    requestRedraw();
}

void TextField::setOnChanged(std::function<void(const std::string&)> cb) {
    m_onChanged = std::move(cb);
}

void TextField::setPlaceholder(const std::string& text) {
    m_placeholderText.setContent(text);
    requestRedraw();
}
void TextField::setPlaceholderColor(Color c) {
    m_placeholderColor = c;
    requestRedraw();
}

void TextField::setPadding(Padding p) {
    m_padding = p;
    requestRedraw();
}
void TextField::setPadding(int16_t uniform) {
    setPadding(Padding::uniform(uniform));
}

void TextField::setIdleColor(Color c) {
    m_idleBackground.setColor(c);
    requestRedraw();
}
void TextField::setIdleImage(Image* image, FitMode fit) {
    m_idleBackground.setImage(image);
    m_idleBackground.setFit(fit);
    requestRedraw();
}
void TextField::setIdleBackground(const Background& bg) {
    m_idleBackground = bg;
    requestRedraw();
}

void TextField::setSelectedColor(Color c) {
    m_selectedBackground.setColor(c);
    requestRedraw();
}
void TextField::setSelectedImage(Image* image, FitMode fit) {
    m_selectedBackground.setImage(image);
    m_selectedBackground.setFit(fit);
    requestRedraw();
}
void TextField::setSelectedBackground(const Background& bg) {
    m_selectedBackground = bg;
    requestRedraw();
}

void TextField::setEditingColor(Color c) {
    m_editingBackground.setColor(c);
    requestRedraw();
}
void TextField::setEditingImage(Image* image, FitMode fit) {
    m_editingBackground.setImage(image);
    m_editingBackground.setFit(fit);
    requestRedraw();
}
void TextField::setEditingBackground(const Background& bg) {
    m_editingBackground = bg;
    requestRedraw();
}

bool TextField::isFocusStop() const {
    return true;
}

void TextField::appendChar(char c) {
    m_value.push_back(c);
    m_text.setContent(m_value);
    requestRedraw();
    if (m_onChanged) m_onChanged(m_value);
}

void TextField::backspace() {
    if (m_value.empty()) return;
    m_value.pop_back();
    m_text.setContent(m_value);
    requestRedraw();
    if (m_onChanged) m_onChanged(m_value);
}

void TextField::resolveAncestors() {
    if (m_enclosingScroller) return;
    for (Widget* w = m_parent; w; w = w->parent()) {
        if (auto* sc = dynamic_cast<ScrollContainer*>(w)) {
            m_enclosingScroller = sc;
            break;
        }
    }
}

void TextField::openKeyboard() {
    resolveAncestors();

    m_keyboard->setOnCharacter([this](char c) { appendChar(c); });
    m_keyboard->setOnBackspace([this]() { backspace(); });
    m_keyboard->setOnDone([this]() { onKeyboardClosed(); });

    m_editing = true;
    requestRedraw();

    m_keyboard->open(this);
}

void TextField::onKeyboardClosed() {
    m_editing = false;
    requestRedraw();
    if (m_enclosingScroller) m_enclosingScroller->selectChild(this);
}

bool TextField::handle(const Event& e) {
    if (e.type() == EventType::WIDGET) {
        const auto& we = static_cast<const WidgetEvent&>(e);
        if (we.action == WidgetAction::FOCUS_GAINED) {
            m_focused = true;
            requestRedraw();
            return true;
        }
        if (we.action == WidgetAction::FOCUS_LOST) {
            m_focused = false;
            requestRedraw();
            return true;
        }
        return false;
    }

    if (e.type() == EventType::INPUT) {
        const auto& ie = static_cast<const InputEvent&>(e);
        if (ie.action == InputAction::BUTTON_PRESS && !m_editing) {
            openKeyboard();
            return true;
        }
    }
    return false;
}


void TextField::draw(Canvas* canvas, ContentRect r) {
    const Background& bg = m_editing ? m_editingBackground : (m_focused ? m_selectedBackground : m_idleBackground);
    Renderer::drawBackground(canvas, r, bg);

    Font* font = m_text.font();
    int16_t baselineY = r.y + 4;
    if (font) baselineY = (int16_t)(r.y + 4 + font->getAscender() * m_text.size());

    if (m_value.empty() && !m_placeholderText.content().empty()) {
        Renderer::drawText(canvas, m_placeholderText, ContentRect{(int16_t)(r.x + 4), (int16_t)(r.y + 4), (int16_t)(r.w - 8), (int16_t)(r.h - 8)});
    } else {
        Renderer::drawText(canvas, m_text, ContentRect{(int16_t)(r.x + 4), (int16_t)(r.y + 4), (int16_t)(r.w - 8), (int16_t)(r.h - 8)});
    }
}