#include <AGF/llgui.h>
#include <AGF/widget.h>
#include "spline_eff.h"
#include "../app.h"
#include <ACL/math/cmath.h>


namespace {

class SplineEffSettingsWidget : public widgets::StaticGroup<Spline,
                                                            widgets::Button,
                                                            widgets::Button> {
public:
    using Base = widgets::StaticGroup<Spline,
                                      widgets::Button,
                                      widgets::Button>;
    EVENT_HANDLER_USING(Base);


    SplineEffSettingsWidget(widgets::Window *parent_, const Rect<double> &region_,
                            SplineEffect &effect_);

protected:
    friend class SplineEffect;

    SplineEffect &effect;


    SGRP_DECLARE_BINDING_I(spline, 0);
    SGRP_DECLARE_BINDING_I(btnApply, 1);
    SGRP_DECLARE_BINDING_I(btnReset, 2);

};

SplineEffSettingsWidget::SplineEffSettingsWidget(widgets::Window *parent_,
                                                 const Rect<double> &region_,
                                                 SplineEffect &effect_) :
    Base(parent_, region_,
         nullptr,
         nullptr,
         nullptr),
    effect{effect_} {

    constexpr double PAD = 5;

    const double BTN_HEIGHT = 26;

    Rect<double> splineRegion = region;
    splineRegion.pad(PAD * 2);
    Rect<double> btnsRegion = splineRegion;

    splineRegion.h(splineRegion.h() - BTN_HEIGHT - PAD);
    btnsRegion.y0(splineRegion.y1() + PAD);

    Rect<double> btnRegions[2] = {btnsRegion, btnsRegion};
    btnRegions[0].w(btnRegions[0].w() / 2 - PAD / 2);
    btnRegions[1].x0(btnRegions[0].x1() + PAD / 2);

    spline(new Spline(this, splineRegion));
    btnApply(new widgets::Button(this, btnRegions[0], "Apply"));
    btnReset(new widgets::Button(this, btnRegions[1], "Reset"));

    spline().sigChanged += [this](Spline &spline) {
        effect.updateSplineBuf(spline.getSamples());

        Canvas *activeCanvas = MyApp::getInstance().toolMgr.getActiveCanvas();

        if (activeCanvas) {
            activeCanvas->activeLayer().beginPreview();
            effect.apply(activeCanvas->activeLayer());
        }

        return false;
    };

    sigDeath += [](Widget &) {
        Canvas *activeCanvas = MyApp::getInstance().toolMgr.getActiveCanvas();
        if (activeCanvas) {
            activeCanvas->activeLayer().endPreview(false);
        }
    };

    btnApply().sigClick += [this]() {
        Canvas *activeCanvas = MyApp::getInstance().toolMgr.getActiveCanvas();
        if (!activeCanvas) {
            return false;
        }

        activeCanvas->activeLayer().endPreview(true);

        widgets::Window *wnd = dynamic_cast<widgets::Window *>(getParent());
        if (wnd) {
            wnd->die();
        }

        return false;
    };

    btnReset().sigClick += [this]() {
        spline().resetPoints();
        effect.resetSplineBuf();

        return false;
    };
}

}


SplineEffect::SplineEffect(ToolManager *manager_) :
    Effect(manager_) {

    resetSplineBuf();
}

void SplineEffect::apply(Layer &layer) {
    if (settingsWnd) {
        updateSplineBuf(dynamic_cast<SplineEffSettingsWidget &>(settingsWnd->getContents()).spline().getSamples());
    }

    abel::gui::PackedColor *srcBuf  = layer.getTexture().getBuf(true, false);
    abel::gui::PackedColor *destBuf = layer.getPreview().getBuf(false, true);

    const unsigned limit = layer.getTexture().width() * layer.getTexture().height();
    for (unsigned i = 0; i < limit; ++i) {
        destBuf[i].R = splineBuf[srcBuf[i].R];
        destBuf[i].G = splineBuf[srcBuf[i].G];
        destBuf[i].B = splineBuf[srcBuf[i].B];
        destBuf[i].A = srcBuf[i].A;
    }

    layer.getTexture().flushBuf();
    layer.getPreview().flushBuf();

    layer.setFlushPolicy(true);
}

bool SplineEffect::supportsSettings() const {
    return true;
}

Vector2d SplineEffect::getDesiredSettingsWndSize() const {
    return Vector2d{250, 270};
}

void SplineEffect::populateSettingsWindow() {
    Effect::populateSettingsWindow();

    settingsWnd->setContents(new SplineEffSettingsWidget(nullptr,
                                                         Rect<double>::wh(Vector2d{}, getDesiredSettingsWndSize()),
                                                         *this));
    settingsWnd->getTitleLabel().setText("Spline config");
}

void SplineEffect::updateSplineBuf(const Spline::SampleArr &samples) {
    assert(!samples.isEmpty());

    Vector2d lastSample = samples[0];
    unsigned sampleIdx = 1;
    for (unsigned i = 0;
         i < PIXEL_PRECISION &&
         sampleIdx < samples.getSize();
         ++i) {

        const double curX = (double)i / (PIXEL_PRECISION - 1);

        while (sampleIdx < samples.getSize() &&
               samples[sampleIdx].x() < curX) {

            lastSample = samples[sampleIdx];
            ++sampleIdx;
        }

        if (sampleIdx == samples.getSize()) {
            splineBuf[i] = (unsigned char)abel::math::clamp(lastSample.y() * 256, 0., 255.);
            continue;
        }

        const Vector2d &sample = samples[sampleIdx];

        if (abel::cmpDbl(lastSample.x(), sample.x()) == 0) {
            splineBuf[i] = (unsigned char)abel::math::clamp((sample.y() + lastSample.y()) / 2 * 256, 0., 255.);
        } else {
            splineBuf[i] = (unsigned char)abel::math::clamp(
                (lastSample +
                (sample - lastSample) *
                (curX       - lastSample.x()) /
                (sample.x() - lastSample.x())).y() * 256, 0., 255.);
        }

        // DBG("[%u] %hhu", i, splineBuf[i]);
    }
}

void SplineEffect::resetSplineBuf() {
    for (unsigned i = 0; i < PIXEL_PRECISION; ++i) {
        splineBuf[i] = (unsigned char)i;
    }
}