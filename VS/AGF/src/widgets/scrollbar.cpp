#include <AGF/llgui.h>
#include <AGF/widgets/scrollbar.h>


namespace abel::gui::widgets {


namespace _impl {

EVENT_HANDLER_IMPL(ScrollbarArrow, Render) {
    Style::Element elem{};

    switch (dir) {
    case DIR_LEFT:
        elem = Style::EL_SCROLLBAR_ARR_LEFT;
        break;

    case DIR_RIGHT:
        elem = Style::EL_SCROLLBAR_ARR_RIGHT;
        break;

    case DIR_UP:
        elem = Style::EL_SCROLLBAR_ARR_UP;
        break;

    case DIR_DOWN:
        elem = Style::EL_SCROLLBAR_ARR_DOWN;
        break;

    NODEFAULT
    }

    return renderCustomized(event, elem, true);
}

void ScrollbarThumb::renderThumb(Texture &target, const Rect<double> &at) {
    Style::Element elem{};

    if (lockedV()) {
        assert(!lockedH());
        elem = Style::EL_SCROLLBAR_HTHUMB;
    } else {
        assert(lockedH());
        elem = Style::EL_SCROLLBAR_VTHUMB;
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

}  // namespace _impl

template
class Scrollbar<false>;

template
class Scrollbar<true>;


}
