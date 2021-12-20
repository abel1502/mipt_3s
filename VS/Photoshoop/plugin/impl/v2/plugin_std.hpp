#pragma once

#pragma push_macro("interface")
#undef interface

#include <string_view>
#include <filesystem>

#include "lib_std/types_std.h"
#include "lib_std/widgets/collection.h"
#include "lib_std/extensioner_std.h"


namespace plugin {

constexpr uint32_t STD_VERSION = 0x00010000;

constexpr char EXT_STD[] = "std";

// this function is only defined in plugin. call it to get PluginInterface to interact with plugin
// make sure you wrap it into extern C section to avoid mangling
// const PluginInterface *get_plugin_interface();

struct PluginInterface;

constexpr char GET_INTERFACE_FUNC[] = "get_plugin_interface";
typedef PluginInterface* (*PluginGetInterfaceType)();
#ifdef _WIN32
#define PLUGIN_GETINTF_EXPORT __declspec(dllexport)
#else
#define PLUGIN_GETINTF_EXPORT
#endif

// ============================================================================


class RenderTarget {
public:
    virtual ~RenderTarget() {}

    virtual RenderTarget *get_copy() const = 0;

    virtual Vec2s get_size() const = 0;

    virtual RGBA get_pixel(size_t x, size_t y) const = 0;
    virtual void set_pixel(size_t x, size_t y, const RGBA &color) = 0;

    virtual RGBA *get_pixels() const = 0;

    virtual void clear(const RGBA &color = 0) = 0; // fills the target with `color`

// render
    virtual void render_circle(const Vec2f &position, float radius, const RGBA &color, const RenderMode &render_mode = {}) = 0;
    virtual void render_line(const Vec2f &start, const Vec2f &end, const RGBA &color, const RenderMode &render_mode = {}) = 0;
    virtual void render_triangle(const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, const RGBA &color, const RenderMode &render_mode = {}) = 0;
    virtual void render_rectangle(const Vec2f &p1, const Vec2f &p2, const RGBA &color, const RenderMode &render_mode = {}) = 0;

    virtual void render_texture(const Vec2f &position, const RenderTarget *texture, const RenderMode &render_mode = {}) = 0;
    virtual void render_pixels(const Vec2f &position, const Vec2s &size, const RGBA *data, const RenderMode &render_mode = {}) = 0;

    virtual void apply_shader(const Shader *shader) = 0;
};


struct AppInterface;
struct PluginInterface;

struct PluginInfo {
    const uint32_t std_version;
    void *reserved;

    const PluginInterface *const interface;

    const char *const name;
    const char *const version;
    const char *const author;
    const char *const description;
    const RenderTarget *icon;

    const PluginType type;
};

struct PluginInterface {
    PluginInterface(uint32_t std_version = STD_VERSION, void* reserved = nullptr) :
        std_version(std_version), reserved(reserved) {}

    uint32_t std_version;
    void *reserved;

    // enables specified extension
    virtual bool ext_enable(const char *name) const = 0;

    // returns given function, if it is implemented in the specified (enabled) extension
    virtual void *ext_get_func(const char *extension, const char *func) const = 0;

    // returns given interface, if it is implemented in the specified (enabled) extension
    virtual void *ext_get_interface(const char *extension, const char *name) const = 0;

    virtual const  PluginInfo *get_info()    const = 0;
    virtual Status init(const AppInterface*,
                        const std::filesystem::path& path = std::filesystem::path("./"))
                                                   = 0;
    virtual Status deinit()                        = 0;
    virtual void   dump()                    const = 0;

    virtual void on_tick(double dt) const = 0;

    virtual void effect_apply() const = 0;

    virtual void tool_on_press  (const Vec2f &position)              const = 0;
    virtual void tool_on_release(const Vec2f &position)              const = 0;
    virtual void tool_on_move   (const Vec2f &from, const Vec2f &to) const = 0;

    virtual void show_settings() const = 0;
};

struct WidgetFactory {
    virtual ~WidgetFactory() {}

    virtual Button      *button       (const WBody &body, Widget *parent = nullptr) const = 0;
    virtual Button      *button       (const Vec2f &pos, const char *caption, Widget *parent = nullptr) const = 0; // button fit to contain caption
    virtual Slider      *slider       (Slider::Type type, const WBody &body, Widget *parent = nullptr) const = 0;
    virtual TextField   *text_field   (const WBody &body, Widget *parent = nullptr) const = 0;
    virtual Window      *window       (const char *name, const WBody &body, Widget *parent = nullptr) const = 0;
    virtual ColorPicker *color_picker (const WBody &body, Widget *parent = nullptr) const = 0;
    virtual Label       *label        (const Vec2f &pos, const char *text, Widget *parent = nullptr) const = 0;
    virtual Widget      *abstract     (const WBody &body, Widget *parent = nullptr) const = 0;
};

struct ShaderFactory {
    virtual ~ShaderFactory() {}

    virtual Shader *compile (const char *code, ShaderType type, bool is_code = true) const = 0;
};

struct RenderTargetFactory {
    virtual ~RenderTargetFactory() {}

    virtual RenderTarget *spawn(const Vec2s &size, const RGBA &color = {0, 0, 0, 255}) const = 0; // color -> fill with it
    virtual RenderTarget *from_pixels(const Vec2s &size, const RGBA *data) const = 0;
    virtual RenderTarget *from_file(const char *path) const = 0;
};

struct AppInterface {
    uint32_t std_version;
    void *reserved;

    int feature_level;

    struct {
        WidgetFactory       *widget;
        ShaderFactory       *shader;
        RenderTargetFactory *target;
    } factory;

// extension
    // enables specified extension
    virtual bool ext_enable(const char *name) const = 0;

    // returns given function, if it is implemented in the specified (enabled) extension
    virtual void *ext_get_func(const char *extension, const char *func) const = 0;

    // returns given interface, if it is implemented in the specified (enabled) extension
    virtual void *ext_get_interface(const char *extension, const char *name) const = 0;

    // registers plugin as the holder of functions and interfaces for an extension
    virtual void ext_register_as(const char *extension) const = 0;

// general
    virtual void log(const char *fmt, ...) const = 0;
    virtual double get_absolute_time()     const = 0;

    virtual RGBA get_color() const = 0;
    virtual float get_size() const = 0;

    virtual void set_color(const RGBA &color) const = 0;
    virtual void set_size(float size) const = 0;

    virtual const std::vector<WBody> get_windows() const = 0;
    virtual Widget *get_root_widget() const = 0;

// target
    virtual RenderTarget *get_target()  const = 0; // returns actual active  layer, drawing in it changes app's layer
    virtual RenderTarget *get_preview() const = 0; // returns actual preview layer, drawing in it changes app's layer
    virtual void flush_preview()        const = 0;

    virtual ~AppInterface() {}
};

}

#pragma pop_macro("interface")
