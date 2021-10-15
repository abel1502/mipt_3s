#ifndef COMPONENTS_RENDER_H
#define COMPONENTS_RENDER_H

#include "component.h"
#include "../gui/coords.h"
#include <ACL/math/vector.h>
#include <ACL/gui/color.h>
#include <ACL/math/cmath.h>


class Texture;
using Color = abel::gui::Color;
class CircleRenderComp;
class SquareRenderComp;

class RenderComp : public Component {
public:
    DECLARE_ERROR(error, abel::error);


    inline RenderComp(Molecule *object_) noexcept :
        Component(object_) {}

    virtual void render(Texture &target, const Coords &coords) = 0;

    virtual void render(Texture &target, const Coords &coords, const Color &color) = 0;

    virtual ~RenderComp() noexcept = default;

    virtual RenderComp *copy() const = 0;

    virtual void scale(double coeff) = 0;

    virtual void scaleTo(double dismension) = 0;

    // TODO: Maybe virtual, but for now this will do
    inline void scaleTrue(double coeff) {
        scale(std::sqrt(coeff));
    }

    /// This function may delete the `this` instance!
    virtual CircleRenderComp *becomeCircle() = 0;

    /// This function may delete the `this` instance!
    virtual SquareRenderComp *becomeSquare() = 0;

protected:
    constexpr const abel::math::Vector2d &getPos() const noexcept;

};


class CircleRenderComp : public RenderComp {
public:
    inline CircleRenderComp(Molecule *object_, double radius_) noexcept :
        RenderComp(object_), radius{radius_} {}

    virtual void render(Texture &target, const Coords &coords) override;

    virtual void render(Texture &target, const Coords &coords, const Color &color) override;

    virtual CircleRenderComp *copy() const override;

    virtual ~CircleRenderComp() noexcept override = default;

    virtual void scale(double coeff) override;

    virtual void scaleTo(double dismension) override;

    /// This function may delete the `this` instance!
    virtual CircleRenderComp *becomeCircle() override;

    /// This function may delete the `this` instance!
    virtual SquareRenderComp *becomeSquare() override;

protected:
    double radius;

};


class SquareRenderComp : public RenderComp {
public:
    inline SquareRenderComp(Molecule *object_, double side_) noexcept :
        RenderComp(object_), side{side_} {}

    virtual void render(Texture &target, const Coords &coords) override;

    virtual void render(Texture &target, const Coords &coords, const Color &color) override;

    virtual SquareRenderComp *copy() const override;

    virtual ~SquareRenderComp() noexcept override = default;

    virtual void scale(double coeff) override;

    virtual void scaleTo(double dismension) override;

    /// This function may delete the `this` instance!
    virtual CircleRenderComp *becomeCircle() override;

    /// This function may delete the `this` instance!
    virtual SquareRenderComp *becomeSquare() override;

protected:
    double side;

};


#endif // COMPONENTS_RENDER_H

