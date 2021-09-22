#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <ACL/general.h>
#include <ACL/math/cmath.h>
#include <ACL/math/vector.h>
#include <ACL/vector.h>
#include <ACL/gui/color.h>


using abel::math::Vector3f;
using Point = Vector3f;
using abel::vector;
using abel::gui::Color;
using abel::gui::PackedColor;


DECLARE_ERROR(raycast_error, abel::error);


struct CollisionInfo {
    Point from;
    Point at;
    Vector3f dir;
    float dist;
    Vector3f normal;
    Color color;
    unsigned specularIntensity;
};


class Light {
public:
    Point pos;
    Color color;
    float intensity;
};


class Camera {
public:
    Point pos;
    Vector3f forward;
    Vector3f right;
    Vector3f up;

    /// x and y from -1.f to 1.f
    constexpr Vector3f getDir(float x, float y) const noexcept {
        return forward + right * x + up * y;
    }

    /// This one pretty much just rotates dir accordingly
    constexpr Vector3f getDir(const Vector3f &dir) const noexcept {
        return forward * dir.z() + right * dir.x() + up * dir.y();
    }

};


class Sphere {
public:
    constexpr Sphere() :
        center{0, 0, 0}, radius{1}, color{1, 0, 0}, specularIntensity{5} {}

    constexpr Sphere(const Point &new_center, float new_radius, const Color &new_color,
                     unsigned new_specularIntensity) :
        center{new_center}, radius{new_radius}, color{new_color},
        specularIntensity{new_specularIntensity} {}

    /// Returns whether the collision has occured
    // Could have sent it through result, but it would be slower, and we're dealing with graphics here
    bool castRay(const Point &from, Vector3f dir, CollisionInfo *result) const;

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


class Scene {
public:
    vector<Sphere> objects;
    vector<Light> lights;
    Color ambientColor;
    Camera camera;

    Scene() :
        objects{{Sphere{Point{0, 0, 0},  4,   Color{1.f, 0.f,  0.f},   5},
                 Sphere{Point{-6, 0, 1}, 1.5, Color{1.f, 0.7f, 0.1f}, 10}}},
        lights{{Light{Point{14, 0, 3}, Color{1.f, 1.f, 1.f},   0.7f},
                Light{Point{0, 0, 7}, Color{0.3f, 0.3f, 0.3f}, 1.8f}}},
        ambientColor{0.1f, 0.1f, 0.1f},
        camera{Point{-2, 0, 10},
               Vector3f{0, 0, -10},
               Vector3f{10, 0, 0},
               Vector3f{0, 10, 0}} {}

    // TODO: An actual ctor

    void tick(double deltaT);

    void render(PackedColor *buf, unsigned width, unsigned height) const;

};


#endif // RAYCASTER_H
