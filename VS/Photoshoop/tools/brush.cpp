#include <AGF/llgui.h>
#include "brush.h"


void BrushTool::applyLine(Layer &layer, const Vector2d &pos0, const Vector2d &pos1) {
    abel::gui::Texture &target = layer.getPreview();

    double radius = configureTarget(target);

    target.drawLine(pos0, pos1);
    target.drawCircle(pos0, radius);
    target.drawCircle(pos1, radius);
}

void BrushTool::applyPoint(Layer &layer, const Vector2d &pos) {
    abel::gui::Texture &target = layer.getPreview();

    double radius = configureTarget(target);

    target.drawCircle(pos, radius);
}

double BrushTool::configureTarget(abel::gui::Texture &target) const {
    double radius = getManager().getRadius();
    Color  color  = getManager().getColor();
    double alpha  = getManager().getAlpha();

    target.setLineWidth((float)radius);
    target.setLineColor(color, (float)alpha);
    target.setFillColor(color, (float)alpha);

    return radius;
}
