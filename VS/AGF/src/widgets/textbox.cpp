#include <AGF/llgui.h>
#include <AGF/widgets/textbox.h>
#include <AGF/application.h>


namespace abel::gui::widgets {


#pragma region Caret
TextBox::Caret::Caret(TextBox &textBox_) :
    textBox{textBox_} {}

void TextBox::Caret::shift(int direction) {
    const unsigned mod = textBox.getLength() + 1;

    assert(mod);

    pos = (pos + mod + direction) % mod;
}
#pragma endregion Caret


#pragma region TextBox
TextBox::TextBox(Widget *parent_, const Rect<double> &region_,
                 const char *defaultText) :
    Base(parent_, region_), text{defaultText} {

    // TODO: Hook mt signals, make scrollable
}

EVENT_HANDLER_IMPL(TextBox, MouseClick) {
    return mt.processEvent(event, Base::dispatchEvent(event));
}

EVENT_HANDLER_IMPL(TextBox, MouseMove) {
    return mt.processEvent(event, Base::dispatchEvent(event));
}

EVENT_HANDLER_IMPL(TextBox, MouseScroll) {
    return mt.processEvent(event, Base::dispatchEvent(event));
}

EVENT_HANDLER_IMPL(TextBox, Render) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    event.target.clipPush(region);

    Style &style = getStyle();

    Style::ElementState elemState = mt.getElemState();
    if (isActive()) {
        elemState = style.ELS_ACTIVE;
    }

    style.drawElement(event.target, region, style.EL_TEXTBOX_BODY, elemState);

    shiftToShowCaret();

    Rect<double> textRegion = region.padded(2);

    event.target.clipPush(textRegion);
    event.target.drawText(textRegion + textOffset, text.data(), Texture::TAL_LEFT);
    event.target.clipPop();

    // TODO: Caret
    if (isActive()) {
        style.drawElement(event.target, getCaretRect(), style.EL_TEXTBOX_CARET, style.ELS_NORMAL);
    }

    event.target.clipPop();

    return status;
}

EVENT_HANDLER_IMPL(TextBox, FocusUpdate) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    maybeChangeActive(event.focus);

    return status;
}

EVENT_HANDLER_IMPL(TextBox, KeyboardInput) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    if (!maybeCaptureKbd()) {
        return status;
    }

    if (event.isAscii()) {
        char chr = event.getAscii();

        for (unsigned i = 0; i < event.repeatCnt; ++i) {
            addChar(chr);
        }
    }

    return status;
}

EVENT_HANDLER_IMPL(TextBox, KeyPress) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    if (event.type == KeyPressType::Up) {
        return status;
    }

    switch (event.key) {
    case Key::Escape:
        DBG("esc");
        maybeReleaseKbd();
        break;

    case Key::Backspace:
        DBG("bksp");
        remChar();
        break;

    case Key::Delete:
        DBG("del");
        remChar(true);
        break;

    case Key::Left:
        DBG("left");
        --caret;
        break;

    case Key::Right:
        DBG("right");
        ++caret;
        break;

    case Key::Return:
        DBG("enter");
        sigSubmit(getText());
        maybeReleaseKbd();
        break;

    default:
        break;
    }

    DBG("Pos: %u/%u", caret.getPos(), text.size());

    return status;
}

bool TextBox::isActive() const {
    return Application::getInstance().isKbdCaptured(this);
}

void TextBox::maybeChangeActive(bool active) {
     bool wasActive = isActive();

     if (active && !wasActive) {
         Application::getInstance().captureKbd(this);
         return;
     }

     if (!active && wasActive) {
         Application::getInstance().releaseKbd(this);
         return;
     }
}

bool TextBox::maybeCaptureKbd() {
    if (!Application::getInstance().isKbdCaptured()) {
        Application::getInstance().captureKbd(this);
    }

    return isActive();
}

void TextBox::maybeReleaseKbd() {
    if (Application::getInstance().isKbdCaptured(this)) {
        Application::getInstance().releaseKbd(this);
    }
}

void TextBox::setText(const char *text_) {
    unsigned pos = caret.getPos();

    text = text_;

    if (pos > text.size()) {
        pos = (unsigned)text.size();
    }
}

Rect<double> TextBox::getCharRect(unsigned idx) const {
    Texture tmp{Vector2d{1}};

    assert(idx <= text.size());

    std::string textCopy{text.begin(), text.begin() + idx};
    Vector2d size0 = tmp.getTextSize(textCopy.data());

    textCopy += (idx == text.size() ? ' ' : text[idx]);
    Vector2d size1 = tmp.getTextSize(textCopy.data());
    double height = std::max(size0.y(), size1.y());
    size0.y() = 0;
    size1.y() = height;

    return Rect<double>::wh(region.getPos() + textOffset + size0, size1 - size0);
}

Rect<double> TextBox::getTextRect() const {
    Texture tmp{Vector2d{1}};

    return Rect<double>::wh(region.getPos() + textOffset, tmp.getTextSize(text.data()));
}

void TextBox::shiftToShowCaret() {
    Rect<double> caretRect = getCaretRect();
    Rect<double> destRect = region.paddedH(2);

    if (destRect.contains(caretRect.getPos())) {
        return;
    }

    double offset = 0;
    if (cmpDbl(caretRect.x(), destRect.x0()) <= 0) {
        // We're too far to the left
        offset = destRect.x0() - caretRect.x0();
    } else {
        // We're too far to the right
        offset = destRect.x1() - caretRect.x1();
    }

    textOffset.x() = offset;
}

void TextBox::addChar(char chr) {
    DBG("+char %hhu", chr);
    text.insert(text.begin() + caret.getPos(), chr);
    ++caret;
}

void TextBox::remChar(bool fwd) {
    DBG("-char %s", fwd ? "fwd" : "bck");
    if (fwd) {
        if (caret.getPos() == text.size()) {
            return;
        }

        text.erase(text.begin() + caret.getPos());
    } else {
        if (caret.getPos() == 0) {
            return;
        }

        --caret;
        text.erase(text.begin() + caret.getPos());
    }
}
#pragma endregion TextBox


}
