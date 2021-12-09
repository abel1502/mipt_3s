#pragma once

#include <ACL/general.h>
#include <AGF/widget.h>
#include <AGF/widgets/all.h>
#include "tool.h"
#include <string>


using abel::gui::Rect;
using abel::gui::Color;
using abel::math::Vector2d;
namespace widgets = abel::gui::widgets;


class ColorPicker;


// Not simply _impl to be more friendly towards VS's intellisense
namespace _myimpl {

class ColorSliderThumb : public widgets::Thumb {
public:
    using Base = widgets::Thumb;
    EVENT_HANDLER_USING(Base);


    using Base::Thumb;

protected:
    virtual void renderThumb(abel::gui::Texture &target, const Rect<double> &at) override;
};

class ColorSliderSV : public widgets::Slider2D {
public:
    using Base = widgets::Slider2D;
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


class ColorSliderH : public widgets::SliderH {
public:
    using Base = widgets::SliderH;
    EVENT_HANDLER_USING(Base);


    inline ColorSliderH(ColorPicker *parent_, const Rect<double> &region_,
                        double min, double max);

    inline const ColorPicker *getParent() const;
    inline       ColorPicker *getParent()      ;

protected:
    mutable abel::unique_ptr<abel::gui::Texture> cachedTexture = nullptr;


    virtual void renderBackground(abel::gui::Texture &target, const Rect<double> &at) override;

};


class ColorSliderA : public widgets::SliderH {
public:
    using Base = widgets::SliderH;
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


class ColorPicker : public widgets::StaticGroup<
                               _myimpl::ColorSliderSV,
                               _myimpl::ColorSliderH,
                               _myimpl::ColorSliderA
                           > {
public:
    using Base = widgets::StaticGroup<
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


class SizePicker : public widgets::SliderH {
public:
    using Base = widgets::SliderH;
    EVENT_HANDLER_USING(Base);

    static constexpr double
        MIN_SIZE = 1,
        MAX_SIZE = 50,
        DEFAULT_SIZE = 5;


    SizePicker(Widget *parent_, const Rect<double> &region_);

    inline double getSize() const;
    inline void setSize(double value);

protected:
    //

};


namespace _myimpl {

#pragma region ToolButton
using _ToolButtonBase =
    widgets::StaticGroup<
        widgets::Button
    >;

class ToolButton : public _ToolButtonBase {
public:
    using Base = _ToolButtonBase;
    EVENT_HANDLER_USING(Base);

    ToolButton(Widget *parent_, const Rect<double> &region_,
               ToolManager::tool_handle_t handle_, const char *name_);

protected:
    ToolManager::tool_handle_t handle;
    const std::string_view name;
    const std::string nameActive = std::string("* ") + name.data();
    bool isNameActive = false;


    SGRP_DECLARE_BINDING_I(button, 0);


    void changeActiveState(bool active);

};
#pragma endregion ToolButton

#pragma region EffectButton
using _EffectButtonBase =
    widgets::StaticGroup<
        widgets::Button,
        widgets::Button
    >;

class EffectButton : public _EffectButtonBase {
public:
    using Base = _EffectButtonBase;
    EVENT_HANDLER_USING(Base);

    EffectButton(Widget *parent_, const Rect<double> &region_,
                 ToolManager::effect_handle_t handle_, const char *name_);

protected:
    ToolManager::effect_handle_t handle;
    const char *name;


    SGRP_DECLARE_BINDING_I(button, 0);
    SGRP_DECLARE_BINDING_I(settingsButton, 1);

};
#pragma endregion EffectButton


using _ToolsWidgetBase =
    widgets::StaticGroup<
        ColorPicker,
        SizePicker,
        widgets::LayoutOf<
            ToolButton
        >,
        widgets::LayoutOf<
            EffectButton
        >
    >;

}


class ToolsWidget : public _myimpl::_ToolsWidgetBase {
public:
    using Base = _myimpl::_ToolsWidgetBase;
    EVENT_HANDLER_USING(Base);


    ToolsWidget(Widget *parent_, const Rect<double> &region_);

    void   addToolButton(ToolManager::  tool_handle_t   toolIdx, const char *name);
    void addEffectButton(ToolManager::effect_handle_t effectIdx, const char *name);

protected:
    using   _tool_buttons_type = Base::Types::item<2>; // The type name is too long
    using _effect_buttons_type = Base::Types::item<3>; // Same

    SGRP_DECLARE_BINDING_T(colorPicker, ColorPicker);
    SGRP_DECLARE_BINDING_T( sizePicker,  SizePicker);
    SGRP_DECLARE_BINDING_T(  toolButtons,   _tool_buttons_type);
    SGRP_DECLARE_BINDING_T(effectButtons, _effect_buttons_type);


    Rect<double> buttonSize{};

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


inline double SizePicker::getSize() const {
    return getValue();
}

inline void SizePicker::setSize(double value) {
    return setValue(value);
}
