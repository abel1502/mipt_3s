#ifndef GUI_COORDS_H
#define GUI_COORDS_H

#include <ACL/general.h>
#include <ACL/math/vector.h>
#include <ACL/gui/rect.h>

using abel::gui::Rect;
using abel::math::Vector2i;
using abel::math::Vector2d;


class Coords {
public:
    template <bool NOVALIDATE = false>
    constexpr Coords(const Rect<double> &screenPlane_, const Rect<double> &virtualPlane_) :
        screenPlane{screenPlane_}, virtualPlane{virtualPlane_} {

        if constexpr (!NOVALIDATE) {
            if (!screenPlane.getStart().isZero()) {
                DBG("Weird coordinate system chosen. "
                    "Are you sure you don't want the screen coordinates "
                    "to start at the top left corner?");
            }
        }
    }

    template <bool NOVALIDATE = false>
    constexpr Coords(const Rect<double> &screenPlane_, double maxVirtX) :
        screenPlane{screenPlane_}, virtualPlane{} {

        if constexpr (!NOVALIDATE) {
            if (!screenPlane.getStart().isZero()) {
                DBG("Weird coordinate system chosen. "
                    "Are you sure you don't want the screen coordinates "
                    "to start at the top left corner?");
            }

            REQUIRE(!abel::isZero(screenPlane.h()) && !abel::isZero(screenPlane.w()));
        }

        double maxVirtY = maxVirtX * screenPlane.h() / screenPlane.w();

        virtualPlane = Rect<double>::se(-maxVirtX, -maxVirtY, maxVirtX,  maxVirtY);
    }

    constexpr Vector2d scale_v2s(Vector2d virt) const {
        virt.x() *= screenPlane.w() / virtualPlane.w();
        virt.y() *= screenPlane.h() / virtualPlane.h();

        return virt;
    }

    constexpr Vector2d scale_s2v(Vector2d screen) const {
        screen.x() *= virtualPlane.w() / screenPlane.w();
        screen.y() *= virtualPlane.h() / screenPlane.h();

        return screen;
    }

    constexpr Vector2d virt2screen(const Vector2d &virt) const {
        return screenPlane.getStart() + scale_v2s(virt - virtualPlane.getStart());
    }

    constexpr Vector2d screen2virt(const Vector2d &screen) const {

        return virtualPlane.getStart() + scale_s2v(screen - screenPlane.getStart());
    }

    constexpr Rect<double> virt2screen(const Rect<double> &virt) const {
        return Rect<double>::wh(virt2screen(virt.getStart()), scale_v2s(virt.getDiag()));
    }

    constexpr Rect<double> screen2virt(const Rect<double> &screen) const {
        return Rect<double>::wh(screen2virt(screen.getStart()), scale_s2v(screen.getDiag()));
    }

    constexpr const Rect<double> &getScreenPlane () const { return screenPlane;  }
    constexpr const Rect<double> &getVirtualPlane() const { return virtualPlane; }

protected:
    Rect<double>  screenPlane;
    Rect<double> virtualPlane;

};


#endif // GUI_COORDS_H
