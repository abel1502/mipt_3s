#ifndef AGF_WIDGETS_SLIDER_H
#define AGF_WIDGETS_SLIDER_H

#include <ACL/general.h>
#include <AGF/widget.h>
#include <AGF/widgets/static_group.h>
#include <AGF/helpers/mouse_tracker.h>
#include <AGF/helpers/animation.h>
#include <ACL/type_traits.h>
#include <ACL/gui/coords.h>


namespace abel::gui::widgets {


class Thumb : public Widget {
public:
    using Base = Widget;
    EVENT_HANDLER_USING(Base);

    static constexpr double DEFAULT_SIZE = 15;


    Signal<bool (const Thumb &thumb)> sigMove{};


    Thumb(Widget *parent_, const Rect<double> &region_, bool lockV = false, bool lockH = false);

    inline Thumb(Widget *parent_, const Vector2d &pos_, bool lockV = false, bool lockH = false) :
        Thumb(parent_,
              Rect<double>::wh(pos - Vector2d{DEFAULT_SIZE / 2},
                               Vector2d{DEFAULT_SIZE}),
              lockV, lockH) {}

    EVENT_HANDLER_OVERRIDE(Render);

    EVENT_HANDLER_OVERRIDE(MouseClick);

    EVENT_HANDLER_OVERRIDE(MouseMove);

    EVENT_HANDLER_OVERRIDE(MouseScroll);

    EVENT_HANDLER_OVERRIDE(Move);

    // Warning: cannot and doesn't check for bounds
    virtual bool staticShift(const Vector2d &by) override;

    virtual void renderThumb(Texture &target, const Rect<double> &at);

    const Vector2d &getPos() const {
        return pos;
    }

    constexpr bool lockedV() const { return locked[0]; };
    constexpr bool lockedH() const { return locked[1]; };

    constexpr void lockV(bool value) { locked[0] = value; };
    constexpr void lockH(bool value) { locked[1] = value; };

    template <bool Visual = false>
    inline Rect<double> getBounds() const {
        assert(parent);

        Rect<double> bounds = parent->getRegion().padded(region.w() / 2, region.h() / 2);

        constexpr double EXTRA_LOCKED_PAD = Visual ? 3 : 0;

        if (lockedV()) {
            double val = bounds.getCenter().y();
            bounds.y0(val - EXTRA_LOCKED_PAD);
            bounds.y1(val + EXTRA_LOCKED_PAD);
        }

        if (lockedH()) {
            double val = bounds.getCenter().x();
            bounds.x0(val - EXTRA_LOCKED_PAD);
            bounds.x1(val + EXTRA_LOCKED_PAD);
        }

        return bounds;
    }

    inline Vector2d clampPos(const Vector2d &physPos) const noexcept {
        const Rect<double> &bounds = getBounds();

        return physPos.clamped(bounds.getStart(), bounds.getEnd());
    }

protected:
    MouseTracker mt{this};
    Animation anim{};
    Vector2d pos;
    bool locked[2] = {};

};


template <bool LockV, bool LockH>
class Slider : public StaticGroup<Thumb> {
public:
    static_assert(!LockV || !LockH, "A slider must be able to move in at least one direction");

    using Base = StaticGroup<Thumb>;
    EVENT_HANDLER_USING(Base);


    Signal<bool (Vector2d value)> sigChanged{};


    inline Slider(Widget *parent_, const Rect<double> &region_,
           const Rect<double> &limits_, const Vector2d &initial_ = {}) :
        Slider(parent_, region_,
               limits_, initial_,
               new Thumb(this, region_.getCenter(), LockV, LockH)) {}

    EVENT_HANDLER_OVERRIDE(Render) {
        // Skipping Base for now to fiddle with the order
        EventStatus status = Widget::dispatchEvent(event);

        if (!status.shouldHandle(status.NODE)) {
            return status;
        }

        /*region.padded(getStyle().sliderThumbSize / 2 - 2)*/
        renderBackground(event.target, thumb().getBounds<true>()/*.padded(-2)*/);

        return _dispatchToChildren(event);
    }

    EVENT_HANDLER_OVERRIDE(MouseClick) {
        // Skipping Base for now to fiddle with the order
        EventStatus status = Widget::dispatchEvent(event);

        if (!status.shouldHandle(status.NODE)) {
            return status;
        }

        if (event.button == MouseBtn::Left) {
            setThumbPos(event.pos);
        }

        return _dispatchToChildren(event);
    }

    EVENT_HANDLER_OVERRIDE(MouseMove) {
        return handleMouseOpaque(event, Base::dispatchEvent(event));
    }

    inline Vector2d getValue() const {
        return posToValue(getThumbPos());
    }

    inline void setValue(const Vector2d &value) {
        setThumbPos(valueToPos(value));
    }

    inline void resetValue() {
        setValue(initial);
    }

    inline const Rect<double> &getLimits() const {
        return limits;
    }

protected:
    SGRP_DECLARE_BINDING_T(thumb, Thumb);


    Rect<double> limits;  // TODO: Store coords here instead
    Vector2d initial;
    Vector2d lastValue{NAN};  // To make sure it doesn't collide with any valid value


    Slider(Widget *parent_, const Rect<double> &region_,
           const Rect<double> &limits_, const Vector2d &initial_,
           Thumb *thumb_) :
        Base(parent_, region_, thumb_),
        limits{limits_},
        initial{initial_} {

        setValue(initial);

        thumb().sigMove += [this](const Thumb &thumb) {
            Vector2d newValue = getValue();

            if (newValue != lastValue) {
                sigChanged(newValue);
            }

            lastValue = newValue;

            return false;
        };
    }

    inline const Vector2d &getThumbPos() const {
        return thumb().getPos();
    }

    void setThumbPos(const Vector2d &pos) {
        thumb().dispatchEvent(MoveEvent{pos - getThumbPos()});
    }

    constexpr Vector2d valueToPos(const Vector2d &value) const {
        Coords coords{thumb().getBounds(), limits};

        return coords.virt2screen(value);
    }

    Vector2d posToValue(const Vector2d &pos) const {
        Coords coords{thumb().getBounds(), limits};

        return coords.screen2virt(pos);
    }

    virtual void renderBackground(Texture &target, const Rect<double> &at) {
        Style::Element elem = Style::EL_SLIDER_BODY_2D;

        if constexpr (LockV) {
            static_assert(!LockH);
            elem = Style::EL_SLIDER_HBODY;
        }

        if constexpr (LockH) {
            static_assert(!LockV);
            elem = Style::EL_SLIDER_VBODY;
        }

        getStyle().drawElement(target, at, elem, Style::ELS_NORMAL);
    }

};

extern template
class Slider<false, false>;

extern template
class Slider<false, true>;

extern template
class Slider<true, false>;


class SliderV : public Slider<false, true> {
public:
    using Base = Slider<false, true>;


    inline SliderV(Widget *parent_, const Rect<double> &region_,
                   double min, double max, double initial_ = 0) :
        Base(parent_, region_,
             Rect<double>::se<false>(-1, min, 1, max),
             Vector2d{0, initial_}) {}

    inline double getValue() const {
        return Base::getValue().y();
    }

    inline void setValue(double value) {
        Base::setValue({0, value});
    }

protected:
    inline SliderV(Widget *parent_, const Rect<double> &region_,
                   double min, double max, double initial_, Thumb *thumb_) :
        Base(parent_, region_,
             Rect<double>::se<false>(-1, min, 1, max),
             Vector2d{initial_, 0},
             thumb_) {}

};


class SliderH : public Slider<true, false> {
public:
    using Base = Slider<true, false>;


    inline SliderH(Widget *parent_, const Rect<double> &region_,
                   double min, double max, double initial_ = 0) :
        Base(parent_, region_,
             Rect<double>::se<false>(-1, min, 1, max),
             Vector2d{initial_, 0}) {}

    inline double getValue() const {
        return Base::getValue().x();
    }

    inline void setValue(double value) {
        Base::setValue({value, 0});
    }

protected:
    inline SliderH(Widget *parent_, const Rect<double> &region_,
                   double min, double max, double initial_, Thumb *thumb_) :
        Base(parent_, region_,
             Rect<double>::se<false>(min, -1, max, 1),
             Vector2d{initial_, 0},
             thumb_) {}

};


using Slider2D = Slider<false, false>;


template <bool Vertical>
using Slider1D = std::conditional_t<Vertical, SliderV, SliderH>;


}


#endif // AGF_WIDGETS_SLIDER_H
