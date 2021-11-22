#include <AGF/llgui.h>
#include "canvas.h"


using namespace abel;
using namespace abel::gui;


Canvas::Canvas(Widget *parent_, const Rect<double> &region_) :
    Base(parent_, region_),
    layers(1) {

    layers[0].emplace(region.getDiag());
    activeLayer().setFillColor(Color::GREEN);
    activeLayer().clear();

    using namespace std::placeholders;

    mt.sigDrag += std::bind(&Canvas::onDrag, this, _1, _2);
    mt.sigDown += [this](const MouseClickEvent &event) {
        // We translate single presses into trivial movements, just in case
        onDrag(event.button, MouseMoveEvent(event.pos, event.pos, event.attrs));
        return false;
    };
}


EVENT_HANDLER_IMPL(Canvas, Render) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    // TODO: Fill with checkers instead
    event.target.setFillColor(Color::WHITE);
    event.target.drawRect(region);

    // TODO: Blend in a separate texture instead
    for (const auto &layer : layers) {
        event.target.embed(region, *layer);
    }

    return EventStatus::done();
}

EVENT_HANDLER_IMPL(Canvas, MouseClick) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    if (!mt.processEvent(event)) {
        return status;
    }

    return EventStatus::stop(EventStatus::TREE);
}

EVENT_HANDLER_IMPL(Canvas, MouseMove) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    if (!mt.processEvent(event)) {
        return status;
    }

    return EventStatus::stop(EventStatus::TREE);
}

bool Canvas::onDrag(MouseBtn btn, const MouseMoveEvent &event) {
    if (btn != MouseBtn::Left) {
        return false;
    }

    Texture &target = activeLayer();
    // TODO: Only modify through the palette and in ctor
    float width = 5.f;
    target.setLineColor(Color::BLACK);
    target.setFillColor(Color::BLACK);
    target.setLineWidth(width);

    Vector2d pos0 = event.pos0 - region.getPos();
    Vector2d pos1 = event.pos1 - region.getPos();

    target.drawLine(pos0, pos1);
    target.drawCircle(pos0, width / 2);
    target.drawCircle(pos1, width / 2);

    return false;
}