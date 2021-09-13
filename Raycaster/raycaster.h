#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "ACL/general.h"
#include "ACL/math/vector.h"
#include "color.h"


using abel::math::Vector3f;
using Point = Vector3f;


class RenderedObject;
class Shape;
struct CollisionInfo;


class Scene {
public:
    //

protected:
    //

};


class Sphere {
public:
    constexpr Sphere() :
        radius{1}, color(1, 0, 0), specularIntensity{5} {}

    constexpr Sphere(const Point &new_center, float new_radius, const Color &new_color,
                     unsigned new_specularIntensity) :
        center{new_center}, radius{new_radius}, color{new_color},
        specularIntensity{new_specularIntensity} {}

    constexpr Sphere(const Sphere &other) :
        center{other.center}, radius{other.radius}, color{other.color},
        specularIntensity{other.specularIntensity} {}

    constexpr Sphere &operator=(const Sphere &other) {
        center = other.center;
        radius = other.radius;
        color = other.color;
        specularIntensity = other.specularIntensity;

        return *this;
    }

    /// Returns whether the collision has occured
    // Could have sent it through result, but it would be slower, and we're dealing with graphics here
    constexpr bool castRay(const Point &from, const Vector3f &dir, CollisionInfo *result) const {
        float distToMid = (center - from) * dir.normalized();  // TODO: Maybe instead require dir.magnitude == 1?

        if (distToMid < 0.f)
            return false;

        float offsetFromCenter = std::fabs((center - from) & dir.normalized());  // TODO: Cache?

        if (offsetFromCenter > radius)
            return false;

        float intersectionOffset = std::sqrt(offsetFromCenter * offsetFromCenter - radius * radius);

        if (result)
            return true;

        float dist = (intersectionOffset < distToMid ?
                        distToMid - intersectionOffset :
                        distToMid + intersectionOffset);
        result->sphere = *this;
        result->from = from;
        result->point = from + dir * dist;
        result->dist = dist;

        return true;
    }

    constexpr const Point &getCenter() const {
        return center;
    }

    constexpr float getRadius() const {
        return radius;
    }

    /// I'm pretty sure color shouldn't be cached, so I'll allow modifying it by reference
    constexpr Color &getColor() {
        return color;
    }

    constexpr const Color &getColor() const {
        return color;
    }

    constexpr unsigned getSpecularIntensity() const {
        return specularIntensity;
    }

protected:
    Point center;
    float radius;
    Color color;
    unsigned specularIntensity;

};


struct CollisionInfo {
    const Sphere &sphere;

    const Point &from
    Point point;
    float dist;
};


#endif // RAYCASTER_H
