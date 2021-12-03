#include <AGF/llgui.h>
#include "brush.h"


void BrushTool::applyLine(Layer &layer, const Vector2d &pos0, const Vector2d &pos1) {
    abel::gui::Texture &target = layer.getPreview();

    double radius = configureTarget(target);

    target.setOverwrite();
    target.drawLine(pos0, pos1);
    target.drawCircle(pos0, radius);
    target.drawCircle(pos1, radius);
    target.resetOverwrite();
}

void BrushTool::applyPoint(Layer &layer, const Vector2d &pos) {
    abel::gui::Texture &target = layer.getPreview();

    double radius = configureTarget(target);

    target.setOverwrite();
    target.drawCircle(pos, radius);
    target.resetOverwrite();
}

double BrushTool::configureTarget(abel::gui::Texture &target) const {
    double radius = getManager().getRadius();
    Color  color  = getManager().getColor();
    double alpha  = getManager().getAlpha();

    target.setLineWidth((float)radius * 2);
    target.setLineColor(color, (float)alpha);
    target.setFillColor(color, (float)alpha);

    return radius;
}
