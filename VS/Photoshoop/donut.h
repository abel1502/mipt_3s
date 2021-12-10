#pragma once

#include <AGF/widget.h>
#include <AGF/widgets/all.h>


using abel::gui::Rect;
using abel::gui::Color;
using abel::math::Vector2d;
namespace widgets = abel::gui::widgets;


class Donut : public widgets::Button {
public:
    using Base = widgets::Button;
    EVENT_HANDLER_USING(Base);

    Donut(Widget *parent_, const Rect<double> &region_);

    EVENT_HANDLER_OVERRIDE(abel::gui::Render);

    virtual bool hitTest(const Vector2d &pos) const override;

protected:
    Rect<double> getHole() const;

};