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
    // EventStatus status = Widget::dispatchEvent(event);

    // TODO: Widget::?
    return mt.processEvent(event, Base::dispatchEvent(event));
}


EVENT_HANDLER_IMPL(Button, MouseMove) {
    return mt.processEvent(event, Base::dispatchEvent(event));
}


EVENT_HANDLER_IMPL(Button, MouseScroll) {
    return mt.processEvent(event, Base::dispatchEvent(event));
}


EVENT_HANDLER_IMPL(Button, Render) {
    return renderCustomized(event, Style::EL_BUTTON, false);
}

Widget::EventStatus Button::renderCustomized(const RenderEvent &event, Style::Element elem,
                                             bool ignoreLabel) {
    // Skipping StaticGroup level, because we want to handle stuff manually
    EventStatus status = Widget::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    Style::ElementState
        lastState = mt.getLastElemState(),
         curState = mt.getElemState();

    anim.maybeStop(curState);

    if (!anim.isGoing() && lastState != curState) {
        double duration = 0;

        if (lastState == Style::ELS_HOVERED && curState == Style::ELS_NORMAL) {
            duration = 0.15;
        } else if (lastState == Style::ELS_PRESSED) {
            duration = 0.10;
        }

        if (sgnDbl(duration) > 0) {
            anim = getStyle().animElement(region, elem, lastState, curState, duration);
            anim.start();
        }
    }

    if (anim.isGoing()) {
        anim.render(event.target, region);
    } else {
        getStyle().drawElement(event.target, region, elem, curState);
    }

    if (ignoreLabel) {
        return EventStatus::done();
    }

    return _dispatchToChildren(event);
}


}
