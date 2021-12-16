#include <AGF/llgui.h>
#include "canvas.h"
#include "app.h"
#include "tool.h"


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
    mt.sigDown += std::bind(&Canvas::onDown, this, _1);
    mt.sigDragStateChange += std::bind(&Canvas::onDragStateChange, this, _1, _2, _3);

    mt.sigDown += [this](const abel::gui::MouseClickEvent &event) {
        MyApp::getInstance().enqueueAction([this, event, &myApp = MyApp::getInstance()](Application &) {
            myApp.toolMgr.getActiveTool().onBegin(event.pos - region.getPos());

            myApp.demandRedraw();
        });

        return false;
    };

    mt.sigUp += [this](const abel::gui::MouseClickEvent &event) {
        MyApp::getInstance().enqueueAction([this, event, &myApp = MyApp::getInstance()](Application &) {
            myApp.toolMgr.getActiveTool().onEnd(event.pos - region.getPos());

            myApp.demandRedraw();
        });

        return false;
    };

    MyApp::getInstance().toolMgr.sigConfigChanged += [inst = WidgetRefTo{this}](ToolManager &toolMgr) {
        if (!inst) {
            return true;
        }

        if (toolMgr.getActiveCanvas() != inst.get()) {
            inst->activeLayer().endPreview(false);
        } else {
            inst->activeLayer().beginPreview();
        }

        return false;
    };

    becomeActive();
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
        if (!layer.willOverwrite()) {
            event.target.embed(region, layer.getTexture());
        }
        event.target.embed(region, layer.getPreview());
    }

    return EventStatus::done();
}

EVENT_HANDLER_IMPL(Canvas, MouseClick) {
    return mt.processEvent(event, Base::dispatchEvent(event));
}

EVENT_HANDLER_IMPL(Canvas, MouseMove) {
    return mt.processEvent(event, Base::dispatchEvent(event));
}

EVENT_HANDLER_IMPL(Canvas, FocusUpdate) {
    EventStatus status = Base::dispatchEvent(event);

    if (status.shouldHandle(status.NODE) && event.focus) {
        becomeActive();
    }

    return status;
}

bool Canvas::onDrag(MouseBtn btn, const MouseMoveEvent &event) {
    if (btn != MouseBtn::Left) {
        return false;
    }

    MyApp::getInstance().enqueueAction([this, event, &myApp = MyApp::getInstance()](Application &) {
        myApp.toolMgr.getActiveTool()
            .applyLine(activeLayer(), event.pos0 - region.getPos(), event.pos1 - region.getPos());

        myApp.demandRedraw();
    });

    return false;
}

bool Canvas::onDown(const abel::gui::MouseClickEvent &event) {
    if (event.button != MouseBtn::Left) {
        return false;
    }

    MyApp::getInstance().enqueueAction([this, event, &myApp = MyApp::getInstance()](Application &) {
        myApp.toolMgr.getActiveTool()
            .applyPoint(activeLayer(), event.pos - region.getPos());

        myApp.demandRedraw();
    });

    return false;
}

bool Canvas::onDragStateChange(abel::gui::MouseBtn btn,
                               abel::gui::MouseAttrs attrs, bool state) {
    if (btn != MouseBtn::Left) {
        return false;
    }

    becomeActive();

    if (state) {
        activeLayer().beginPreview();
    } else {
        activeLayer().endPreview(true);
    }


    return false;
}

void Canvas::addLayer(bool switchTo) {
    layers.appendEmplace(activeLayer().getSize());

    if (switchTo) {
        selectLayer(getLayersCnt() - 1);
    }
}

void Canvas::selectLayer(unsigned idx) {
    assert(idx < getLayersCnt());

    activeLayerIdx = idx;

    if (!sigLayerChanged.isBeingInvoked()) {
        sigLayerChanged(*this);
    }
}

void Canvas::applyEffect(Effect &effect) {
    effect.apply(activeLayer());
}

void Canvas::loadImage(const std::filesystem::path &path) {
    abel::gui::Texture image{path.c_str()};

    activeLayer().getTexture().embed(activeLayer().getTexture().getRect(), image);
}

void Canvas::becomeActive() {
    MyApp::getInstance().toolMgr.setActiveCanvas(this);
}

