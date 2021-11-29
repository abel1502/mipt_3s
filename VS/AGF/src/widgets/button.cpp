#include <AGF/llgui.h>
#include <AGF/widgets/button.h>
#include <ACL/general.h>
#include <AGF/application.h>


namespace abel::gui::widgets {


Button::Button(Widget *parent_, const Rect<double> &region_, const char *text_) :
    Base(parent_, region_,
         new Label(this, region_, text_)) {

    mt.sigClick += [this](const MouseClickEvent &event) {
        if (event.button == MouseBtn::Left) {
            sigClick();
        }

        return false;
    };
}


EVENT_HANDLER_IMPL(Button, MouseClick) {
    // DBG("[%1zu] %4s at (%3lg %3lg)<=((%3lg %3lg))<=(%3lg %3lg) - %s",
    //     ((size_t)this) % 7,
    //     event.type == decltype(event.type)::Down ? "down" : "up",
    //     region.getStart().x(), region.getStart().y(),
    //     event.pos.x(), event.pos.y(),
    //     region.getEnd().x(), region.getEnd().y(),
    //     region.contains(event.pos) ? "hit" : "miss");

    assert(areChildrenSet());

    // Intentionally skipping StaticGroup::dispatchEvent, not to pass this to our children
    EventStatus status = Widget::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    if (!mt.processEvent(event)) {
        return status;
    }

    return EventStatus::stop(EventStatus::TREE);
}


EVENT_HANDLER_IMPL(Button, MouseMove) {
    EventStatus status = StaticGroup::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    if (!mt.processEvent(event)) {
        return status;
    }

    // TODO: Maybe validate that we aren't covered by another widget?
    // TODO: Add a timer to the hovered state

    // TODO: Encapsulate into a widget method?
    // And finish the MouseMove screening logic in general (add this handler to window borders)
    // Removed for now, probably forever
    // if (hitTest(event.pos0) && hitTest(event.pos1))
    //     return EventStatus::stop(EventStatus::TREE);

    return EventStatus::stop(EventStatus::TREE);
}


EVENT_HANDLER_IMPL(Button, Render) {
    return renderCustomized(event, Style::EL_BUTTON, false);
}

Widget::EventStatus Button::renderCustomized(const RenderEvent &event, Style::Element elem, bool ignoreLabel) {
    // Skipping StaticGroup level, because we want to handle stuff manually
    EventStatus status = Widget::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    getStyle().drawElement(event.target, region, elem, mt.getElemState());

    if (ignoreLabel) {
        return EventStatus::done();
    }

    return _dispatchToChildren(event);
}


}
