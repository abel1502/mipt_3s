#include <AGF/widget.h>


namespace abel::gui {


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

EVENT_HANDLER_IMPL(Widget, Render) {
    if (!visible)
        return E_CONTINUE;

    return E_SKIP;
}

EVENT_HANDLER_IMPL(Widget, Move) {
    return E_SKIP;
}

EVENT_HANDLER_IMPL(Widget, Resize) {
    region = event.newRegion;

    return E_CONTINUE;
}

EVENT_HANDLER_IMPL(Widget, ActiveStatusUpdate) {
    return E_SKIP;
}

EVENT_HANDLER_IMPL(Widget, Start) {
    return E_SKIP;
}

EVENT_HANDLER_IMPL(Widget, Exit) {
    return E_SKIP;
}

EVENT_HANDLER_IMPL(Widget, MouseClick) {
    return E_SKIP;
}

EVENT_HANDLER_IMPL(Widget, MouseMove) {
    return E_SKIP;
}

EVENT_HANDLER_IMPL(Widget, Keyboard) {
    return E_SKIP;
}

EVENT_HANDLER_IMPL(Widget, User) {
    return E_SKIP;
}

#pragma GCC diagnostic pop


Widget::Widget(Widget *parent_, const Rect<double> &region_) :
    parent{parent_}, region{region_} {}


}
