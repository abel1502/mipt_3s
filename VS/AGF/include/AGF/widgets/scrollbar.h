#ifndef AGF_WIDGETS_SCROLLBAR_H
#define AGF_WIDGETS_SCROLLBAR_H

#include <AGF/widget.h>
#include <AGF/widgets/slider.h>
#include <AGF/widgets/button.h>
#include <AGF/widgets/static_group.h>
#include <AGF/helpers/widget_ref.h>
#include <ACL/math/cmath.h>


namespace abel::gui::widgets {


namespace _impl {

class ScrollbarArrow : public Button {
public:
    enum Direction {
        DIR_LEFT,
        DIR_RIGHT,
        DIR_UP,
        DIR_DOWN,
    };

    inline ScrollbarArrow(Widget *parent_, const Rect<double> &region_, Direction dir_) :
        Button(parent_, region_, ""), dir{dir_} {}

    EVENT_HANDLER_OVERRIDE(Render);

protected:
    Direction dir;

};

class ScrollbarThumb : public Thumb {
public:
    using Base = Thumb;
    EVENT_HANDLER_USING(Base);


    inline ScrollbarThumb(Widget *parent_, const Rect<double> &region_, bool vertical = false) :
        Base(parent_, region_, !vertical, vertical) {}

    inline void setLength(double length) {
        if (!lockedV()) {
            region.y(region.getCenter().y() - length / 2);
            region.h(length);
        } else {
            region.x(region.getCenter().x() - length / 2);
            region.w(length);
        }
    }

protected:
    virtual void renderThumb(Texture &target, const Rect<double> &at) override;

};

template <bool Vertical>
class ScrollbarSlider : public Slider1D<Vertical> {
public:
    using Base = Slider1D<Vertical>;
    EVENT_HANDLER_USING(Base);


    inline ScrollbarSlider(Widget *parent_, const Rect<double> &region_,
                           double min = 0, double max = 1, double initial_ = 0) :
        Base(parent_, region_,
             min, max, initial_,
             createThumb(this, region_)) {}

    using Base::getValue;
    using Base::setValue;

    inline double getThumbScale() const {
        return thumbScale;
    }

    inline void setThumbScale(double scale) {
        scale = math::clamp(scale, 0.1, 0.99);

        double oldVal = getValue();

        thumbScale = scale;

        double totalLength = 0;
        if constexpr (Vertical) {
            totalLength = this->region.h();
        } else {
            totalLength = this->region.w();
        }

        dynamic_cast<ScrollbarThumb &>(this->thumb()).setLength(totalLength * thumbScale);

        setValue(oldVal);
    }

protected:
    static constexpr double DEFAULT_THUMB_SCALE = 0.2;
    double thumbScale = DEFAULT_THUMB_SCALE;


    virtual void renderBackground(Texture &target, const Rect<double> &) override {
        this->getStyle().drawElement(target, this->region, Vertical ?
                               Style::EL_SCROLLBAR_VTRACK :
                               Style::EL_SCROLLBAR_HTRACK,
                               Style::ELS_NORMAL);
    }

private:
    static ScrollbarThumb *createThumb(ScrollbarSlider *inst, const Rect<double> &region) {
        Rect<double> thumbPos = region;

        if constexpr (Vertical) {
            thumbPos.h(thumbPos.h() * DEFAULT_THUMB_SCALE);
        } else {
            thumbPos.w(thumbPos.w() * DEFAULT_THUMB_SCALE);
        }

        return new ScrollbarThumb(inst, thumbPos, Vertical);
    }

};

}  // namespace _impl


template <bool Vertical = false>
class Scrollbar : public StaticGroup<_impl::ScrollbarSlider<Vertical>,
                                     _impl::ScrollbarArrow,
                                     _impl::ScrollbarArrow> {
public:
    using Base = StaticGroup<_impl::ScrollbarSlider<Vertical>,
                             _impl::ScrollbarArrow,
                             _impl::ScrollbarArrow>;
    EVENT_HANDLER_USING(Base);


    Signal<bool (Scrollbar &scrollbar)> sigChanged{};


    Scrollbar(Widget *parent_, const Rect<double> &region_, double min = 0, double max = 1, double initial = 0) :
        Base(parent_, region_,
             nullptr,
             nullptr,
             nullptr) {

        constexpr _impl::ScrollbarArrow::Direction
            BACK_DIR  = Vertical ?
                        _impl::ScrollbarArrow::DIR_UP :
                        _impl::ScrollbarArrow::DIR_LEFT,
            FRONT_DIR = Vertical ?
                        _impl::ScrollbarArrow::DIR_DOWN :
                        _impl::ScrollbarArrow::DIR_RIGHT;

        Rect<double> arrowLRegion = this->region;
        Rect<double> arrowRRegion = this->region;
        Rect<double> sliderRegion = this->region;

        if constexpr (Vertical) {
            arrowLRegion.h(arrowLRegion.w());
            arrowRRegion.h(arrowRRegion.w());
            arrowRRegion.y(this->region.y1() - arrowRRegion.h());
            sliderRegion.y0(arrowLRegion.y1());
            sliderRegion.y1(arrowRRegion.y0());
        } else {
            arrowLRegion.w(arrowLRegion.h());
            arrowRRegion.w(arrowRRegion.h());
            arrowRRegion.x(this->region.x1() - arrowRRegion.w());
            sliderRegion.x0(arrowLRegion.x1());
            sliderRegion.x1(arrowRRegion.x0());
        }

        arrowL(new _impl::ScrollbarArrow(this, arrowLRegion, BACK_DIR ));
        arrowR(new _impl::ScrollbarArrow(this, arrowRRegion, FRONT_DIR));
        slider(new _impl::ScrollbarSlider<Vertical>(this, sliderRegion,
                                                    min, max, initial));

        arrowL().sigClick += [this]() {
            setValue(getValue() - getStep());

            return false;
        };

        arrowR().sigClick += [this]() {
            setValue(getValue() + getStep());

            return false;
        };

        slider().sigChanged += [this](Vector2d value) {
            sigChanged(*this);

            return false;
        };
    }

    inline double getValue() const {
        return slider().getValue();
    }

    inline void setValue(double value) {
        slider().setValue(value);
    }

    inline void resetValue() {
        slider().resetValue();
    }

    inline double getStep() const {
        const Rect<double> &limits = slider().getLimits();

        constexpr double STEP_FRACTION = 1. / 5;

        double limit = 0;

        if constexpr (Vertical) {
            limit = limits.h();
        } else {
            limit = limits.w();
        }

        return std::abs(limit * STEP_FRACTION);
    }

    inline double getThumbScale() const {
        return slider().getThumbScale();
    }

    inline void setThumbScale(double scale) {
        slider().setThumbScale(scale);
    }

protected:
    using typename Base::Types;

    SGRP_DECLARE_BINDING_I(slider, 0);
    SGRP_DECLARE_BINDING_I(arrowL, 1);
    SGRP_DECLARE_BINDING_I(arrowR, 2);

};


using ScrollbarH = Scrollbar<false>;
using ScrollbarV = Scrollbar<true> ;


extern template
class Scrollbar<false>;

extern template
class Scrollbar<true>;


}


#endif // AGF_WIDGETS_SCROLLBAR_H

