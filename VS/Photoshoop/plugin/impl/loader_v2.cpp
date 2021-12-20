#define JUST_DEFINE_PLUGINS_VER
#include "../loader.h"


#if PHOTOSHOOP_PLUGINS_VER == 2
#include <AGF/llgui.h>
#include "../loader.h"
#include "../../canvas.h"
#include "plugin_tool.h"
#include "../../layer.h"
#include "../../app.h"
#include <string_view>
#include <cstdint>
#include <cstdarg>
#include <cstring>
#include <string>

#undef interface

#define PHOTOSHOOP_PLUGINS_WIDGETS_GUARD
#include "v2/widgets.h"
#undef PHOTOSHOOP_PLUGINS_WIDGETS_GUARD


static_assert(sizeof(PluginMgr));


namespace {

using abel::gui::Rect;
using abel::gui::Color;
using abel::math::Vector2d;

#pragma warning (suppress : 4455)
using std::literals::operator ""sv;

}


#pragma region OwningRenderTarget
OwningRenderTarget::OwningRenderTarget(abel::gui::Texture *target_) :
    target{target_}, proxy{target_} {

    assert(target);
}

plugin::RenderTarget *OwningRenderTarget::get_copy() const {
    return proxy.get_copy();
}

plugin::Vec2s OwningRenderTarget::get_size() const {
    return proxy.get_size();
}

plugin::RGBA OwningRenderTarget::get_pixel(size_t x, size_t y) const {
    return proxy.get_pixel(x, y);
}

void OwningRenderTarget::set_pixel(size_t x, size_t y, const plugin::RGBA &color) {
    return proxy.set_pixel(x, y, color);
}

plugin::RGBA *OwningRenderTarget::get_pixels() const {
    return proxy.get_pixels();
}

void OwningRenderTarget::clear(const plugin::RGBA &color) {
    return proxy.clear();
}

void OwningRenderTarget::render_circle(const plugin::Vec2f &position,
                                       float radius,
                                       const plugin::RGBA &color,
                                       const plugin::RenderMode &render_mode) {
    return proxy.render_circle(position, radius, color, render_mode);
}

void OwningRenderTarget::render_line(const plugin::Vec2f &start,
                                     const plugin::Vec2f &end,
                                     const plugin::RGBA &color,
                                     const plugin::RenderMode &render_mode) {
    return proxy.render_line(start, end, color, render_mode);
}

void OwningRenderTarget::render_triangle(const plugin::Vec2f &p1,
                                         const plugin::Vec2f &p2,
                                         const plugin::Vec2f &p3,
                                         const plugin::RGBA &color,
                                         const plugin::RenderMode &render_mode) {
    return proxy.render_triangle(p1, p2, p3, color, render_mode);
}

void OwningRenderTarget::render_rectangle(const plugin::Vec2f &p1,
                                          const plugin::Vec2f &p2,
                                          const plugin::RGBA &color,
                                          const plugin::RenderMode &render_mode) {
    return proxy.render_rectangle(p1, p2, color, render_mode);
}

void OwningRenderTarget::render_texture(const plugin::Vec2f &position,
                                        const plugin::RenderTarget *texture,
                                        const plugin::RenderMode &render_mode) {
    return proxy.render_texture(position, texture, render_mode);
}

void OwningRenderTarget::render_pixels(const plugin::Vec2f &position,
                                       const plugin::Vec2s &size,
                                       const plugin::RGBA *data,
                                       const plugin::RenderMode &render_mode) {
    return proxy.render_pixels(position, size, data, render_mode);
}

void OwningRenderTarget::apply_shader(const plugin::Shader *shader) {
    return proxy.apply_shader(shader);
}
#pragma endregion OwningRenderTarget


#pragma region ProxyRenderTarget
ProxyRenderTarget::ProxyRenderTarget(abel::gui::Texture *target_) :
    target{target_} {

    assert(target);
}

plugin::RenderTarget *ProxyRenderTarget::get_copy() const {
    abel::gui::Texture *targetCopy = new abel::gui::Texture(target->getSize());

    targetCopy->embed(targetCopy->getRect(), *target);

    return new OwningRenderTarget(targetCopy);
}

plugin::Vec2s ProxyRenderTarget::get_size() const {
    return target->getSize();
}

plugin::RGBA ProxyRenderTarget::get_pixel(size_t x, size_t y) const {
    return target->getPixelColor(Vector2d{(double)x, (double)y});
}

void ProxyRenderTarget::set_pixel(size_t x, size_t y, const plugin::RGBA &color) {
    target->setPixelColor(Vector2d{(double)x, (double)y}, color);
}

plugin::RGBA *ProxyRenderTarget::get_pixels() const {
    static_assert(sizeof(plugin::RGBA) == sizeof(uint32_t));

    abel::gui::Texture &texture = *target;

    uint32_t *buf = texture.getBufRGBA(true, false);
    size_t size = (size_t)texture.width() * (size_t)texture.height();
    uint32_t *copy = new uint32_t[size];
    if (!copy) {
        texture.flushBuf();

        return nullptr;
    }

    memcpy(copy, buf, size * sizeof(uint32_t));

    texture.flushBuf();

    return (plugin::RGBA *)copy;
}

namespace {
void applyColor(abel::gui::Texture &target, const plugin::RGBA &color) {
    Color myColor{abel::gui::PackedColor{color}};
    float alpha = (float)color.a / 255.f;

    target.setFillColor(myColor, alpha);
    target.setLineColor(myColor, alpha);
}
}

void ProxyRenderTarget::clear(const plugin::RGBA &color) {
    applyColor(*target, color);
    target->clear();
}

#define BODY_RENDER_(...) {                                                 \
    applyColor(*target, color);                                             \
                                                                            \
    if (render_mode.blend == plugin::COPY) {                                \
        target->setOverwrite();                                             \
    }                                                                       \
                                                                            \
    __VA_ARGS__;                                                            \
                                                                            \
    target->resetOverwrite();                                               \
}

void ProxyRenderTarget::render_circle(const plugin::Vec2f &position,
                                      float radius,
                                      const plugin::RGBA &color,
                                      const plugin::RenderMode &render_mode) {
    BODY_RENDER_(
        target->drawCircle(abel::math::Vector2d{position},
                           (double)radius);
    );
}

void ProxyRenderTarget::render_line(const plugin::Vec2f &start,
                                    const plugin::Vec2f &end,
                                    const plugin::RGBA &color,
                                    const plugin::RenderMode &render_mode) {
    BODY_RENDER_(
        target->drawLine(abel::math::Vector2d{start},
                         abel::math::Vector2d{end});
    );
}

void ProxyRenderTarget::render_triangle(const plugin::Vec2f &p1,
                                        const plugin::Vec2f &p2,
                                        const plugin::Vec2f &p3,
                                        const plugin::RGBA &color,
                                        const plugin::RenderMode &render_mode) {
    BODY_RENDER_(
        target->drawTriangle(abel::math::Vector2d{p1},
                             abel::math::Vector2d{p2},
                             abel::math::Vector2d{p3});
    );
}

void ProxyRenderTarget::render_rectangle(const plugin::Vec2f &p1,
                                         const plugin::Vec2f &p2,
                                         const plugin::RGBA &color,
                                         const plugin::RenderMode &render_mode) {
    BODY_RENDER_(
        target->drawRect(Rect<double>::se(abel::math::Vector2d{p1},
                                          abel::math::Vector2d{p2}));
    );
}

void ProxyRenderTarget::render_texture(const plugin::Vec2f &position,
                                       const plugin::RenderTarget *texture,
                                       const plugin::RenderMode &render_mode) {
    const abel::gui::Texture *source = extractTexture(*texture);

    assert(source);

    if (render_mode.blend == plugin::COPY) {
        target->setOverwrite();
    }

    target->embed(Rect<double>::wh(position, source->getSize()), *source);

    target->resetOverwrite();
}

void ProxyRenderTarget::render_pixels(const plugin::Vec2f &position,
                                      const plugin::Vec2s &size,
                                      const plugin::RGBA *data,
                                      const plugin::RenderMode &render_mode) {
    static_assert(sizeof(uint32_t) == sizeof(plugin::RGBA));

    if (render_mode.blend == plugin::COPY) {
        target->setOverwrite();
    }

    abel::gui::Texture tmp{(unsigned)size.x, (unsigned)size.y};
    memcpy(tmp.getBufRGBA(false, true), data, size.x * size.y * sizeof(plugin::RGBA));
    tmp.flushBuf();

    target->embed(Rect<double>::wh(Vector2d{position},
                                   Vector2d{(double)size.x, (double)size.y}), tmp);

    target->resetOverwrite();
}
#undef BODY_RENDER_

void ProxyRenderTarget::apply_shader(const plugin::Shader */*shader*/) {
    // Not doing anything, heh
}
#pragma endregion ProxyRenderTarget


#pragma region extractTexture
abel::gui::Texture *extractTexture(plugin::RenderTarget &rt) {
     ProxyRenderTarget *prt = dynamic_cast< ProxyRenderTarget *>(&rt);
    OwningRenderTarget *ort = dynamic_cast<OwningRenderTarget *>(&rt);
    if (prt) {
        return prt->getRawTarget();
    } else if (ort) {
        return ort->getRawTarget();
    }

    ERR("Bad RenderTarget");
    assert(false);
    return nullptr;
}
#pragma endregion extractTexture


#pragma region MyAppInterface
bool MyAppInterface::ext_enable(const char *name) const {
    if (name == EXT_ABEL) {
        return true;
    }

    if (MyApp::getInstance().pluginMgr.hasExt(name)) {
        return true;
    }

    return false;
}

namespace {
uint32_t ext_abel_get_magic() {
    return 'ABEL';
}

uint32_t ext_abel_get_version() {
    return 0;
}

void *dispatchAbelExt(const char *name, bool isInterface) {
    // TODO: Stuff :)

    if (!isInterface && name == "get_magic"sv) {
        return &ext_abel_get_magic;
    }

    if (!isInterface && name == "get_version"sv) {
        return &ext_abel_get_version;
    }

    return nullptr;
}
}

void *MyAppInterface::ext_get_func(const char *extension, const char *func) const {
    if (extension == EXT_ABEL) {
        return dispatchAbelExt(func, false);
    }

    PluginMgr &pluginMgr = MyApp::getInstance().pluginMgr;

    if (!pluginMgr.hasExt(extension)) {
        return nullptr;
    }

    return pluginMgr.getExt(extension).getFunc(func);
}

void *MyAppInterface::ext_get_interface(const char *extension, const char *name) const {
    if (extension == EXT_ABEL) {
        return dispatchAbelExt(name, true);
    }

    PluginMgr &pluginMgr = MyApp::getInstance().pluginMgr;

    if (!pluginMgr.hasExt(extension)) {
        return nullptr;
    }

    return pluginMgr.getExt(extension).getInterface(name);
}

void MyAppInterface::ext_register_as(const char *extension) const {
    if (extension == EXT_ABEL) {
        ERR("Such audacity!");
        return;
    }

    PluginMgr &pluginMgr = MyApp::getInstance().pluginMgr;

    bool success = pluginMgr.registerExt(new PluginExtension{extension, client});

    if (!success) {
        ERR("Tough luck, bleh");
        return;
    }
}

void MyAppInterface::log(const char *fmt, ...) const {
    va_list args{};

    va_start(args, fmt);
    int requiredLen = vsnprintf(nullptr, 0, fmt, args);
    va_end(args);

    if (requiredLen < 0) {
        ERR("vsnprintf failed while logging plugin message");
        return;
    }

    std::string msg{};
    msg.resize(requiredLen + 1);
    va_start(args, fmt);
    vsnprintf(msg.data(), requiredLen + 1, fmt, args);
    va_end(args);

    DBG("[PLUGIN] %s", msg.data());
}

double MyAppInterface::get_absolute_time() const {
    return MyApp::getInstance().getTime();
}

namespace {
plugin::RGBA convertColor(const abel::gui::PackedColor &color, unsigned char alpha) {
    return plugin::RGBA{color.R, color.G, color.B, alpha};
}

plugin::RGBA convertColor(const Color &color, float alpha) {
    return convertColor(color.pack(), (unsigned char)(alpha * 255.f));
}
}

plugin::RGBA MyAppInterface::get_color() const {
    const auto &toolMgr = MyApp::getInstance().toolMgr;

    return convertColor(toolMgr.getColor(), (float)toolMgr.getAlpha());
}

float MyAppInterface::get_size() const {
    return (float)MyApp::getInstance().toolMgr.getRadius();
}

namespace {
void applyColor(const plugin::RGBA &color) {
    Color myColor{abel::gui::PackedColor{color}};
    float alpha = (float)color.a / 255.f;

    auto &toolMgr = MyApp::getInstance().toolMgr;

    toolMgr.setColor(myColor);
    toolMgr.setAlpha(alpha);
}
}

void MyAppInterface::set_color(const plugin::RGBA &color) const {
    applyColor(color);
}

void MyAppInterface::set_size(float size) const {
    MyApp::getInstance().toolMgr.setRadius((double)size);
}

const std::vector<plugin::WBody> MyAppInterface::get_windows() const {
    return std::vector<plugin::WBody>();
}

plugin::Widget *MyAppInterface::get_root_widget() const {
    // TODO: Implement
    return nullptr;
}

plugin::RenderTarget *MyAppInterface::get_target() const {
    Canvas *activeCanvas = MyApp::getInstance().toolMgr.getActiveCanvas();

    if (!activeCanvas) {
        return nullptr;
    }

    return new ProxyRenderTarget(&activeCanvas->activeLayer().getTexture());
}

plugin::RenderTarget *MyAppInterface::get_preview() const {
    Canvas *activeCanvas = MyApp::getInstance().toolMgr.getActiveCanvas();

    if (!activeCanvas) {
        return nullptr;
    }

    return new ProxyRenderTarget(&activeCanvas->activeLayer().getPreview());
}

void MyAppInterface::flush_preview() const {
    Canvas *activeCanvas = MyApp::getInstance().toolMgr.getActiveCanvas();

    if (!activeCanvas) {
        return;
    }

    // TODO: ?
    activeCanvas->activeLayer().flushPreview();
}
#pragma endregion MyAppInterface


#pragma region MyWF
using MyWF = MyAppInterface::MyWidgetFactory;

MyWF::MyWidgetFactory(MyAppInterface &app_) :
    app{app_} {}

plugin::Button *MyWF::button(const plugin::WBody &body,
                             plugin::Widget *parent) const {

    PluginProxyWidget *ppw = new PluginProxyWidget(nullptr, body, nullptr, true);

    MyPButton *result = new MyPButton(ppw, "");

    if (parent) {
        parent->add_child(result);
    }

    return result;
}

plugin::Button *MyWF::button(const plugin::Vec2f &pos,
                             const char *caption,
                             plugin::Widget *parent) const {
    Vector2d size{};

    {
        abel::gui::Texture tmp{Vector2d{1, 1}};
        size = tmp.getTextSize(caption);
    }

    size *= 1.1;

    plugin::Button *result = button(Rect<double>::wh(pos, size), parent);
    if (!result) {
        return result;
    }

    result->set_caption(caption, 14);

    return result;
}

plugin::Slider *MyWF::slider(plugin::Slider::Type type,
                             const plugin::WBody &body,
                             plugin::Widget *parent) const {

    PluginProxyWidget *ppw = new PluginProxyWidget(nullptr, body, nullptr, true);

    MyPSlider *result = new MyPSlider(ppw, type);

    if (parent) {
        parent->add_child(result);
    }

    return result;
}

plugin::TextField *MyWF::text_field(const plugin::WBody &body,
                                    plugin::Widget *parent) const {
    return nullptr;
}

plugin::Window *MyWF::window(const char *name,
                             const plugin::WBody &body,
                             plugin::Widget *parent) const {

    PluginProxyWidget *ppw = new PluginProxyWidget(nullptr, body, nullptr, true);

    MyPWindow *result = new MyPWindow(ppw, name);

    MyApp::getInstance().getWindowMgrWidget().createWindow(body, ppw);

    return result;
}

plugin::ColorPicker *MyWF::color_picker(const plugin::WBody &body,
                                        plugin::Widget *parent) const {

    PluginProxyWidget *ppw = new PluginProxyWidget(nullptr, body, nullptr, true);

    MyPColorPicker *result = new MyPColorPicker(ppw);

    if (parent) {
        parent->add_child(result);
    }

    return result;
}

plugin::Label *MyWF::label(const plugin::Vec2f &pos,
                           const char *text,
                           plugin::Widget *parent) const {
    Vector2d size{};

    {
        abel::gui::Texture tmp{Vector2d{1, 1}};
        size = tmp.getTextSize(text);
    }

    size *= 1.1;

    PluginProxyWidget *ppw = new PluginProxyWidget(nullptr, Rect<double>::wh(pos, size), nullptr, true);

    MyPLabel *result = new MyPLabel(ppw, text);

    if (parent) {
        parent->add_child(result);
    }

    return result;
}

plugin::Widget *MyWF::abstract(const plugin::WBody &body,
                               plugin::Widget *parent) const {
    PluginProxyWidget *ppw = new PluginProxyWidget(nullptr, body, nullptr, true);

    MyPWidget *result = new MyPWidget(ppw);

    if (parent) {
        parent->add_child(result);
    }

    return result;
}
#pragma endregion MyWF


#pragma region MyRTF
using MyRTF = MyAppInterface::MyRenderTargetFactory;

MyRTF::MyRenderTargetFactory(MyAppInterface &app_) :
    app{app_} {}

plugin::RenderTarget *MyRTF::spawn(const plugin::Vec2s &size,
                            const plugin::RGBA &color) const {
    abel::gui::Texture *target = nullptr;

    try {
        target = new abel::gui::Texture(Vector2d{size});

        target->setFillColor(Color{abel::gui::PackedColor{color}}, (float)color.a / 255.f);
        target->clear();
    } catch (abel::gui::llgui_error) {
        return nullptr;
    } catch (abel::require_error) {
        return nullptr;
    }

    assert(target);

    return new OwningRenderTarget(target);
}

plugin::RenderTarget *MyRTF::from_pixels(const plugin::Vec2s &size,
                                         const plugin::RGBA *data) const {
    abel::gui::Texture *target = nullptr;

    try {
        target = new abel::gui::Texture(Vector2d{size});

        memcpy(target->getBufRGBA(false, true), data, size.x * size.y * sizeof(plugin::RGBA));
        target->flushBuf();
    } catch (abel::gui::llgui_error) {
        return nullptr;
    } catch (abel::require_error) {
        return nullptr;
    }

    assert(target);

    return new OwningRenderTarget(target);
}

plugin::RenderTarget *MyRTF::from_file(const char *path) const {
    abel::gui::Texture *target = nullptr;

    try {
        target = new abel::gui::Texture(path);
    } catch (abel::gui::llgui_error) {
        return nullptr;
    } catch (abel::require_error) {
        return nullptr;
    }

    assert(target);

    return new OwningRenderTarget(target);
}
#pragma endregion MyRTF


#pragma region PluginExtension
PluginExtension::PluginExtension(const char *name_, Plugin &provider_) :
    name{name_}, provider{provider_} {}

void *PluginExtension::getFunc(const char *func) const {
    return provider.getNativePlugin()->ext_get_func(name.data(), func);
}

void *PluginExtension::getInterface(const char *intf) const {
    return provider.getNativePlugin()->ext_get_interface(name.data(), intf);
}
#pragma endregion PluginExtension


#pragma region Plugin
using gpi_func_ptr_t = plugin::PluginInterface *(FAR WINAPI *)();


bool Plugin::isPluginCandidateFile(const std::fs::path &path) {
    return std::fs::is_regular_file(path) &&
           path.extension().compare(".dll") == 0;
}

void Plugin::load(const std::fs::path &dllPath) {
    #define BAD_PLUGIN_() \
        ERR("Bad plugin: \"%ls\"", dllPath.c_str())

    if (!isPluginCandidateFile(dllPath)) {
        BAD_PLUGIN_();
        throw error("The specified path is not a dll");
    }

    dllHandle = LoadLibraryExW(dllPath.c_str(), NULL, LOAD_LIBRARY_SEARCH_APPLICATION_DIR);
    if (!dllHandle) {
        BAD_PLUGIN_();
        throw error("LoadLibrary failed");
    }

    gpi_func_ptr_t get_plugin_interface =
        (gpi_func_ptr_t)GetProcAddress(dllHandle, plugin::GET_INTERFACE_FUNC);

    if (!get_plugin_interface) {
        BAD_PLUGIN_();
        throw error("The plugin dll doesn't provide a get_plugin_interface() function");
    }

    nativePlugin = get_plugin_interface();

    if (!nativePlugin) {
        BAD_PLUGIN_();
        throw error("get_plugin_interface() returned nullptr");
    }

    if (nativePlugin->std_version != plugin::STD_VERSION) {
        BAD_PLUGIN_();
        ERR("Expected version 0x%8x, got 0x%8x", plugin::STD_VERSION, nativePlugin->std_version);
        throw error("Plugin of unsupported version");
    }

    DBG("Successfully loaded plugin from \"%ls\" with handle 0x%p. Initializing...", dllPath.c_str(), nativePlugin);

    init(dllPath);

    #undef BAD_PLUGIN_
}

Plugin::~Plugin() {
    if (dllHandle) {
        if (!FreeLibrary(dllHandle)) {
            ERR("FreeLibrary failed. Ignoring");
        }

        dllHandle = NULL;
    }

    if (nativePlugin) {
        // Supposedly, called manually
        // deinit();

        nativePlugin = nullptr;
    }
}

void Plugin::init(const std::fs::path &dllPath) {
    assert(nativePlugin);
    assert(nativePlugin->general.init);

    plugin::Status status = nativePlugin->init(&nativeApp, dllPath);

    if (status != plugin::OK) {
        throw error("Plugin failed to initialize itself");
    }

    info = nativePlugin->get_info();
    REQUIRE(info);
    REQUIRE(info->interface == nativePlugin);
    REQUIRE(info->std_version == nativePlugin->std_version);

    MyApp &app = MyApp::getInstance();

    app.sigTick += [nativePlugin = nativePlugin](double deltaT) {
        nativePlugin->on_tick(deltaT);

        return false;
    };

    switch (info->type) {
    case plugin::TOOL: {
        app.getToolsWidget().addToolButton(
            app.toolMgr.addTool(new PluginTool(&app.toolMgr, *this)),
            info->name
        );
    } break;

    case plugin::EFFECT: {
        app.getToolsWidget().addEffectButton(
            app.toolMgr.addEffect(new PluginEffect(&app.toolMgr, *this)),
            info->name
        );
    } break;

    NODEFAULT
    }

    DBG("Successfully initialized plugin '%s' (%s) by '%s'.\n"
        "Brief description:\n"
        "\"%s\"",
        info->name,
        info->type == plugin::TOOL ? "tool" : "effect",
        info->author,
        info->description);
}

void Plugin::applyFlushPolicy(Layer &layer) const {
    layer.setFlushPolicy(false);
}

void Plugin::deinit() {
    if (!nativePlugin) {
        return;
    }

    nativePlugin->deinit();
}

void Plugin::onToolPress(const Vector2d &pos) const {
    assert(info->type == plugin::TOOL);

    nativePlugin->tool_on_press(pos);
}

void Plugin::onToolRelease(const Vector2d &pos) const {
    assert(info->type == plugin::TOOL);

    nativePlugin->tool_on_release(pos);
}

void Plugin::onToolMove(const Vector2d &pos0, const Vector2d &pos1) const {
    assert(info->type == plugin::PT_TOOL);

    nativePlugin->tool_on_move(pos0, pos1);
}

void Plugin::onEffectApply() const {
    assert(info->type == plugin::PT_EFFECT);

    Canvas *activeCanvas = MyApp::getInstance().toolMgr.getActiveCanvas();

    if (!activeCanvas)
        return;

    // activeCanvas->activeLayer().beginPreview();
    nativePlugin->effect_apply();
    // activeCanvas->activeLayer().endPreview(true);
}
#pragma endregion Plugin


#pragma region PluginMgr
void PluginMgr::loadAll() {
    char appPath[MAX_PATH + 1] = "";

    if (!GetModuleFileNameA(NULL, appPath, MAX_PATH)) {
        strcpy_s(appPath, ".");
    }

    std::fs::path pluginsPath = std::fs::path(appPath).remove_filename().append(PLUGINS_DIR);

    std::fs::create_directories(pluginsPath);

    for (const std::fs::directory_entry &entry : std::fs::directory_iterator(pluginsPath)) {
        if (!Plugin::isPluginCandidateFile(entry.path())) {
            continue;
        }

        try {
            load(entry.path());
        } catch (const std::exception &e) {
            ERR("Failed to load plugin \'%ls\' - error '%s'", entry.path().c_str(), e.what());
        }

    }
}

void PluginMgr::load(const std::fs::path &dllPath) {
    plugins.appendEmplace(new Plugin(dllPath));
}

void PluginMgr::deinitAll() {
    for (auto &plugin : plugins) {
        plugin->deinit();
    }
}

bool PluginMgr::hasExt(const char *name) const {
    return extensions.find(name) != extensions.end();
}

PluginExtension &PluginMgr::getExt(const char *name) {
    return *extensions.at(name);
}

bool PluginMgr::registerExt(PluginExtension *ext) {
    if (hasExt(ext->getCName()))
        return false;

    extensions[ext->getName()] = ext;

    return true;
}
#pragma endregion PluginMgr


#endif