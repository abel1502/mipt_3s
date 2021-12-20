#include <AGF/llgui.h>
#include <ACL/unique_ptr.h>
#include <AGF/helpers/widget_ref.h>
#include "tools_widget.h"
#include "app.h"
#include <algorithm>


using namespace abel;
using namespace abel::gui;


#pragma region ColorPicker
namespace _myimpl {

void ColorSliderThumb::renderThumb(abel::gui::Texture &target, const Rect<double> &at) {
    target.setLineColor(Color::WHITE * 0.6f);
    target.setLineWidth(1.5f);
    target.drawEllipse(pos, at.getDiag() / 2, false);
}

void ColorSliderSV::renderBackground(Texture &target, const Rect<double> &at) {
    assert(parent);
    double hue = getParent()->getColorH();

    if (!cachedTexture || hue != cachedTextureHue) {
        cachedTextureHue = hue;

        unsigned w = (unsigned)at.w();
        unsigned h = (unsigned)at.h();

        w = std::min(w, 64u);
        h = std::min(h, 64u);
        assert(w && h);

        cachedTexture.emplace(w, h);

        PackedColor *tmpBuf = cachedTexture->getBuf(false, true);
        assert(tmpBuf);

        unsigned numPixels = w * h;

        unsigned x = 0;
        unsigned y = 0;
        for (unsigned i = 0; i < numPixels; ++i) {
            x++;
            if (x >= w) {
                x = 0;
                y++;
            }

            tmpBuf[y * w + x] = Window::HSV2RGB(
                abel::math::Vector3d{hue, (double)x / w, 1 - (double)y / h}
            ).pack();
        }

        cachedTexture->flushBuf();
    }

    target.embed(at, *cachedTexture);
}

void ColorSliderH::renderBackground(Texture &target, const Rect<double> &at) {
    if (!cachedTexture) {
        unsigned w = (unsigned)at.w();
        unsigned h = (unsigned)at.h();

        w = std::min(w, 64u);
        h = std::min(h, 16u);
        assert(w && h);

        cachedTexture.emplace(w, h);

        PackedColor *tmpBuf = cachedTexture->getBuf(false, true);
        assert(tmpBuf);

        unsigned numPixels = w * h;

        unsigned x = 0;
        unsigned y = 0;
        for (unsigned i = 0; i < numPixels; ++i) {
            x++;
            if (x >= w) {
                x = 0;
                y++;
            }

            tmpBuf[y * w + x] = Window::HSV2RGB(
                abel::math::Vector3d{(double)x / w, 1., 1.}
            ).pack();
        }

        cachedTexture->flushBuf();
    }

    target.embed(at, *cachedTexture);
}

void ColorSliderA::renderBackground(Texture &target, const Rect<double> &at) {
    if (!cachedTexture) {
        unsigned w = (unsigned)at.w();
        unsigned h = (unsigned)at.h();

        w = std::min(w, 64u);
        h = std::min(h, 16u);
        assert(w && h);

        cachedTexture.emplace(w, h);

        PackedColor *tmpBuf = cachedTexture->getBuf(false, true);
        assert(tmpBuf);

        unsigned numPixels = w * h;

        unsigned x = 0;
        unsigned y = 0;
        for (unsigned i = 0; i < numPixels; ++i) {
            x++;
            if (x >= w) {
                x = 0;
                y++;
            }

            tmpBuf[y * w + x] = Color(Color::WHITE * (1.f - (float)x / w)).pack();
        }

        cachedTexture->flushBuf();
    }

    target.embed(at, *cachedTexture);
}

}


ColorPicker::ColorPicker(Widget *parent_, const Rect<double> &region_, bool global) :
    Base(parent_, region_,
         nullptr,
         nullptr,
         nullptr) {

    const double height = region.h();
    const double width  = region.w();

    const double sliderHeight = widgets::Thumb::DEFAULT_SIZE * 1.25;

    REQUIRE(height >= sliderHeight * 5);

    const Rect<double> regionSV = Rect<double>::wh(0, 0, width, height - 2 * sliderHeight);
    const Rect<double> regionH  = Rect<double>::wh(0, height - 2 * sliderHeight, width, sliderHeight);
    const Rect<double> regionA  = Rect<double>::wh(0, height - sliderHeight, width, sliderHeight);

    sliderSV(new _myimpl::ColorSliderSV(nullptr, regionSV, Rect<double>::wh(0, 1, 1, -1)));
    sliderH (new _myimpl::ColorSliderH (nullptr, regionH , 0, 1));
    sliderA (new _myimpl::ColorSliderA (nullptr, regionA , 0, 1));

    if (global) {
        MyApp::getInstance().toolMgr.sigConfigChanged += [inst = WidgetRefTo(this)](ToolManager &mgr) {
            if (!inst) {
                return true;
            }

            inst->setColor(mgr.getColor());
            inst->setAlpha(mgr.getAlpha());

            return false;
        };

        sliderSV().sigChanged += [inst = WidgetRefTo(this)](Vector2d value) {
            if (!inst) {
                return true;
            }

            MyApp::getInstance().toolMgr.setColor(inst->getColor());

            return false;
        };

        sliderH().sigChanged += [inst = WidgetRefTo(this)](Vector2d value) {
            if (!inst) {
                return true;
            }

            MyApp::getInstance().toolMgr.setColor(inst->getColor());

            return false;
        };

        sliderA().sigChanged += [inst = WidgetRefTo(this)](Vector2d value) {
            if (!inst) {
                return true;
            }

            MyApp::getInstance().toolMgr.setAlpha(inst->getAlpha());

            return false;
        };
    }

    sliderSV().sigChanged += [inst = WidgetRefTo(this)](Vector2d value) {
        if (!inst) {
            return true;
        }

        inst->sigChanged(inst->getColor(), (float)inst->getAlpha());

        return false;
    };

    sliderH().sigChanged += [inst = WidgetRefTo(this)](Vector2d value) {
        if (!inst) {
            return true;
        }

        inst->sigChanged(inst->getColor(), (float)inst->getAlpha());

        return false;
    };

    sliderA().sigChanged += [inst = WidgetRefTo(this)](Vector2d value) {
        if (!inst) {
            return true;
        }

        inst->sigChanged(inst->getColor(), (float)inst->getAlpha());

        return false;
    };

    setColor(Color::BLACK);
    setAlpha(1);
}


Color ColorPicker::getColor() const {
    return Window::HSV2RGB(abel::math::Vector3d(getColorH(), getColorS(), getColorV()));
}

void ColorPicker::setColor(const Color &value) {
    abel::math::Vector3d hsl = Window::RGB2HSV(value);

    setColorH(hsl.x());
    setColorS(hsl.y());
    setColorV(hsl.z());
}

double ColorPicker::getColorH() const {
    return sliderH().getValue();
}

double ColorPicker::getColorS() const {
    return sliderSV().getValue().x();
}

double ColorPicker::getColorV() const {
    return sliderSV().getValue().y();
}

void ColorPicker::setColorH(double value) {
    sliderH().setValue(value);
}

void ColorPicker::setColorS(double value) {
    sliderSV().setValue({value, sliderSV().getValue().y()});
}

void ColorPicker::setColorV(double value) {
    sliderSV().setValue({sliderSV().getValue().x(), value});
}
#pragma endregion ColorPicker


#pragma region SizePicker
SizePicker::SizePicker(Widget *parent_, const Rect<double> &region_) :
    Base(parent_, region_, MIN_SIZE, MAX_SIZE, DEFAULT_SIZE) {}
#pragma endregion SizePicker


#pragma region ToolButton
_myimpl::ToolButton::ToolButton(Widget *parent_, const Rect<double> &region_,
                                ToolManager::tool_handle_t handle_, const char *name_) :
    Base(parent_, region_, nullptr),
    handle{handle_}, name{name_} {

    button(new widgets::Button(this, region.padded(2), name.data()));

    changeActiveState(MyApp::getInstance().toolMgr.getActiveToolHandle() == handle);

    MyApp::getInstance().toolMgr.sigConfigChanged += [inst = WidgetRefTo(this)](ToolManager &mgr) {
        if (!inst) {
            return true;
        }

        inst->changeActiveState(mgr.getActiveToolHandle() == inst->handle);

        return false;
    };

    button().sigClick += [inst = WidgetRefTo(this)]() {
        if (!inst) {
            return true;
        }

        MyApp::getInstance().toolMgr.selectTool(inst->handle);

        return false;
    };
}


void _myimpl::ToolButton::changeActiveState(bool active) {
    if (active == isNameActive) {
        return;
    }

    isNameActive = active;

    button().getLabel().setText(isNameActive ?
                                nameActive.data() :
                                name.data());
}
#pragma endregion ToolButton


#pragma region EffectButton
_myimpl::EffectButton::EffectButton(Widget *parent_, const Rect<double> &region_,
                                    ToolManager::effect_handle_t handle_, const char *name_) :
    Base(parent_, region_, nullptr, nullptr),
    handle{handle_}, name{name_} {

    const Rect<double> childrenRect = region.padded(2);
    Rect<double>   buttonRect = childrenRect;
    Rect<double> settingsRect = childrenRect;

    buttonRect.w(childrenRect.w() * 0.7);

    // Setting it this wat ensures that the right corner stays in place
    settingsRect.x0(settingsRect.x0() + childrenRect.w() * 0.75);

    button(new widgets::Button(this, buttonRect, name));
    settingsButton(new widgets::Button(this, settingsRect, "#"));

    button().sigClick += [inst = WidgetRefTo(this)]() {
        if (!inst) {
            return true;
        }

        Effect &effect = MyApp::getInstance().toolMgr.getEffect(inst->handle);

        Canvas *canvas = MyApp::getInstance().toolMgr.getActiveCanvas();

        if (!canvas) {
            DBG("No active canvas, skipping");
            return false;
        }

        canvas->applyEffect(effect);

        return false;
    };

    settingsButton().sigClick += [inst = WidgetRefTo(this)]() {
        if (!inst) {
            return true;
        }

        Effect &effect = MyApp::getInstance().toolMgr.getEffect(inst->handle);

        if (!effect.supportsSettings()) {
            DBG("Effect doesn't support settings");

            return false;
        }

        effect.showSettings();

        return false;
    };
}
#pragma endregion EffectButton


#pragma region ToolsWidget
ToolsWidget::ToolsWidget(Widget *parent_, const Rect<double> &region_) :
    Base(parent_, region_, nullptr, nullptr, nullptr, nullptr) {

    const double height = region.h();
    const double width  = region.w();

    const double _col1W = std::min(width * 0.4, 120.);
    const double colW[2] = {_col1W, std::min(width - _col1W, 180.)};
    const double colX[2] = {0, colW[0]};

    const double sliderHeight = widgets::Thumb::DEFAULT_SIZE * 1.15;
    const double paletteHeight = colW[0] + 2 * sliderHeight;

    const Rect<double> regionColorPicker = Rect<double>::wh(colX[0], 0,             colW[0], paletteHeight);
    const Rect<double> regionSizePicker  = Rect<double>::wh(colX[0], paletteHeight, colW[0], sliderHeight);
    const Rect<double> regionTools       = Rect<double>::wh(colX[1], 0,             colW[1], height / 2);
    const Rect<double> regionEffects     = Rect<double>::wh(colX[1], height / 2,    colW[1], height / 2);

    colorPicker(new ColorPicker(nullptr, regionColorPicker));
    sizePicker(new SizePicker(nullptr, regionSizePicker));
    toolButtons(new _tool_buttons_type(nullptr, regionTools, 0));
    effectButtons(new _effect_buttons_type(nullptr, regionEffects, 0));

    buttonSize = Rect<double>::wh(0, 0, colW[1] * 0.85, 26);

    addToolButton(MyApp::getInstance().toolMgr
                      .getBasicToolHandle(ToolManager::BTT_BRUSH),
                  "Brush");

    addToolButton(MyApp::getInstance().toolMgr
                      .getBasicToolHandle(ToolManager::BTT_ERASER),
                  "Eraser");

    addToolButton(MyApp::getInstance().toolMgr
                      .getBasicToolHandle(ToolManager::BTT_COLOR_PICKER),
                  "Picker");

    MyApp::getInstance().toolMgr.sigConfigChanged += [this](ToolManager &mgr) {
        sizePicker().setSize(mgr.getRadius() * 2);

        return false;
    };

    sizePicker().sigChanged += [](Vector2d value) {
        MyApp::getInstance().toolMgr.setRadius(value.x() / 2);

        return false;
    };
}

void ToolsWidget::addToolButton(ToolManager::tool_handle_t toolIdx, const char *name) {
    toolButtons().createChild(buttonSize, toolIdx, name);
}

void ToolsWidget::addEffectButton(ToolManager::effect_handle_t effectIdx, const char *name) {
    effectButtons().createChild(buttonSize, effectIdx, name);
}
#pragma endregion ToolsWidget
