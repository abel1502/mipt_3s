#pragma once

#include <AGF/llgui_pre.h>
#include <ACL/unique_ptr.h>
#include <ACL/vector.h>


using abel::gui::Rect;
using abel::gui::Color;
using abel::math::Vector2d;


class Layer {
public:
    explicit Layer(const Vector2d &size, const Color &defaultColor = Color::WHITE, double defaultAlpha = 0);

    const abel::gui::Texture &getTexture() const { return *texture; }
          abel::gui::Texture &getTexture()       { return *texture; }

    const abel::gui::Texture &getPreview() const { return *preview; }
          abel::gui::Texture &getPreview()       { return *preview; }

    void flushPreview();

    void clearPreview();

    constexpr void setFlushPolicy(bool overwrite) {
        flushPolicyOverwrite = overwrite;
    }

    constexpr bool willOverwrite() const {
        return flushPolicyOverwrite;
    }

    inline void beginPreview();

    inline void endPreview(bool apply = false);

    // TODO: Finish

    Vector2d getSize() const {
        return texture->getSize();
    }

protected:
    abel::unique_ptr<abel::gui::Texture> texture = nullptr;
    abel::unique_ptr<abel::gui::Texture> preview = nullptr;
    bool flushPolicyOverwrite = false;


    static constexpr bool DEBUG_PREVIEW = false;

};


inline void Layer::beginPreview() {
    if constexpr (DEBUG_PREVIEW) {
        DBG("[%2u] Begin preview", (uintptr_t)this % 17);
    }

    clearPreview();
    flushPolicyOverwrite = false;
}

inline void Layer::endPreview(bool apply) {
    if constexpr (DEBUG_PREVIEW) {
        DBG("[%2u] End preview, %s", (uintptr_t)this % 17, apply ? "applying" : "discarding");
    }

    if (apply) {
        flushPreview();
    } else {
        clearPreview();
    }

    flushPolicyOverwrite = false;
}
