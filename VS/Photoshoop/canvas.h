#pragma once

#include <AGF/widgets/all.h>
#include <AGF/llgui_pre.h>
#include <ACL/unique_ptr.h>
#include <ACL/vector.h>
#include <AGF/helpers/mouse_tracker.h>


using abel::gui::Rect;
using abel::gui::Color;
using abel::math::Vector2d;


class Canvas : public abel::gui::Widget {
public:
    using Base = abel::gui::Widget;
    EVENT_HANDLER_USING(Base);


    Canvas(Widget *parent_, const Rect<double> &region_);

    EVENT_HANDLER_OVERRIDE(abel::gui::Render);

    EVENT_HANDLER_OVERRIDE(abel::gui::MouseClick);

    EVENT_HANDLER_OVERRIDE(abel::gui::MouseMove);

    inline const abel::gui::Texture &activeLayer() const {
        return *layers[activeLayerIdx];
    }

protected:
    abel::gui::MouseTracker mt{this};

    abel::vector<abel::unique_ptr<abel::gui::Texture>> layers;
    unsigned activeLayerIdx = 0;

    Color penColor = Color::BLACK;


    inline abel::gui::Texture &activeLayer() {
        return *layers[activeLayerIdx];
    }

    bool onDrag(abel::gui::MouseBtn btn, const abel::gui::MouseMoveEvent &event);

};