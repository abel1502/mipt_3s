#include <AGF/llgui.h>
#include "spline.h"


Spline::Spline(Widget *parent_, const Rect<double> &region_) :
    Base(parent_, region_) {

    points.append(Point{Vector2d{0, 0}});
    points.append(Point{Vector2d{0.6, 0.4}});
    points.append(Point{Vector2d{1, 1}});

    mt.sigDown += [this](const abel::gui::MouseClickEvent &event) {
        assert(activePointIdx == BAD_IDX);

        unsigned closestIdx = getClosestPointIdx(event.pos);

        Point &point = points[closestIdx];
        Vector2d pointPos = valueToPos(point.value);

        if (abel::cmpDbl((pointPos - event.pos).magnitude(),
                         THUMB_GRAB_RADIUS) <= 0) {
            // It's close enough, we grab the point

            activePointIdx = closestIdx;
        } else {
            // It's too far, we create another

            Point newPoint{posToValue(event.pos).clamped({0, 0}, {1, 1})};

            // TODO: Finish
        }

        return false;
    };

    mt.sigUp += [this](const abel::gui::MouseClickEvent &event) {
        return false;
    };

    mt.sigDragStateChange += [this](abel::gui::MouseBtn btn, abel::gui::MouseAttrs attrs, bool state) {
        return false;
    };

    mt.sigDrag += [this](abel::gui::MouseBtn btn, const abel::gui::MouseMoveEvent &event) {
        return false;
    };

    // TODO: Finish
}

EVENT_HANDLER_IMPL(Spline, abel::gui::MouseClick) {
    return mt.processEvent(event, Base::dispatchEvent(event));
}

EVENT_HANDLER_IMPL(Spline, abel::gui::MouseMove) {
    return mt.processEvent(event, Base::dispatchEvent(event));
}

unsigned Spline::getClosestPointIdx(const Vector2d &pos) const {
    // TODO: Finish
    return BAD_IDX;
}

#pragma region Rendering
EVENT_HANDLER_IMPL(Spline, abel::gui::Render) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    if (!_cachedTextureValid) {
        _cachedTexture = new abel::gui::Texture(region);

        renderBackground(*_cachedTexture, getBounds());

        renderSpline(*_cachedTexture);

        for (const Point &point : points) {
            renderThumb(*_cachedTexture, point);
        }

        _cachedTextureValid = true;
    }

    event.target.embed(region, *_cachedTexture);

    return status;
}

void Spline::renderBackground(abel::gui::Texture &target, const Rect<double> &at) {
    target.setFillColor(Color::WHITE);
    // `region` and not `at`, because we want all of our widget to have this background
    target.drawRect(region);

    target.setLineColor(Color::WHITE * 0.6f);
    target.setLineWidth(1.f);
    target.clipPush(at.padded(-1, -1, -1, -1));

    constexpr unsigned GRID_STEPS = 9;

    // TODO: Plot based on values instead?
    const Vector2d gridStep = at.getDiag() / (GRID_STEPS - 1);
    Vector2d start = at.getStart();
    const Vector2d gridStepH{gridStep.x(), 0};
    const Vector2d gridStepV{0, gridStep.y()};
    const Vector2d gridSizeH{at.w(), 0};
    const Vector2d gridSizeV{0, at.h()};
    for (unsigned i = 0; i < GRID_STEPS; ++i) {
        target.drawLineAlong(start + gridStepV * i, gridSizeH);
        target.drawLineAlong(start + gridStepH * i, gridSizeV);
    }

    target.clipPop();
}

void Spline::renderSpline(abel::gui::Texture &target) {
    const SampleArr &samples = getSamples();

    target.setLineColor(Color::RED * 0.8f);
    target.setLineWidth(1.f);
    target.clipPush(getBounds().padded(-1, 0, -1, 0));

    for (unsigned i = 1; i < samples.getSize(); ++i) {
        target.drawLine(valueToPos(samples[i - 1]),
                        valueToPos(samples[i    ]));
    }

    target.clipPop();
}

void Spline::renderThumb(abel::gui::Texture &target, const Point &point) {
    if (point.hidden) {
        return;
    }

    target.setLineColor(Color::WHITE * 0.3f);
    target.setLineWidth(1.2f);
    target.drawEllipse(valueToPos(point.value), THUMB_SIZE / 2, false);
}
#pragma endregion Rendering

#pragma region Sampling
const Spline::SampleArr &Spline::getSamples() const {
    if (!_cachedSamplesValid) {
        _cachedSamples.clear();

        abel::vector<Point> pointsCopy = points;
        bool foundActive = false;
        for (unsigned i = 0; i < pointsCopy.getSize(); ++i) {
            if (foundActive) {
                assert(i > 0);

                pointsCopy[i - 1] = pointsCopy[i];
            }

            if (pointsCopy[i].active) {
                if ((i > 0                        && pointsCopy[i - 1] > pointsCopy[i]) ||
                    (i < pointsCopy.getSize() - 1 && pointsCopy[i] > pointsCopy[i + 1])) {
                    foundActive = true;
                }
            }
        }
        if (foundActive) {
            pointsCopy.pop();
        }

        assert(!pointsCopy.isEmpty());

        _cachedSamples.append(pointsCopy[0].value);
        _cachedSamples[-1].x() = 0;

        /*for (const Point &point : points) {
            _cachedSamples.append(point.value);
        }*/

        for (unsigned l = 0, r = 1; r < pointsCopy.getSize(); ++l, ++r) {
            const Point &pointL = pointsCopy[l];
            const Point &pointR = pointsCopy[r];
            const Point *neighborL =                        l > 0 ? &pointsCopy[l - 1] : nullptr;
            const Point *neighborR = r + 1 < pointsCopy.getSize() ? &pointsCopy[r + 1] : nullptr;

            if (!pointL.smooth) {
                neighborL = nullptr;
            }

            if (!pointR.smooth) {
                neighborR = nullptr;
            }

            accumulateSegmentSamples(neighborL, pointL, pointR, neighborR);
        }

        _cachedSamples.append(pointsCopy[-1].value);
        _cachedSamples[-1].x() = 1;

        cleanupDuplicateSamples();

        _cachedSamplesValid = true;
    }

    return _cachedSamples;
}

void Spline::accumulateSegmentSamples(const Point *neighborL,
                                      const Point &pointL,
                                      const Point &pointR,
                                      const Point *neighborR) const {
    constexpr unsigned NUM_SAMPLES = 10;

    // Cubic Bezier interpolation.
    // Generally inspired by GIMP,
    // see https://gitlab.gnome.org/GNOME/gimp/-/blob/2afcca4e/app/core/gimpcurve.c#L1178-1292

    const double x0 = pointL.value.x();
    const double y0 = pointL.value.y();

    double y1 = 0, y2 = 0;

    const double x3 = pointR.value.x();
    const double y3 = pointR.value.y();

    const double dx = x3 - x0;
    const double dy = y3 - y0;

    if (abel::sgnDbl(dx) <= 0) {
        _cachedSamples.append(Vector2d{x0, y3});

        return;
    }

    if (!neighborL && !neighborR) {
        y1 = y0 + dy / 3;
        y2 = y1 + dy / 3;
    } else {
        if (neighborL) {
            double slope = (y3 - neighborL->value.y()) /
                           (x3 - neighborL->value.x());

            y1 = y0 + slope * dx / 3;
        }

        if (neighborR) {
            double slope = (y0 - neighborR->value.y()) /
                           (x0 - neighborR->value.x());

            y2 = y3 - slope * dx / 3;
        }

        if (!neighborL) {
            y1 = (y0 + y2) / 2;
        }

        if (!neighborR) {
            y2 = (y1 + y3) / 2;
        }
    }

    constexpr double STATE_STEP = 1. / NUM_SAMPLES;
    double state = 0;
    double x = x0;
    const double stepX = dx / NUM_SAMPLES;
    for (unsigned i = 0; i < NUM_SAMPLES; ++i,
         state += STATE_STEP,
         x += stepX) {

        double y =
                y0 * (1 - state) * (1 - state) * (1 - state) +
            3 * y1 * (1 - state) * (1 - state) *      state  +
            3 * y2 * (1 - state) *      state  *      state  +
                y3 *      state  *      state  *      state;

        y = abel::math::clamp(y, 0., 1.);

        _cachedSamples.append(Vector2d{x, y});
    }
}

void Spline::cleanupDuplicateSamples() const {
    unsigned target = 1;

    for (unsigned i = 1; i < _cachedSamples.getSize(); ++i) {
        if (_cachedSamples[i - 1] != _cachedSamples[i]) {
            _cachedSamples[target] = _cachedSamples[i];
            target++;
        }
    }

    _cachedSamples.resize(target);
}
#pragma endregion Sampling
