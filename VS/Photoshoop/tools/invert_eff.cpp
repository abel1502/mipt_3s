#include <AGF/llgui.h>
#include "invert_eff.h"


void InvertEffect::apply(Layer &layer) {
    abel::gui::Texture &target = layer.getTexture();

    target.embedTransformed(target.getRect(), target, {-1, -1, -1, 1});
}