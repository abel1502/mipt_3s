#include <AGF/llgui.h>
#include "canvas.h"
#include "app.h"


using namespace abel;
using namespace abel::gui;


Canvas::Canvas(Widget *parent_, const Rect<double> &region_) :
    Base(parent_, region_),
    layers{} {

    layers.appendEmplace(region.getDiag());
    abel::gui::Texture &target = activeLayer().getTexture();
    target.setFillColor(Color::GREEN);
    target.clear();

    using namespace std::placeholders;

    mt.sigDrag += std::bind(&Canvas::onDrag,  this, _1, _2);
    mt.sigDown += std::bind(&Canvas::onClick, this, _1);
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
        event.target.embed(region, layer.getTexture());
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

    MyApp::getInstance().toolMgr.getActiveTool()
        .applyLine(activeLayer(), event.pos0, event.pos1);

    return false;
}

bool Canvas::onClick(const abel::gui::MouseClickEvent &event) {
    if (event.button != MouseBtn::Left) {
        return false;
    }

    MyApp::getInstance().toolMgr.getActiveTool()
        .applyPoint(activeLayer(), event.pos);

    return false;
}
