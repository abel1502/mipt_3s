#include <AGF/llgui.h>
#include <AGF/helpers/mouse_tracker.h>
#include <AGF/application.h>


namespace abel::gui {


bool MouseClickTracker::processEvent(const MouseClickEvent &event) {
    bool hit = widget->hitTest(event.pos);

    if (!hit && !Application::getInstance().isMouseCaptured(widget))
        return false;

    if (event.type == decltype(event.type)::Down) {
        // DBG("Down %s", event.button == MouseBtn::Left ? "Left" : "Right");

        // May not be a hit, if another button has already been pressed and captured the input
        // assert(!isDown(event.button));
        if (isDown(event.button)) {
            DBG("Double down!");
            return true;
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

    return true;
}


bool MouseTracker::processEvent(const MouseClickEvent &event) {
    bool hit = MouseClickTracker::processEvent(event);

    if (hit) {
        sigDragStateChange(event.button, event.attrs, event.type == MouseClickType::Down);
    }

    return hit;
}


bool MouseTracker::processEvent(const MouseMoveEvent &event) {
    constexpr MouseBtn BUTTONS[MOUSE_BTN_CNT] = {MouseBtn::Left, MouseBtn::Middle, MouseBtn::Right};

    isHovered(widget->hitTest(event.pos1));

    sigHoverStateChange(isHovered());

    for (MouseBtn btn : BUTTONS) {
        if (!isDown(btn))
            continue;

        sigDrag(btn, event);
    }

    // TODO: Rework after MouseMoveEvent change
    return widget->hitTest(event.pos0) && isHovered();
}

void MouseTracker::updateHovered() {
    isHovered(isHovered() && widget->hitTest(Window::getInstance().getMousePos()));
}


}