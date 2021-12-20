#include <AGF/llgui.h>
#include <ACL/general.h>
#include <AGF/widgets/label.h>
#include <AGF/application.h>


namespace abel::gui::widgets {


Label::Label(Widget *parent_, const Rect<double> &region_, const char *text_) :
    Widget(parent_, region_), text{text_} {

    updated();
}

void Label::setText(const char *text_) {
    text = text_;

    updated();
}

void Label::updated() {
    Application::getInstance().demandRedraw();
}

EVENT_HANDLER_IMPL(Label, Render) {
    EventStatus status = Widget::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE))
        return status;

    event.target.setFillColor(Color::BLACK);
    event.target.setFont("Segoe UI", getStyle().textSize);
    event.target.drawText(region, text.data());
    return EventStatus::done();
}


}
