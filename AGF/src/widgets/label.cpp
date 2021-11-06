#include <AGF/llgui.h>
#include <ACL/general.h>
#include <AGF/widgets/label.h>


namespace abel::gui::widgets {


Label::Label(Widget *parent_, const Rect<double> &region_, const char *text_, double fontSize_) :
    Widget(parent_, region_), fontSize{fontSize_}, text{text_} {

    bakeFont();
}

void Label::updateText(const char *text_) {
    text = text_;

    bakeFont();
}

void Label::bakeFont() {
    // Empty for now, but may get populated eventually
}

EVENT_HANDLER_IMPL(Label, Render) {
    EventStatus status = EVENT_HANDLER_CALL_BASE(Widget, event);

    if (!status.shouldHandle(status.NODE))
        return status.update();

    event.target.setFont("SEGOEUI", fontSize);
    event.target.drawText(event.region.relRect(region), text);
    return status.update(true);
}


}
