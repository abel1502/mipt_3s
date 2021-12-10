#include <AGF/llgui.h>
#include "donut.h"


Donut::Donut(Widget *parent_, const Rect<double> &region_) :
    Base(parent_, region_, "") {}

EVENT_HANDLER_IMPL(Donut, abel::gui::Render) {
    abel::gui::Texture newTarget{region};

    abel::gui::RenderEvent newEvent{newTarget};

    EventStatus status = Base::dispatchEvent(newEvent);
    assert(status.shouldHandle(status.NODE));

    newTarget.setOverwrite();
    newTarget.setFillColor(Color::WHITE, 0);
    newTarget.drawRect(getHole(), true);
    newTarget.resetOverwrite();
    event.target.embed(region, newTarget);

    return status;
}

bool Donut::hitTest(const Vector2d &pos) const {
    return Base::hitTest(pos) && !getHole().contains(pos);
}

Rect<double> Donut::getHole() const {
    return region.padded(region.w() / 4, region.h() / 4);
}