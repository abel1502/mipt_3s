#include <AGF/llgui.h>
#include "layer.h"


Layer::Layer(const Vector2d &size, const Color &defaultColor, double defaultAlpha) :
    texture{new abel::gui::Texture(size)},
    preview{new abel::gui::Texture(size)} {

    getTexture().setFillColor(defaultColor, defaultAlpha);
    getTexture().clear();

    getPreview().setFillColor(Color::WHITE, 0);
    getPreview().clear();
}


void Layer::flushPreview() {
    getTexture().embed(getTexture().getRect(), getPreview());

    getPreview().setFillColor(Color::WHITE, 0);
    getPreview().clear();
}