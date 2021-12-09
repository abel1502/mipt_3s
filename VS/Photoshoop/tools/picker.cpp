#include <AGF/llgui.h>
#include "picker.h"


void PickerTool::applyLine(Layer &layer, const Vector2d &pos0, const Vector2d &pos1) {
    return applyPoint(layer, pos1);
}

void PickerTool::applyPoint(Layer &layer, const Vector2d &pos) {
    abel::gui::Texture &target = layer.getTexture();

    if (!target.getRect().padded(1).contains(pos)) {
        return;
    }

    getManager().setColor(Color{target.getPixelColor(pos)});
}
