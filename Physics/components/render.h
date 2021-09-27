#ifndef COMPONENTS_RENDER_H
#define COMPONENTS_RENDER_H

#include "component.h"


class Texture;

class RenderComp : public Component {
public:
    DECLARE_ERROR(error, abel::error);

    static constexpr double RENDER_INTERVAL = 0.5d;


    RenderComp(Molecule &object_) noexcept;

    void update(double deltaT) noexcept;

    virtual void render(Texture &target) = 0;

    virtual ~RenderComp() noexcept = default;

    virtual RenderComp *copy() = 0;

protected:
    double renderTimer;

};


class CircleRenderComp : public RenderComp {
public:
    CircleRenderComp(Molecule &object_, double radius_) noexcept;

    virtual void render(Texture &target) override;

    virtual CircleRenderComp *copy() override;

protected:
    double radius;

};


class SquareRenderComp : public RenderComp {
public:
    SquareRenderComp(Molecule &object_, double side_) noexcept;

    virtual void render(Texture &target) override;

    virtual SquareRenderComp *copy() override;

protected:
    double side;

};


#endif // COMPONENTS_RENDER_H

