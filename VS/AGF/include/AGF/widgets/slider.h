#ifndef AGF_WIDGETS_SLIDER_H
#define AGF_WIDGETS_SLIDER_H

#include <ACL/general.h>
#include <AGF/widget.h>
#include <AGF/widgets/static_group.h>
#include <AGF/helpers/mouse_tracker.h>
#include <ACL/type_traits.h>


namespace abel::gui::widgets {


class Thumb : public Widget {
public:
    using Base = Widget;
    EVENT_HANDLER_USING(Base);


    Thumb(Widget *parent_, const Rect<double> &region_);

    EVENT_HANDLER_OVERRIDE(Render);

    EVENT_HANDLER_OVERRIDE(MouseClick);

    EVENT_HANDLER_OVERRIDE(MouseMove);

    virtual bool staticShift(const Vector2d &by) override;

    const Vector2d &getPos() const {
        return pos;
    }

protected:
    MouseTracker mt{this};
    Vector2d pos;


    inline Rect<double> getBounds() const {
        assert(parent);

        return parent->getRegion().padded(region.w() / 2, region.h() / 2);
    }

};


template <bool LockV, bool LockH>
class Slider : public StaticGroup<Thumb> {
public:
    using Base = StaticGroup<Thumb>;
    EVENT_HANDLER_USING(Base);


    Slider(Widget *parent_, const Rect<double> &region_,
           const Rect<double> &limits_, const Vector2d &initial_ = {}) :
        Base(parent_, region_),
        limits{limits_},
        thumb{this, region_.getStart()} {}

    EVENT_HANDLER_OVERRIDE(Render) {
        return EventStatus::skip();
    }

    EVENT_HANDLER_OVERRIDE(MouseClick) {
        return EventStatus::skip();
    }

    inline Vector2d getValue() const {
        return posToValue(getThumbPos());
    }

    inline void setValue(const Vector2d &value) {
        setThumbPos(valueToPos(value));
    }

protected:
    SGRP_DECLARE_BINDING_T(thumb, Thumb);


    Rect<double> limits;
    Vector2d initial{};

    inline const Vector2d &getThumbPos() const {
        return thumb().getPos();
    }

    void setThumbPos(const Vector2d &pos);

    Vector2d valueToPos(const Vector2d &value) const;
    Vector2d posToValue(const Vector2d &pos  ) const;

};


class SliderV : public Slider<false, true> {
public:
    SliderV(Widget *parent_, const Rect<double> &region_,
            double min, double max, double initial_ = 0);

};


class SliderH : public Slider<true, false> {
public:
    SliderH(Widget *parent_, const Rect<double> &region_,
            double min, double max, double initial_ = 0);

};


}


#endif // AGF_WIDGETS_SLIDER_H
