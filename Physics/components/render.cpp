#include "../gui/gui.h"

#include "../molecule.h"
#include "render.h"
#include "physics.h"


constexpr const Vector2d &RenderComp::getPos() const noexcept {
    return getObj().getComp<PhysComp>().getPos();
}

void CircleRenderComp::render(Texture &target, const Coords &coords) {
    render(target, coords, Color::ORANGE);
}

void CircleRenderComp::render(Texture &target, const Coords &coords, const Color &color) {
    Vector2d dims{radius, radius};

    try {
        target.drawEllipse(coords.virt2screen(getPos()), coords.scale_v2s(dims), color);
    } catch (const gui_error &e) {
        throw error(e.what());
    }
}

CircleRenderComp *CircleRenderComp::copy() const {
    return new CircleRenderComp(object, radius);
}

void CircleRenderComp::scale(double coeff) {
    radius *= coeff;
}

void CircleRenderComp::scaleTo(double dismension) {
    radius = dismension / 2;
}

CircleRenderComp *CircleRenderComp::becomeCircle() {
    return this;
}

SquareRenderComp *CircleRenderComp::becomeSquare() {
    SquareRenderComp *square = new SquareRenderComp(object, radius * 2);
    delete this;
    return square;
}

void SquareRenderComp::render(Texture &target, const Coords &coords) {
    render(target, coords, Color::LBLUE);
}

void SquareRenderComp::render(Texture &target, const Coords &coords, const Color &color) {
    Vector2d dims{side, side};
    Vector2d from = getPos() - dims / 2;

    try {
        target.drawRect(coords.virt2screen(Rect<double>(from, dims)), color);
    } catch (const gui_error &e) {
        throw error(e.what());
    }
}

SquareRenderComp *SquareRenderComp::copy() const {
    return new SquareRenderComp(object, side);
}

void SquareRenderComp::scale(double coeff) {
    side *= coeff;
}

void SquareRenderComp::scaleTo(double dismension) {
    side = dismension;
}

CircleRenderComp *SquareRenderComp::becomeCircle() {
    CircleRenderComp *circle = new CircleRenderComp(object, side / 2);
    delete this;
    return circle;
}

SquareRenderComp *SquareRenderComp::becomeSquare() {
    return this;
}

