#include <ACL/general.h>
#include <AGF/widgets/button.h>


namespace abel::gui::widgets {


SimpleButton::SimpleButton(Widget *parent_, const Rect<double> &region_, const char */*text_*/) :
    Widget(parent_, region_), body{new Rectangle(this, region, COL_DEFAULT)} {}


template <>
Widget::EventStatus SimpleButton::_processEvent(const MouseClickEvent &event) {
    assert(body);

    EventStatus status = EVENT_HANDLER_CALL_BASE(Widget, event);

    assert(status == E_SKIP);

    if (!region.contains(event.pos)) {
        return E_CONTINUE;
    }

    sigClick();
    return E_STOP_GLOBAL;
}


#define EVENTS_DSL_ITEM_(NAME)                  \
    EVENT_HANDLER_IMPL(SimpleButton, NAME) {    \
        return _processEvent(event);            \
    }

#include <AGF/events.dsl.h>


SimpleButton::~SimpleButton() = default;

}
