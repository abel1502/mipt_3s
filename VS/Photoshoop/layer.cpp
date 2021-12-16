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
    if constexpr (DEBUG_PREVIEW) {
        DBG("[%2u] Flush preview", (uintptr_t)this % 17);
    }

    getTexture().setOverwrite(flushPolicyOverwrite);
    getTexture().embed(getTexture().getRect(), getPreview());
    getTexture().resetOverwrite();
    clearPreview();
}


void Layer::clearPreview() {
    if constexpr (DEBUG_PREVIEW) {
        DBG("[%2u] Clear preview", (uintptr_t)this % 17);
    }

    getPreview().setFillColor(Color::WHITE, 0);
    getPreview().clear();
}