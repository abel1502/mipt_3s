#include <AGF/llgui.h>
#include <AGF/helpers/mouse_tracker.h>
#include <AGF/application.h>


namespace abel::gui {


Widget::EventStatus MouseClickTracker::processEvent(const MouseClickEvent &event,
                                                    Widget::EventStatus baseStatus) {
    // TODO: Finish this and the rest
    if (!baseStatus.shouldHandle(baseStatus.NODE)) {
        return baseStatus;
    }

    bool hit = widget->hitTest(event.pos);

    if (!hit && !Application::getInstance().isMouseCaptured(widget)) {
        return Widget::EventStatus::skip();
    }

    if (event.type == decltype(event.type)::Down) {
        // DBG("Down %s", event.button == MouseBtn::Left ? "Left" : "Right");

        // May not be a hit, if another button has already been pressed and captured the input
        // assert(!isDown(event.button));
        if (isDown(event.button)) {
            DBG("Double down!");
            return Widget::EventStatus::stop(baseStatus.TREE);
        }

        // I guess I should only capture upon left click
        if (event.button == MouseBtn::Left) {
            Application::getInstance().captureMouse(widget);
        }

        isDown(event.button, true);

        sigDown(event);
    } else if (isDown(event.button)) {
        // DBG("Up   %s", event.button == MouseBtn::Left ? "Left" : "Right");

        if (event.button == MouseBtn::Left) {
            Application::getInstance().releaseMouse(widget);
        }

        isDown(event.button, false);

        sigUp(event);

        if (hit) {
            sigClick(event);
        }
    }

    return Widget::EventStatus::stop(baseStatus.TREE);
}


Widget::EventStatus MouseTracker::processEvent(const MouseClickEvent &event,
                                               Widget::EventStatus baseStatus) {
    bool wasDown = isDown(event.button);

    baseStatus = MouseClickTracker::processEvent(event, baseStatus);

    if (wasDown != isDown(event.button)) {
        sigDragStateChange(event.button, event.attrs, isDown(event.button));
    }

    return baseStatus;
}


Widget::EventStatus MouseTracker::processEvent(const MouseMoveEvent &event,
                                               Widget::EventStatus baseStatus) {
    constexpr MouseBtn BUTTONS[MOUSE_BTN_CNT] = {MouseBtn::Left, MouseBtn::Middle, MouseBtn::Right};

    if (!baseStatus.shouldHandle(baseStatus.NODE)) {
        return baseStatus;
    }

    lastMoveType = event.type;

    bool hoverHit = widget->hitTest(event.pos1);

    if (isHovered() != hoverHit) {
        isHovered(hoverHit);

        sigHoverStateChange(hoverHit);
    }

    bool hit = widget->hitTest(event.getCutoffPos());

    if (!hit && !Application::getInstance().isMouseCaptured(widget)) {
        return Widget::EventStatus::skip();
    }

    if (event.isFrom()) {
        for (MouseBtn btn : BUTTONS) {
            if (!isDown(btn))
                continue;

            sigDrag(btn, event);
        }
    }

    return Widget::EventStatus::stop(baseStatus.TREE);
}

void MouseTracker::updateHovered() const {
    // If we didn't receive a move-to event, this means it was screened by another widget, so
    // we aren't hovered anymore
    if (!lastMoveType) {
        isHovered_ = false;
    }
}


}