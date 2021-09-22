#include "raycaster.h"


#include <ACL/math/binpow.h>
#include <limits>


using abel::math::binPow;


bool Sphere::castRay(const Point &from, Vector3f dir, CollisionInfo *result) const {
    dir.normalize();  // TODO: Maybe instead require dir.magnitude == 1?
    float distToMid = (center - from) * dir;

    if (distToMid < 0.f)
        return false;

    float offsetFromCenter = ((center - from) & dir).length();  // TODO: Cache?

    if (offsetFromCenter > radius)
        return false;

    float intersectionOffset = std::sqrt(radius * radius - offsetFromCenter * offsetFromCenter);
    assert(!std::isnan(intersectionOffset));

    if (!result)
        return true;

    float dist = (intersectionOffset < distToMid ?
                  distToMid - intersectionOffset :
                  distToMid + intersectionOffset);

    result->from = from;
    result->at = from + dir * dist;
    result->dir = dir;
    result->dist = dist;
    result->normal = (result->at - center).normalized();  // TODO: Maybe normalize, since a - b is already a tmp vector
    result->color = color;
    result->specularIntensity = specularIntensity;

    return true;
}


void Scene::tick(double deltaT) {
    constexpr float LIGHT_ROTATION_SPEED = 10;

    abel::math::Vector2f tmp{lights[0].pos.x(), lights[0].pos.y()};

    tmp.rotateDegrees(deltaT * LIGHT_ROTATION_SPEED);

    lights[0].pos.x() = tmp.x();
    lights[0].pos.y() = tmp.y();
}


void Scene::render(PackedColor *buf, unsigned width, unsigned height) const {
    constexpr float LIGHT_DIST_IMPACT = 0.08f;

    assert(buf);

    #define BUF_(X, Y)  buf[(Y) * width + (X)]

    CollisionInfo collision{},
                  bestCollision{};
    const Sphere *collisionObj = nullptr;
    Vector3f curDir{};

    float camDepth = 0.8f * (float)std::hypot(width, height);  // Just for scaling purposes

    Color result{};

    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            curDir = camera.getDir(Vector3f{(float)x * 2 - width, -(float)y * 2 + height, camDepth});

            bestCollision.dist = std::numeric_limits<float>::infinity();
            collisionObj = nullptr;

            for (const Sphere &sphere : objects) {
                if (!sphere.castRay(camera.pos, curDir, &collision)) {
                    continue;
                }

                if (collision.dist < bestCollision.dist) {
                    bestCollision = collision;
                    collisionObj = &sphere;
                }
            }

            if (!collisionObj) {
                BUF_(x, y) = PackedColor{0, 0, 0};
                continue;
            }

            //result = bestCollision.normal.x() > 0 ? Color{1, 0, 0} : Color{0, 1, 0};

            result = bestCollision.color.reflected(ambientColor);

            for (const Light &light : lights) {
                bool shadowed = false;

                Vector3f lightRay = bestCollision.at - light.pos;

                for (const Sphere &sphere : objects) {
                    if (collisionObj == &sphere)
                        continue;

                    if (!sphere.castRay(light.pos, lightRay, &collision))
                        continue;

                    if (collision.dist * collision.dist < lightRay.magnitude()) {
                        shadowed = true;
                        break;
                    }
                }

                // Normalize instead of -ed for optimization purposes, since it's applied to an rvalue
                float lightAngleCos = (-lightRay).normalize() * bestCollision.normal;

                shadowed = shadowed || lightAngleCos < 0;

                //result = shadowed ? Color{0, 0, 1} : Color{1, 0, 0};
                //break;

                if (shadowed)
                    continue;

                float distCoeff = lightRay.magnitude() * LIGHT_DIST_IMPACT;
                // (bestCollision.at - bestCollision.from).magnitude()

                result += light.color.reflected(bestCollision.color) * lightAngleCos * light.intensity / distCoeff;

                result += light.color * binPow(lightAngleCos, bestCollision.specularIntensity);

                //result = lightAngleCos > 0 ? Color{1, 0, 0} : Color{0, 1, 0};
            }

            BUF_(x, y) = result.pack();
        }
    }



    #undef BUF_
}
