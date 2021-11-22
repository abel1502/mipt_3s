#include <AGF/llgui.h>
#include <AGF/widgets/slider.h>


namespace abel::gui::widgets {


#pragma region Thumb
Thumb::Thumb(Widget *parent_, const Rect<double> &region_) :
    Base(parent_, region_),
    pos{region_.getCenter()} {

    mt.sigDrag += [this](MouseBtn btn, const MouseMoveEvent &event) {
        const Rect<double> &bounds = getBounds();
        Vector2d destPos = event.pos1.clamped(bounds.getStart(), bounds.getEnd());

        dispatchEvent(MoveEvent(destPos - pos));

        return false;
    };
}

EVENT_HANDLER_IMPL(Thumb, Render) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    event.target.setLineColor(Color::WHITE * 0.8f);
    event.target.drawEllipse(pos, region.getDiag(), false);

    return EventStatus::done();
}

EVENT_HANDLER_IMPL(Thumb, MouseClick) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    if (!mt.processEvent(event)) {
        return status;
    }

    return EventStatus::stop(EventStatus::TREE);
}

EVENT_HANDLER_IMPL(Thumb, MouseMove) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    if (!mt.processEvent(event)) {
        return status;
    }

    return EventStatus::stop(EventStatus::TREE);
}

bool Thumb::staticShift(const Vector2d &by) {
    if (Base::staticShift(by)) {
        return true;
    }

    pos += by;

    return false;
}
#pragma endregion Thumb


}
