#pragma once

#include "../tool.h"
#include <ACL/vector.h>
#include "../spline.h"

class SplineEffect final : public Effect {
public:
    SplineEffect(ToolManager *manager_);

    virtual void apply(Layer &layer) override;

    virtual bool supportsSettings() const override;

    void updateSplineBuf(const Spline::SampleArr &samples);

    void resetSplineBuf();

protected:
    static constexpr unsigned PIXEL_PRECISION = 256;


    unsigned char splineBuf[PIXEL_PRECISION] = {};


    virtual Vector2d getDesiredSettingsWndSize() const override;
    virtual void populateSettingsWindow() override;

};
