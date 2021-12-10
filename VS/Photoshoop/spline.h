#pragma once

#include <AGF/widget.h>
#include <AGF/widgets/all.h>
#include <ACL/general.h>
#include <ACL/vector.h>
#include <ACL/gui/coords.h>
#include <AGF/helpers/mouse_tracker.h>
#include <ACL/cmp.h>
#include <ACL/signal.h>


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
    static constexpr double THUMB_GRAB_RADIUS = 7;


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


    // WARNING: Called OFTEN
    abel::Signal<bool (Spline &spline)> sigChanged{};


    Spline(Widget *parent_, const Rect<double> &region_);

    EVENT_HANDLER_OVERRIDE(abel::gui::Render);

    EVENT_HANDLER_OVERRIDE(abel::gui::MouseClick);

    EVENT_HANDLER_OVERRIDE(abel::gui::MouseMove);

    const SampleArr &getSamples() const;

protected:
    static constexpr bool DEBUG_DRAW_SAMPLES = false;
    static constexpr Rect<double> limits{Rect<double>::wh(0, 1, 1, -1)};
    static constexpr unsigned BAD_IDX = -1u;


    abel::gui::MouseTracker mt{this};
    abel::vector<Point> points{};
    abel::gui::MouseBtn draggingButton{};
    unsigned activePointIdx = BAD_IDX;
    Vector2d dragOffset{};

    mutable bool _cachedSamplesValid = false;
    mutable SampleArr _cachedSamples{};

    mutable bool _cachedTextureValid = false;
    mutable abel::unique_ptr<abel::gui::Texture> _cachedTexture = nullptr;


    constexpr abel::gui::Coords getCoords() const {
        return abel::gui::Coords{getBounds(), limits};
    }

    constexpr Vector2d valueToPos(const Vector2d &value) const {
        return getCoords().virt2screen(value);
    }

    Vector2d posToValue(const Vector2d &pos) const {
        return getCoords().screen2virt(pos);
    }

    inline void invalidateSamplesCache() {
        _cachedSamplesValid = false;
        _cachedTextureValid = false;

        sigChanged(*this);
    }

    constexpr Rect<double> getBounds() const {
        return region.padded(THUMB_SIZE.x() / 2, THUMB_SIZE.y() / 2);
    }

    virtual void renderBackground(abel::gui::Texture &target, const Rect<double> &at);

    virtual void renderSpline(abel::gui::Texture &target);

    virtual void renderThumb(abel::gui::Texture &target, const Point &point);

    inline Point &getActivePoint() {
        assert(activePointIdx != BAD_IDX);

        return points[activePointIdx];
    }

    inline const Point &getActivePoint() const {
        return const_cast<Spline *>(this)->getActivePoint();
    }

    inline void forgetActivePoint() {
        if (activePointIdx != BAD_IDX) {
            points[activePointIdx].active = false;
        }

        activePointIdx = BAD_IDX;
    }

    unsigned getClosestFreePointIdx(const Vector2d &pos) const;

    inline unsigned addPointAt(const Vector2d &pos) {
        return addPoint(posToValue(pos).clamped({0, 0}, {1, 1}));
    }

    unsigned addPoint(const Vector2d &value);

    void removePoint(unsigned idx);

    void moveActivePoint(const Vector2d &by);

    inline bool isActivePointMisplaced() const {
        return activePointIdx != BAD_IDX && isPointMisplaced(activePointIdx);
    }

    bool isPointMisplaced(unsigned idx) const;

private:
    void accumulateSegmentSamples(const Point *neighborL,
                                  const Point &pointL,
                                  const Point &pointR,
                                  const Point *neighborR) const;

    void cleanupDuplicateSamples() const;

};

