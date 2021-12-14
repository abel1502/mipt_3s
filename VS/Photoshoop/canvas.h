#pragma once

#include <AGF/widgets/all.h>
#include <AGF/llgui_pre.h>
#include <ACL/unique_ptr.h>
#include <ACL/vector.h>
#include <AGF/helpers/mouse_tracker.h>
#include "layer.h"
#include <filesystem>


using abel::gui::Rect;
using abel::gui::Color;
using abel::math::Vector2d;
namespace widgets = abel::gui::widgets;


class Tool;
class Effect;

class Canvas : public abel::gui::Widget {
public:
    using Base = abel::gui::Widget;
    EVENT_HANDLER_USING(Base);


    Canvas(Widget *parent_, const Rect<double> &region_);

    EVENT_HANDLER_OVERRIDE(abel::gui::Render);

    EVENT_HANDLER_OVERRIDE(abel::gui::MouseClick);

    EVENT_HANDLER_OVERRIDE(abel::gui::MouseMove);

    EVENT_HANDLER_OVERRIDE(abel::gui::FocusUpdate);

    inline const Layer &activeLayer() const { return layers[activeLayerIdx]; }
    inline       Layer &activeLayer()       { return layers[activeLayerIdx]; }

    void addLayer(bool switchTo = true);

    void selectLayer(unsigned idx);
    inline unsigned getLayersCnt() { return layers.getSize(); }

    inline void nextLayer() { selectLayer((activeLayerIdx + 1                 ) % getLayersCnt()); }
    inline void prevLayer() { selectLayer((activeLayerIdx - 1 + getLayersCnt()) % getLayersCnt()); }

    inline unsigned getActiveLayerIdx() const { return activeLayerIdx; }

    void applyEffect(Effect &effect);

    void loadImage(const std::filesystem::path &path);

protected:
    abel::gui::MouseTracker mt{this};

    abel::vector<Layer> layers;
    unsigned activeLayerIdx = 0;

    Color penColor = Color::BLACK;


    bool onDrag(abel::gui::MouseBtn btn, const abel::gui::MouseMoveEvent &event);
    bool onDown(const abel::gui::MouseClickEvent &event);
    bool onDragStateChange(abel::gui::MouseBtn btn, abel::gui::MouseAttrs attrs, bool state);


    void becomeActive();

    void setupPreview(bool isEntry);

};
