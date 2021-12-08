#pragma once

#include <AGF/widget.h>
#include <AGF/widgets/all.h>
#include <ACL/general.h>
#include <ACL/vector.h>
#include <ACL/gui/coords.h>


using abel::gui::Rect;
using abel::gui::Color;
using abel::math::Vector2d;


class Spline : public abel::gui::widgets::GroupOf<abel::gui::widgets::Thumb> {
public:
    using Base = abel::gui::widgets::GroupOf<abel::gui::widgets::Thumb>;
    EVENT_HANDLER_USING(Base);


    Spline(Widget *parent_, const Rect<double> &region_, const Rect<double> &limits);

    EVENT_HANDLER_OVERRIDE(abel::gui::Render);

    EVENT_HANDLER_OVERRIDE(abel::gui::MouseClick);

protected:
    using Base::addChild;
    using Base::createChild;


    abel::gui::Coords coords;


    void onThumbMove(const abel::gui::widgets::Thumb &thumb);

    abel::vector<Vector2d> getSortedPoints();

    abel::vector<Vector2d> getInterpolatedPoints();



};

