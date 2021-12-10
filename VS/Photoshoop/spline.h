#pragma once

#include <AGF/widget.h>
#include <AGF/widgets/all.h>
#include <ACL/general.h>
#include <ACL/vector.h>
#include <ACL/gui/coords.h>
#include <AGF/helpers/mouse_tracker.h>
#include <ACL/cmp.h>


using abel::gui::Rect;
using abel::gui::Color;
using abel::math::Vector2d;
namespace widgets = abel::gui::widgets;


class Spline : public abel::gui::Widget {
public:
    using Base = abel::gui::Widget;
    EVENT_HANDLER_USING(Base);

    using SampleArr = abel::vector<Vector2d>;

    static constexpr Vector2d THUMB_SIZE{6, 6};
    static constexpr double THUMB_GRAB_RADIUS = 7.5;


    struct Point {
        struct {
            // Whether the point is rendered and shown to the user
            bool hidden : 1;
            // Whether it's locked from movement by the user
            bool locked : 1;
            // Whether it's currently being dragged, and thus deserves special treatment
            bool active : 1;
            // Whether the point should be smooth or sharp
            bool smooth : 1;
        };

        Vector2d value{};


        inline explicit Point(const Vector2d &value_) :
            value{value_},
            hidden{false},
            locked{false},
            active{false},
            smooth{true } {}

        inline bool operator<(const Point &other) const noexcept {
            return value.x() < other.value.x();
        }
    };


    Spline(Widget *parent_, const Rect<double> &region_);

    EVENT_HANDLER_OVERRIDE(abel::gui::Render);

    EVENT_HANDLER_OVERRIDE(abel::gui::MouseClick);

    EVENT_HANDLER_OVERRIDE(abel::gui::MouseMove);

    const SampleArr &getSamples() const;

protected:
    static constexpr Rect<double> limits{Rect<double>::wh(0, 1, 1, -1)};
    static constexpr unsigned BAD_IDX = -1u;


    abel::gui::MouseTracker mt{this};
    abel::vector<Point> points{};
    unsigned activePointIdx = BAD_IDX;

    mutable bool _cachedSamplesValid = false;
    mutable SampleArr _cachedSamples{};

    mutable bool _cachedTextureValid = false;
    mutable abel::unique_ptr<abel::gui::Texture> _cachedTexture = nullptr;


    constexpr Vector2d valueToPos(const Vector2d &value) const {
        abel::gui::Coords coords{getBounds(), limits};

        return coords.virt2screen(value);
    }

    Vector2d posToValue(const Vector2d &pos) const {
        abel::gui::Coords coords{getBounds(), limits};

        return coords.screen2virt(pos);
    }

    inline void invalidateSamplesCache() const {
        _cachedSamplesValid = false;
        _cachedTextureValid = false;
    }

    constexpr Rect<double> getBounds() const {
        return region.padded(THUMB_SIZE.x() / 2, THUMB_SIZE.y() / 2);
    }

    virtual void renderBackground(abel::gui::Texture &target, const Rect<double> &at);

    virtual void renderSpline(abel::gui::Texture &target);

    virtual void renderThumb(abel::gui::Texture &target, const Point &point);

    unsigned getClosestPointIdx(const Vector2d &pos) const;

private:
    void accumulateSegmentSamples(const Point *neighborL,
                                  const Point &pointL,
                                  const Point &pointR,
                                  const Point *neighborR) const;

    void cleanupDuplicateSamples() const;

};

