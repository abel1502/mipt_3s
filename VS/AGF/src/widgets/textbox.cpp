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

    pos = (pos + mod + (direction % mod)) % mod;
}
#pragma endregion Caret


#pragma region TextBox
TextBox::TextBox(Widget *parent_, const Rect<double> &region_,
                 const char *defaultText) :
    Base(parent_, region_), text{defaultText} {}

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

    event.target.clipPop();
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
    if (event.isAscii()) {
        char chr = event.getAscii();

        for (unsigned i = 0; i < event.repeatCnt; ++i) {
            addChar(chr);
        }
    }
}

EVENT_HANDLER_IMPL(TextBox, KeyPress) {
    //
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
#pragma endregion TextBox


}
