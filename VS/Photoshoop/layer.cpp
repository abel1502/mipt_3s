#include <AGF/llgui.h>
#include "layer.h"


Layer::Layer(const Vector2d &size, const Color &defaultColor, double defaultAlpha) :
    texture{new abel::gui::Texture(size)},
    preview{new abel::gui::Texture(size)} {

    getTexture().setFillColor(defaultColor, (float)defaultAlpha);
    getTexture().clear();

    clearPreview();
}


void Layer::flushPreview() {
    getTexture().embed(getTexture().getRect(), getPreview());
    getTexture().resetOverwrite();
    clearPreview();
}


void Layer::clearPreview() {
    getPreview().setFillColor(Color::WHITE, 0);
    getPreview().clear();
}