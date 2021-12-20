#pragma once

#ifndef PHOTOSHOOP_PLUGINS_IMPL_GUARD
#error "This file should not be included manually"
#endif


#if PHOTOSHOOP_PLUGINS_VER == 2
#include <AGF/llgui_pre.h>
#include <AGF/impl/windows.h>
#include <ACL/gui/rect.h>
#include <ACL/gui/color.h>
#include <ACL/math/vector.h>
#include <ACL/general.h>
#include <ACL/vector.h>
#include <ACL/unique_ptr.h>
#include <ACL/type_traits.h>
#include <filesystem>
#include <vector>
#include <map>


using abel::gui::Rect;
using abel::gui::Color;
using abel::math::Vector2d;


#include "v2/plugin_std.hpp"
#include "v2/lib_std/widget_std.h"

namespace std {
namespace fs = filesystem;
}


class Layer;

class Plugin;
class PluginMgr;

class ProxyRenderTarget;
class OwningRenderTarget;

class MyAppInterface;
class PluginExtension;

DECLARE_ERROR(notimpl_error, abel::error);


class ProxyRenderTarget final : public plugin::RenderTarget {
public:
    ProxyRenderTarget(abel::gui::Texture *target_);

    virtual RenderTarget *get_copy() const override;

    virtual plugin::Vec2s get_size() const override;

    virtual plugin::RGBA get_pixel(size_t x, size_t y) const override;

    virtual void set_pixel(size_t x, size_t y, const plugin::RGBA &color) override;

    virtual plugin::RGBA *get_pixels() const override;

    virtual void clear(const plugin::RGBA &color = 0) override;

    virtual void render_circle(const plugin::Vec2f &position, float radius, const plugin::RGBA &color, const plugin::RenderMode &render_mode = {}) override;

    virtual void render_line(const plugin::Vec2f &start, const plugin::Vec2f &end, const plugin::RGBA &color, const plugin::RenderMode &render_mode = {}) override;

    virtual void render_triangle(const plugin::Vec2f &p1, const plugin::Vec2f &p2, const plugin::Vec2f &p3, const plugin::RGBA &color, const plugin::RenderMode &render_mode = {}) override;

    virtual void render_rectangle(const plugin::Vec2f &p1, const plugin::Vec2f &p2, const plugin::RGBA &color, const plugin::RenderMode &render_mode = {}) override;

    virtual void render_texture(const plugin::Vec2f &position, const plugin::RenderTarget *texture, const plugin::RenderMode &render_mode = {}) override;

    virtual void render_pixels(const plugin::Vec2f &position, const plugin::Vec2s &size, const plugin::RGBA *data, const plugin::RenderMode &render_mode = {}) override;

    virtual void apply_shader(const plugin::Shader *shader) override;

    abel::gui::Texture *getRawTarget() {
        return target;
    }

    const abel::gui::Texture *getRawTarget() const {
        return target;
    }

protected:
    abel::gui::Texture *target;

};


class OwningRenderTarget final : public plugin::RenderTarget {
public:
    // Warning: TAKES OWNERSHIP OVER THE TARGET!
    OwningRenderTarget(abel::gui::Texture *target_);

    virtual plugin::RenderTarget *get_copy() const override;

    virtual plugin::Vec2s get_size() const override;

    virtual plugin::RGBA get_pixel(size_t x, size_t y) const override;

    virtual void set_pixel(size_t x, size_t y, const plugin::RGBA &color) override;

    virtual plugin::RGBA *get_pixels() const override;

    virtual void clear(const plugin::RGBA &color = 0) override;

    virtual void render_circle(const plugin::Vec2f &position, float radius, const plugin::RGBA &color, const plugin::RenderMode &render_mode = {}) override;

    virtual void render_line(const plugin::Vec2f &start, const plugin::Vec2f &end, const plugin::RGBA &color, const plugin::RenderMode &render_mode = {}) override;

    virtual void render_triangle(const plugin::Vec2f &p1, const plugin::Vec2f &p2, const plugin::Vec2f &p3, const plugin::RGBA &color, const plugin::RenderMode &render_mode = {}) override;

    virtual void render_rectangle(const plugin::Vec2f &p1, const plugin::Vec2f &p2, const plugin::RGBA &color, const plugin::RenderMode &render_mode = {}) override;

    virtual void render_texture(const plugin::Vec2f &position, const plugin::RenderTarget *texture, const plugin::RenderMode &render_mode = {}) override;

    virtual void render_pixels(const plugin::Vec2f &position, const plugin::Vec2s &size, const plugin::RGBA *data, const plugin::RenderMode &render_mode = {}) override;

    virtual void apply_shader(const plugin::Shader *shader) override;

    abel::gui::Texture *getRawTarget() {
        return proxy.getRawTarget();
    }

    const abel::gui::Texture *getRawTarget() const {
        return proxy.getRawTarget();
    }

protected:
    abel::unique_ptr<abel::gui::Texture> target;
    ProxyRenderTarget proxy;

};


abel::gui::Texture *extractTexture(plugin::RenderTarget &rt);

inline const abel::gui::Texture *extractTexture(const plugin::RenderTarget &rt) {
    return extractTexture(const_cast<plugin::RenderTarget &>(rt));
}


class MyAppInterface : public plugin::AppInterface {
public:
    static constexpr std::string_view EXT_ABEL = "abel";


    class MyWidgetFactory : public plugin::WidgetFactory {
    public:
        MyWidgetFactory(MyAppInterface &app_);

        virtual plugin::Button
            *button       (const plugin::WBody &body,
                           plugin::Widget *parent = nullptr) const override;

        virtual plugin::Button
            *button       (const plugin::Vec2f &pos,
                           const char *caption,
                           plugin::Widget *parent = nullptr) const override;

        virtual plugin::Slider
            *slider       (plugin::Slider::Type type,
                           const plugin::WBody &body,
                           plugin::Widget *parent = nullptr) const override;

        virtual plugin::TextField
            *text_field   (const plugin::WBody &body,
                           plugin::Widget *parent = nullptr) const override;

        virtual plugin::Window
            *window       (const char *name,
                           const plugin::WBody &body,
                           plugin::Widget *parent = nullptr) const override;

        virtual plugin::ColorPicker
            *color_picker (const plugin::WBody &body,
                           plugin::Widget *parent = nullptr) const override;

        virtual plugin::Label
            *label        (const plugin::Vec2f &pos,
                           const char *text,
                           plugin::Widget *parent = nullptr) const override;

        virtual plugin::Widget
            *abstract     (const plugin::WBody &body,
                           plugin::Widget *parent = nullptr) const override;

    protected:
        MyAppInterface &app;

    };

    class MyRenderTargetFactory : public plugin::RenderTargetFactory {
    public:
        MyRenderTargetFactory(MyAppInterface &app_);

        virtual plugin::RenderTarget *spawn(const plugin::Vec2s &size, const plugin::RGBA &color = {0, 0, 0, 255}) const override;
        virtual plugin::RenderTarget *from_pixels(const plugin::Vec2s &size, const plugin::RGBA *data) const override;
        virtual plugin::RenderTarget *from_file(const char *path) const override;

    protected:
        MyAppInterface &app;

    };


    inline MyAppInterface(Plugin &client_) :
        client{client_} {

        std_version = plugin::STD_VERSION;
        reserved = nullptr;
        feature_level = 0 /*| plugin::SETTINGS_SUPPORT*/;
        factory.widget = widgetFactory.get();
        factory.target = rtFactory.get();
        factory.shader = nullptr;
    }

    virtual bool ext_enable(const char *name) const override;

    virtual void *ext_get_func(const char *extension, const char *func) const override;

    virtual void *ext_get_interface(const char *extension, const char *name) const override;

    virtual void ext_register_as(const char *extension) const override;

    virtual void log(const char *fmt, ...) const override;
    virtual double get_absolute_time()     const override;

    virtual plugin::RGBA get_color() const override;
    virtual float        get_size () const override;

    virtual void set_color(const plugin::RGBA &color) const override;
    virtual void set_size(float size) const override;

    virtual const std::vector<plugin::WBody> get_windows() const override;
    virtual plugin::Widget *get_root_widget() const override;

    virtual plugin::RenderTarget *get_target()  const override;
    virtual plugin::RenderTarget *get_preview() const override;
    virtual void flush_preview()                const override;

protected:
    Plugin &client;
    abel::unique_ptr<MyWidgetFactory>   widgetFactory{new       MyWidgetFactory(*this)};
    abel::unique_ptr<MyRenderTargetFactory> rtFactory{new MyRenderTargetFactory(*this)};

};


class PluginExtension {
public:
    PluginExtension(const char *name_, Plugin &provider_);

    void *getFunc(const char *func) const;

    void *getInterface(const char *intf) const;

    inline const char *getCName() const {
        return name.data();
    }

    inline std::string_view getName() const {
        return name;
    }

protected:
    std::string name;
    Plugin &provider;

};


class Plugin {
public:
    DECLARE_ERROR(error, abel::error);


    inline Plugin(const std::fs::path &dllPath) {
        load(dllPath);
    }

    Plugin(const Plugin &other) = delete;
    Plugin &operator=(const Plugin &other) = delete;

    inline Plugin(Plugin &&other) noexcept;
    inline Plugin &operator=(Plugin &&other) noexcept;

    ~Plugin();

    static bool isPluginCandidateFile(const std::fs::path &path);

    const plugin::PluginInterface *getNativePlugin() const {
        return nativePlugin;
    }

    void onToolPress(const Vector2d &pos) const;
    void onToolRelease(const Vector2d &pos) const;
    void onToolMove(const Vector2d &pos0, const Vector2d &pos1) const;

    void onEffectApply() const;

    void applyFlushPolicy(Layer &layer) const;

    void deinit();

protected:
    HMODULE dllHandle = NULL;
    MyAppInterface nativeApp{*this};
    plugin::PluginInterface *nativePlugin = nullptr;
    const plugin::PluginInfo *info = nullptr;


    void load(const std::fs::path &dllPath);

    void init(const std::fs::path &dllPath);

};

#pragma region
inline Plugin::Plugin(Plugin &&other) noexcept :
    dllHandle{other.dllHandle},
    nativePlugin{nativePlugin},
    info{other.info} {

    other.dllHandle = NULL;
    other.nativePlugin = nullptr;
    other.info = nullptr;
}

inline Plugin &Plugin::operator=(Plugin &&other) noexcept {
    std::swap(dllHandle, other.dllHandle);
    std::swap(nativePlugin, other.nativePlugin);
    std::swap(info, other.info);

    return *this;
}
#pragma endregion


class PluginMgr {
public:
    static constexpr char PLUGINS_DIR[] = "plugins/";


    PluginMgr() = default;

    void loadAll();

    void load(const std::fs::path &dllPath);

    Plugin &find(plugin::PluginInterface *nativePlugin) {
        for (auto &plugin : plugins) {
            if (plugin->getNativePlugin() == nativePlugin) {
                return *plugin;
            }
        }

        throw Plugin::error("Plugin not found");
    }

    inline const Plugin &find(const plugin::PluginInterface *nativePlugin) const {
        return const_cast<PluginMgr *>(this)->find(nativePlugin);
    }

    void deinitAll();

    bool hasExt(const char *name) const;

    PluginExtension &getExt(const char *name);

    // Returns true on success
    bool registerExt(PluginExtension *ext);

    inline const PluginExtension &getExt(const char *name) const {
        return const_cast<PluginMgr *>(this)->getExt(name);
    }

protected:
    abel::vector<abel::unique_ptr<Plugin>> plugins{};
    std::map<std::string_view, abel::unique_ptr<PluginExtension>> extensions{};

};


#endif