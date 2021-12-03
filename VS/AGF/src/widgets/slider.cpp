#include <AGF/llgui.h>
#include <AGF/widgets/slider.h>


namespace abel::gui::widgets {


#pragma region Thumb
Thumb::Thumb(Widget *parent_, const Vector2d &pos_, bool lockV, bool lockH) :
    Base(parent_, Rect<double>::se(pos_ - Vector2d{DEFAULT_SIZE / 2},
                                   pos_ + Vector2d{DEFAULT_SIZE / 2})),
    pos{pos_},
    locked{lockV, lockH} {

    mt.sigDrag += [this](MouseBtn btn, const MouseMoveEvent &event) {
        dispatchEvent(MoveEvent(clampPos(event.pos1) - pos /* + Vector2d {1, 0}*/));

        return false;
    };
}

EVENT_HANDLER_IMPL(Thumb, Render) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    renderThumb(event.target, region);

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

EVENT_HANDLER_IMPL(Thumb, Move) {
    Vector2d newPos = clampPos(pos + event.delta);

    MoveEvent newEvent{event};
    newEvent.delta = newPos - pos;

    pos = newPos;

    EventStatus status = Base::dispatchEvent(newEvent);
    assert(status.shouldHandle(status.NODE));

    sigMove(*this);

    return status;
}

bool Thumb::setStyle(StyleManager::StyleHandle newHandle) {
    if (Base::setStyle(newHandle)) {
        return true;
    }

    Vector2d diag{getStyle().sliderThumbSize / 2.};

    region = Rect<double>::se(pos - diag, pos + diag);

    return false;
}

bool Thumb::staticShift(const Vector2d &by) {
    if (Base::staticShift(by)) {
        return true;
    }

    pos += by;

    return false;
}

void Thumb::renderThumb(Texture &target, const Rect<double> &at) {
    Style::Element elem = Style::EL_SLIDER_THUMB;

    if (lockedV()) {
        assert(!lockedH());
        elem = Style::EL_SLIDER_HTHUMB;
    }

    if (lockedH()) {
        assert(!lockedV());
        elem = Style::EL_SLIDER_VTHUMB;
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
            anim = getStyle().animElement(at, elem, lastState, curState, duration);
            anim.start();
        }
    }

    // We animate even the static states and transitions, because otherwise
    // there's a one-pixel difference in positions, which annoys me a lot
    if (!anim.isGoing()) {
        anim = getStyle().animElement(at, elem, lastState, curState, 0.001);
        anim.start();
    }

    assert(anim.isGoing());
    anim.render(target, at);
}
#pragma endregion Thumb


#pragma region Slider
template
class Slider<false, false>;

template
class Slider<false, true>;

template
class Slider<true, false>;
#pragma endregion Slider


}
