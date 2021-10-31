#include <AGF/llgui.h>
#include <AGF/widgets/button.h>
#include <ACL/general.h>


namespace abel::gui::widgets {


SimpleButton::SimpleButton(Widget *parent_, const Rect<double> &region_, const char */*text_*/) :
    Widget(parent_, region_), body{new Rectangle(this, region, COL_DEFAULT)} {}


template <>
Widget::EventStatus SimpleButton::_processEvent(const MouseClickEvent &event) {
    assert(body);

    EventStatus status = EVENT_HANDLER_CALL_BASE(Widget, event);

    if (!status.shouldHandle(status.NODE))
        return status.update();

    if (!region.contains(event.pos))
        return status.update();

    if (event.type == decltype(event.type)::Down) {
        body->recolor(COL_PRESSED);
    } else {
        body->recolor(COL_DEFAULT);
        sigClick();
    }

    return EventStatus::stop(EventStatus::TREE);
}


#define EVENTS_DSL_ITEM_(NAME)                  \
    EVENT_HANDLER_IMPL(SimpleButton, NAME) {    \
        return _processEvent(event);            \
    }

#include <AGF/events.dsl.h>


SimpleButton::~SimpleButton() = default;

}
