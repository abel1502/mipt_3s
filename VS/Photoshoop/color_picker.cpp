#include <AGF/llgui.h>
#include <ACL/unique_ptr.h>
#include "color_picker.h"
#include "app.h"


using namespace abel;
using namespace abel::gui;


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


ColorPicker::ColorPicker(Widget *parent_, const Rect<double> &region_) :
    Base(parent_, region_,
         nullptr,
         nullptr,
         nullptr) {

    const double height = region.h();
    const double width  = region.w();

    const double sliderHeight = abel::gui::widgets::Thumb::DEFAULT_SIZE * 1.25;

    REQUIRE(height >= sliderHeight * 5);

    const Rect<double> regionSV = Rect<double>::wh(0, 0, width, height - 2 * sliderHeight);
    const Rect<double> regionH  = Rect<double>::wh(0, height - 2 * sliderHeight, width, sliderHeight);
    const Rect<double> regionA  = Rect<double>::wh(0, height - sliderHeight, width, sliderHeight);

    sliderSV(new _myimpl::ColorSliderSV(nullptr, regionSV, Rect<double>::wh(0, 1, 1, -1)));
    sliderH (new _myimpl::ColorSliderH (nullptr, regionH , 0, 1));
    sliderA (new _myimpl::ColorSliderA (nullptr, regionA , 0, 1));

    MyApp::getInstance().toolMgr.sigConfigChanged += [this](ToolManager &mgr) {
        setColor(mgr.getColor());
        setAlpha(mgr.getAlpha());

        return false;
    };

    sliderSV().sigChanged += [this](Vector2d value) {
        MyApp::getInstance().toolMgr.setColor<true>(getColor());

        return false;
    };

    sliderH().sigChanged += [this](Vector2d value) {
        MyApp::getInstance().toolMgr.setColor<true>(getColor());

        return false;
    };

    sliderA().sigChanged += [this](Vector2d value) {
        MyApp::getInstance().toolMgr.setAlpha<true>(getAlpha());

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


SizePicker::SizePicker(Widget *parent_, const Rect<double> &region_) :
    Base(parent_, region_, MIN_SIZE, MAX_SIZE, DEFAULT_SIZE) {}


ToolsWidget::ToolsWidget(Widget *parent_, const Rect<double> &region_) :
    Base(parent_, region_, nullptr, nullptr, nullptr) {

    const double height = region.h();
    const double width = region.w();

    const double colW[2] = {width * 0.4, width * 0.6};
    const double colX[2] = {0, colW[0]};

    const double sliderHeight = abel::gui::widgets::Thumb::DEFAULT_SIZE * 1.15;
    const double paletteHeight = colW[0] + 2 * sliderHeight;

    const Rect<double> regionColorPicker = Rect<double>::wh(colX[0], 0,             colW[0], paletteHeight);
    const Rect<double> regionSizePicker  = Rect<double>::wh(colX[0], paletteHeight, colW[0], sliderHeight);
    const Rect<double> regionTools       = Rect<double>::wh(colX[1], 0,             colW[1], height);

    colorPicker(new ColorPicker(nullptr, regionColorPicker));
    sizePicker(new SizePicker(nullptr, regionSizePicker));
    toolButtons(new _tool_buttons_type(nullptr, regionTools, abel::gui::widgets::LAD_VERTICAL));

    const double btnX = 0;
    const double btnW = colW[1] * 0.9;
    const double btnH = 30;

    toolButtons()
        .createChild(Rect<double>::wh(btnX, 0, btnW, btnH), "Brush")
        .sigClick += []() {

        MyApp::getInstance().toolMgr.selectBasicTool(ToolManager::BTT_BRUSH);

        return false;
    };

    toolButtons()
        .createChild(Rect<double>::wh(btnX, 0, btnW, btnH), "Eraser")
        .sigClick += []() {

        MyApp::getInstance().toolMgr.selectBasicTool(ToolManager::BTT_ERASER);

        return false;
    };

    toolButtons()
        .createChild(Rect<double>::wh(btnX, 0, btnW, btnH), "Picker")
        .sigClick += []() {

        MyApp::getInstance().toolMgr.selectBasicTool(ToolManager::BTT_COLOR_PICKER);

        return false;
    };


    MyApp::getInstance().toolMgr.sigConfigChanged += [this](ToolManager &mgr) {
        sizePicker().setSize(mgr.getRadius() * 2);

        return false;
    };

    sizePicker().sigChanged += [](Vector2d value) {
        MyApp::getInstance().toolMgr.setRadius<true>(value.x() / 2);

        return false;
    };
}
