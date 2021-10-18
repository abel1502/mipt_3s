#ifndef BORDERS_H
#define BORDERS_H

#include <ACL/math/vector.h>


using abel::math::Vector2d;

namespace abel::gui {
class Texture;
class Coords;
}

using abel::gui::Texture;
using abel::gui::Coords;

class Border {
public:
    constexpr Border(const Vector2d &point_, const Vector2d &dirIn_) noexcept :
        point{point_}, dirIn{dirIn_.normalized()} {}

    /// Positive for inwards, negative for outwards
    constexpr double getDist(const Vector2d &from) const noexcept {
        return (from - point) * dirIn;
    }

    constexpr bool isInside (const Vector2d &testPoint) const noexcept { return getDist(testPoint) >  0.d; }
    constexpr bool isOutside(const Vector2d &testPoint) const noexcept { return getDist(testPoint) <= 0.d; }

    constexpr const Vector2d &getDirIn()  const noexcept { return  dirIn; }
    constexpr       Vector2d  getDirOut() const noexcept { return -dirIn; }
    constexpr const Vector2d &getPoint()  const noexcept { return point; }
    constexpr       Vector2d  getLine()   const noexcept { return dirIn.orthogonalized(); }

    void render(Texture &target, const Coords &coords) const;

protected:
    Vector2d point;
    Vector2d dirIn;  /// Normalized

};


#endif // BORDERS_H
