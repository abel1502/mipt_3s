#include <AGF/llgui.h>
#include <ACL/general.h>
#include <AGF/widgets/label.h>
#include <AGF/application.h>


namespace abel::gui::widgets {


Label::Label(Widget *parent_, const Rect<double> &region_, const char *text_, double fontSize_, unsigned format_) :
    Widget(parent_, region_), fontSize{fontSize_}, text{text_}, format{format_} {

    updated();
}

void Label::setText(const char *text_) {
    text = text_;

    updated();
}

void Label::setFontSize(double fontSize_) {
    fontSize = fontSize_;

    updated();
}

void Label::resetFormat() {
    format = -1u;

    updated();
}

void Label::setFormat(unsigned format_) {
    format = format_;

    updated();
}

void Label::updated() {
    deducedFontSize = fontSize;

    if (abel::isZero(fontSize)) {
        deducedFontSize = region.h() * 0.8;
    }

    Application::getInstance().demandRedraw();
}

EVENT_HANDLER_IMPL(Label, Render) {
    EventStatus status = Widget::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE))
        return status;

    event.target.setFillColor(Color::BLACK);
    event.target.setFont("SEGOEUI", deducedFontSize);
    event.target.drawText(region, text);
    return EventStatus::done();
}


}
