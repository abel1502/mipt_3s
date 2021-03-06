#include <AGF/llgui.h>
#include "eraser.h"


void EraserTool::applyLine(Layer &layer, const Vector2d &pos0, const Vector2d &pos1) {
    abel::gui::Texture &target = layer.getTexture();

    double radius = configureTarget(target);

    target.setOverwrite();
    target.drawLine(pos0, pos1);
    target.drawCircle(pos0, radius);
    target.drawCircle(pos1, radius);
    target.resetOverwrite();
}

void EraserTool::applyPoint(Layer &layer, const Vector2d &pos) {
    abel::gui::Texture &target = layer.getTexture();

    double radius = configureTarget(target);

    target.drawCircle(pos, radius);
}

double EraserTool::configureTarget(abel::gui::Texture &target) const {
    double radius = getManager().getRadius();

    target.setOverwrite();
    target.setLineWidth((float)radius * 2);
    target.setLineColor(Color::WHITE, 0);
    target.setFillColor(Color::WHITE, 0);
    target.resetOverwrite();

    return radius;
}