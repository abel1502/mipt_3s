#pragma once

#include "../tool.h"
#include <ACL/vector.h>
#include "../spline.h"

class SplineEffect : public Effect {
public:
    SplineEffect(ToolManager *manager_);

    virtual void apply(Layer &layer) final override;

    virtual bool supportsSettings() const final override;

    void updateSplineBuf(const Spline::SampleArr &samples);

    void resetSplineBuf();

protected:
    static constexpr unsigned PIXEL_PRECISION = 256;


    unsigned char splineBuf[PIXEL_PRECISION] = {};


    virtual Vector2d getDesiredSettingsWndSize() const final override;
    virtual void populateSettingsWindow() final override;

};
