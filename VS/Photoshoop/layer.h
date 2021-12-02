#pragma once

#include <AGF/llgui_pre.h>
#include <ACL/unique_ptr.h>
#include <ACL/vector.h>


using abel::gui::Rect;
using abel::gui::Color;
using abel::math::Vector2d;


class Layer {
public:
    explicit Layer(const Vector2d &size, const Color &defaultColor = Color::WHITE, double defaultAlpha = 1);

    const abel::gui::Texture &getTexture() const { return *texture; }
          abel::gui::Texture &getTexture()       { return *texture; }

    const abel::gui::Texture &getPreview() const { return *preview; }
          abel::gui::Texture &getPreview()       { return *preview; }

    void flushPreview();

    Vector2d getSize() const {
        return texture->getSize();
    }

protected:
    abel::unique_ptr<abel::gui::Texture> texture = nullptr;
    abel::unique_ptr<abel::gui::Texture> preview = nullptr;

};