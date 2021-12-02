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

    double height = region.h();

    Rect<double> regionSV = region;
    regionSV.h(height * 0.7);

    Rect<double> regionH = region;
    regionH.y(height * 0.75);
    regionH.h(height * 0.1);

    Rect<double> regionA = region;
    regionA.y(height * 0.9);
    regionA.h(height * 0.1);

    sliderSV(new _myimpl::ColorSliderSV(this, regionSV, Rect<double>::wh(0, 1, 1, -1)));
    sliderH (new _myimpl::ColorSliderH (this, regionH , 0, 1));
    sliderA (new _myimpl::ColorSliderA (this, regionA , 0, 1));

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