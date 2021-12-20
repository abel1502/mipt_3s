#include <ACL/general.h>
#include <ACL/math/vector.h>
#include <ACL/gui/rect.h>
#include "../std/plugin_std.hpp"
#include <deque>


using abel::math::Vector2d;
using abel::math::Vector2i;
using abel::gui::Rect;


class Fill : public PUPPY::PluginInterface {
public:
    Fill() :
        PUPPY::PluginInterface() {};

    virtual bool ext_enable(const char *name) const override;

    virtual void *ext_get_func(const char *extension, const char *func) const override;

    virtual void *ext_get_interface(const char *extension, const char *name) const override;

    virtual const PUPPY::PluginInfo *get_info() const override;
    virtual PUPPY::Status init(const PUPPY::AppInterface*, const std::filesystem::path& path) override;
    virtual PUPPY::Status deinit() override;
    virtual void dump() const override;

    virtual void on_tick(double dt) const override;

    virtual void effect_apply() const override;

    virtual void tool_on_press(const PUPPY::Vec2f &position) const override;
    virtual void tool_on_release(const PUPPY::Vec2f &position) const override;
    virtual void tool_on_move(const PUPPY::Vec2f &from, const PUPPY::Vec2f &to) const override;

    virtual void show_settings() const override;

    inline static Fill &getInstance() {
        return instance;
    }

protected:
    static const PUPPY::AppInterface *appIntf;
    static Fill instance;

    static const PUPPY::PluginInfo info;


    static bool cmpColors(const PUPPY::RGBA &a, const PUPPY::RGBA &b);

    template <typename T = double, typename U>
    static abel::math::Vector2<T> cvtVec(const PUPPY::Vec2T<U> &vec) {
        return abel::math::Vector2<T>((T)vec.x, (T)vec.y);
    }

    template <typename T = float, typename U>
    static PUPPY::Vec2T<T> cvtVec(const abel::math::Vector2<U> &vec) {
        return PUPPY::Vec2T<T>((T)vec.x(), (T)vec.y());
    }

};

//===================================================================

const PUPPY::AppInterface *Fill::appIntf = nullptr;
Fill Fill::instance{};

const PUPPY::PluginInfo Fill::info = {
    PUPPY::STD_VERSION,     // std_version
    0,                      // reserved

    &Fill::getInstance(),   // plugin interface

    "Fill",
    "1.0",
    "abel1502",
    "A fill effect, implemented to test my plugin handling",

    nullptr,                // icon

    PUPPY::TOOL
};

//===================================================================

void (*release_pixels)(PUPPY::RGBA *);
void (*release_rt)(PUPPY::RenderTarget *);

extern "C" PLUGIN_GETINTF_EXPORT const PUPPY::PluginInterface *get_plugin_interface() {
    return &Fill::getInstance();
}

//===================================================================

bool Fill::ext_enable(const char *name) const {
    return false;
}

void *Fill::ext_get_func(const char *extension, const char *func) const {
    return nullptr;
}

void *Fill::ext_get_interface(const char *extension, const char *name) const {
    return nullptr;
}

const PUPPY::PluginInfo *Fill::get_info() const {
    return &info;
}

PUPPY::Status Fill::init(const PUPPY::AppInterface* appInterface, const std::filesystem::path& path) {
    appIntf = appInterface;

    if (!appIntf) {
        return PUPPY::ERR;
    }

    if (appIntf->ext_enable("free")) {
        release_pixels = (decltype(release_pixels))appIntf->ext_get_func("free", "release_pixels");
        release_rt     = (decltype(release_rt    ))appIntf->ext_get_func("free", "release_rt"    );
    }

    if (appIntf->factory.widget) {
        //appIntf->factory.widget->window()
    }

    appIntf->log("Fill inited successfully");

    return PUPPY::Status::OK;
}

PUPPY::Status Fill::deinit() {
    return PUPPY::Status::OK;
}

void Fill::dump() const {
    appIntf->log("Nothing to dump, tbh. How's your day?");
}

void Fill::on_tick(double dt) const {}

void Fill::effect_apply() const {}

void Fill::tool_on_press(const PUPPY::Vec2f &position) const {
    PUPPY::RenderTarget *activeLayer = appIntf->get_target();

    if (!activeLayer) {
        return;
    }

    Vector2i clickPos = cvtVec<int>(position);
    Vector2i layerSize = cvtVec<int>(activeLayer->get_size());

    if (clickPos.x() < 0 ||
        clickPos.y() < 0 ||
        clickPos.x() >= layerSize.x() ||
        clickPos.y() >= layerSize.y()) {

        return;
    }

    PUPPY::RGBA fillColor = appIntf->get_color();
    PUPPY::RGBA *pixels = activeLayer->get_pixels();
    PUPPY::RGBA oldColor = pixels[layerSize.x() * clickPos.y() + clickPos.x()];

    if (!cmpColors(fillColor, oldColor)) {
        std::deque<Vector2i> queue;
        queue.push_back(clickPos);

        while (!queue.empty()) {
            Vector2i curPos = queue.front();
            queue.pop_front();

            if (curPos.x() < 0 ||
                curPos.y() < 0 ||
                curPos.x() >= layerSize.x() ||
                curPos.y() >= layerSize.y()) {

                continue;
            }

            if (!cmpColors(pixels[layerSize.x() * curPos.y() + curPos.x()], oldColor))
                continue;

            constexpr Vector2i DIRS[4] = {
                Vector2i{ 1,  0},
                Vector2i{ 0,  1},
                Vector2i{-1,  0},
                Vector2i{ 0, -1},
            };

            for (const Vector2i &dir : DIRS) {
                queue.push_back(curPos + dir);
            }

            pixels[layerSize.x() * curPos.y() + curPos.x()] = fillColor;
        }

        PUPPY::RenderMode mode(PUPPY::BlendMode::COPY);
        activeLayer->render_pixels(PUPPY::Vec2f{}, cvtVec<size_t>(layerSize), pixels, mode);
    }

    if (release_pixels) {
        release_pixels(pixels);
    } else {
        delete[] pixels;
    }

    if (release_rt) {
        release_rt(activeLayer);
    } else {
        delete activeLayer;
    }
}

void Fill::tool_on_release(const PUPPY::Vec2f &position) const {}

void Fill::tool_on_move(const PUPPY::Vec2f &from, const PUPPY::Vec2f &to) const {
    tool_on_press(to);
}

void Fill::show_settings() const {}

bool Fill::cmpColors(const PUPPY::RGBA &a, const PUPPY::RGBA &b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}
