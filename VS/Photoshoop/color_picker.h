#pragma once

#include <ACL/general.h>
#include <AGF/widget.h>
#include <AGF/widgets/all.h>


using abel::gui::Rect;
using abel::gui::Color;
using abel::math::Vector2d;


class ColorPicker;


// Not simply _impl to be more friendly towards VS's intellisense
namespace _myimpl {

class ColorSliderThumb : public abel::gui::widgets::Thumb {
public:
    using Base = abel::gui::widgets::Thumb;
    EVENT_HANDLER_USING(Base);


    using Base::Thumb;

protected:
    virtual void renderThumb(abel::gui::Texture &target, const Rect<double> &at) override;
};

class ColorSliderSV : public abel::gui::widgets::Slider2D {
public:
    using Base = abel::gui::widgets::Slider2D;
    EVENT_HANDLER_USING(Base);


    inline ColorSliderSV(ColorPicker *parent_, const Rect<double> &region_,
                         const Rect<double> &limits_);

    inline const ColorPicker *getParent() const;
    inline       ColorPicker *getParent()      ;

protected:
    mutable double cachedTextureHue = NAN;
    mutable abel::unique_ptr<abel::gui::Texture> cachedTexture = nullptr;


    virtual void renderBackground(abel::gui::Texture &target, const Rect<double> &at) override;

};


class ColorSliderH : public abel::gui::widgets::SliderH {
public:
    using Base = abel::gui::widgets::SliderH;
    EVENT_HANDLER_USING(Base);


    inline ColorSliderH(ColorPicker *parent_, const Rect<double> &region_,
                        double min, double max);

    inline const ColorPicker *getParent() const;
    inline       ColorPicker *getParent()      ;

protected:
    mutable abel::unique_ptr<abel::gui::Texture> cachedTexture = nullptr;


    virtual void renderBackground(abel::gui::Texture &target, const Rect<double> &at) override;

};


class ColorSliderA : public abel::gui::widgets::SliderH {
public:
    using Base = abel::gui::widgets::SliderH;
    EVENT_HANDLER_USING(Base);


    inline ColorSliderA(ColorPicker *parent_, const Rect<double> &region_,
                        double min, double max);

    inline const ColorPicker *getParent() const;
    inline       ColorPicker *getParent()      ;

protected:
    mutable abel::unique_ptr<abel::gui::Texture> cachedTexture = nullptr;


    virtual void renderBackground(abel::gui::Texture &target, const Rect<double> &at) override;

};

}  // namespace _myimpl


class ColorPicker : public abel::gui::widgets::StaticGroup<
                               _myimpl::ColorSliderSV,
                               _myimpl::ColorSliderH,
                               _myimpl::ColorSliderA
                           > {
public:
    using Base = abel::gui::widgets::StaticGroup<
                     _myimpl::ColorSliderSV,
                     _myimpl::ColorSliderH,
                     _myimpl::ColorSliderA
                 >;
    EVENT_HANDLER_USING(Base);

    ColorPicker(Widget *parent_, const Rect<double> &region_);

    Color getColor() const;
    void setColor(const Color &value);

    inline double getAlpha() const;
    inline void setAlpha(double value);

protected:
    friend class _myimpl::ColorSliderSV;
    friend class _myimpl::ColorSliderH;
    friend class _myimpl::ColorSliderA;

    SGRP_DECLARE_BINDING_T(sliderSV, _myimpl::ColorSliderSV);
    SGRP_DECLARE_BINDING_T(sliderH , _myimpl::ColorSliderH );
    SGRP_DECLARE_BINDING_T(sliderA , _myimpl::ColorSliderA );

    double getColorH() const;
    double getColorS() const;
    double getColorV() const;

    void setColorH(double value);
    void setColorS(double value);
    void setColorV(double value);

};


inline double ColorPicker::getAlpha() const {
    return sliderA().getValue();
}

inline void ColorPicker::setAlpha(double value) {
    sliderA().setValue(value);
}


inline const ColorPicker *_myimpl::ColorSliderSV::getParent() const { return dynamic_cast<const ColorPicker *>(parent); }
inline       ColorPicker *_myimpl::ColorSliderSV::getParent()       { return dynamic_cast<      ColorPicker *>(parent); }
inline const ColorPicker *_myimpl::ColorSliderH ::getParent() const { return dynamic_cast<const ColorPicker *>(parent); }
inline       ColorPicker *_myimpl::ColorSliderH ::getParent()       { return dynamic_cast<      ColorPicker *>(parent); }
inline const ColorPicker *_myimpl::ColorSliderA ::getParent() const { return dynamic_cast<const ColorPicker *>(parent); }
inline       ColorPicker *_myimpl::ColorSliderA ::getParent()       { return dynamic_cast<      ColorPicker *>(parent); }

inline _myimpl::ColorSliderSV::ColorSliderSV(ColorPicker *parent_, const Rect<double> &region_,
                                             const Rect<double> &limits_) :
    Base(parent_, region_, limits_, {},
         new ColorSliderThumb(this, region_.getCenter())) {}

inline _myimpl::ColorSliderH::ColorSliderH(ColorPicker *parent_, const Rect<double> &region_,
                                           double min, double max) :
    Base(parent_, region_, min, max, 0,
         new ColorSliderThumb(this, region_.getCenter(), true)) {}

inline _myimpl::ColorSliderA::ColorSliderA(ColorPicker *parent_, const Rect<double> &region_,
                                           double min, double max) :
    Base(parent_, region_, min, max, 1,
         new ColorSliderThumb(this, region_.getCenter(), true)) {}