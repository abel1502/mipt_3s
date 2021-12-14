#include <AGF/llgui.h>
#include "../loader.h"

#if PHOTOSHOOP_PLUGINS_VER == 1
#include <string_view>
#include <cstdint>
#include <cstdarg>
#include <cstring>
#include <string>
#include <ACL/gui/color.h>
#include <ACL/math/vector.h>
#include <ACL/gui/rect.h>
#include "../../app.h"
#include "../../canvas.h"
#include "plugin_tool.h"


#pragma region nativeApp
namespace {

using abel::gui::Rect;
using abel::gui::Color;
using abel::math::Vector2d;

#pragma warning (suppress : 4455)
using std::literals::operator ""sv;

static constexpr std::string_view PEXT_ABEL = "abel"sv;

bool ext_enable(const char *name) {
    if (name == PEXT_ABEL) {
        return true;
    }

    return false;
}

uint32_t ext_abel_get_magic() {
    return 'ABEL';
}

uint32_t ext_abel_get_version() {
    return 0;
}

void *dispatchAbelExt(const char *name) {
    // TODO: Stuff :)

    if (name == "get_magic"sv) {
        return &ext_abel_get_magic;
    }

    if (name == "get_version"sv) {
        return &ext_abel_get_version;
    }

    return nullptr;
}

void *ext_get_func(const char *extension, const char *func) {
    if (extension == PEXT_ABEL) {
        return dispatchAbelExt(func);
    }

    return nullptr;
}

void general_log(const char *fmt, ...) {
    va_list args{};

    va_start(args, fmt);
    int requiredLen = vsnprintf(nullptr, 0, fmt, args);
    va_end(args);

    if (requiredLen < 0) {
        throw abel::error("vsnprintf failed");
    }

    std::string msg{};
    msg.resize(requiredLen + 1);
    va_start(args, fmt);
    vsnprintf(msg.data(), requiredLen + 1, fmt, args);
    va_end(args);

    DBG("[PLUGIN] %s", msg.data());
}

double general_get_absolute_time() {
    return MyApp::getInstance().getTime();
}

void general_release_pixels(plugin::PRGBA *pixels) {
    delete[] (uint32_t *)pixels;
}

plugin::PRGBA convertColor(const abel::gui::PackedColor &color, unsigned char alpha) {
    return plugin::PRGBA{color.R, color.G, color.B, alpha};
}

plugin::PRGBA convertColor(const Color &color, float alpha) {
    return convertColor(color.pack(), (unsigned char)(alpha * 255.f));
}

void applyColor(abel::gui::Texture &texture, const plugin::PRGBA &color) {
    abel::gui::PackedColor pc{color.r, color.g, color.b, color.a};
    Color myColor{pc};
    float alpha = (float)pc.A / 255.f;

    texture.setFillColor(myColor, alpha);
    texture.setLineColor(myColor, alpha);
}

plugin::PRGBA general_get_color() {
    const auto &toolMgr = MyApp::getInstance().toolMgr;

    return convertColor(toolMgr.getColor(), (float)toolMgr.getAlpha());
}

float general_get_size() {
    return (float)MyApp::getInstance().toolMgr.getRadius();
}

plugin::PRGBA *target_get_pixels() {
    static_assert(sizeof(plugin::PRGBA) == sizeof(uint32_t));

    Canvas *activeCanvas = MyApp::getInstance().toolMgr.getActiveCanvas();
    if (!activeCanvas) {
        return nullptr;
    }

    abel::gui::Texture &texture = activeCanvas->activeLayer().getTexture();

    uint32_t *buf = texture.getBufRGBA(true, false);
    size_t size = (size_t)texture.width() * (size_t)texture.height();
    uint32_t *copy = new uint32_t[size];
    if (!copy) {
        texture.flushBuf();

        return nullptr;
    }

    memcpy(copy, buf, size * sizeof(uint32_t));

    texture.flushBuf();

    return (plugin::PRGBA *)copy;
}

void target_get_size(size_t *width, size_t *height) {
    Canvas *activeCanvas = MyApp::getInstance().toolMgr.getActiveCanvas();
    Vector2d size =
        activeCanvas ?
        activeCanvas->activeLayer().getSize() :
        Vector2d{};

    if (width) {
        *width = (size_t)size.x();
    }

    if (height) {
        *height = (size_t)size.y();
    }
}

plugin::PVec2f convertVector(const Vector2d &vec) {
    return plugin::PVec2f{(float)vec.x(), (float)vec.y()};
}

Vector2d convertVector(const plugin::PVec2f &vec) {
    return Vector2d{(double)vec.x, (double)vec.y};
}

#define BODY_RENDER_(...) {                                                 \
    assert(render_mode);                                                    \
                                                                            \
    Canvas *activeCanvas = MyApp::getInstance().toolMgr.getActiveCanvas();  \
    if (!activeCanvas) {                                                    \
        return;                                                             \
    }                                                                       \
                                                                            \
    abel::gui::Texture &target =                                            \
        (render_mode->draw_policy == plugin::PPDP_ACTIVE) ?                 \
        activeCanvas->activeLayer().getTexture() :                          \
        activeCanvas->activeLayer().getPreview();                           \
                                                                            \
    applyColor(target, color);                                              \
                                                                            \
    if (render_mode->blend == plugin::PPBM_COPY) {                          \
        target.setOverwrite();                                              \
    }                                                                       \
                                                                            \
    __VA_ARGS__;                                                            \
                                                                            \
    target.resetOverwrite();                                                \
}

void render_circle(plugin::PVec2f position, float radius, plugin::PRGBA color,
                   const plugin::PRenderMode *render_mode) {
    BODY_RENDER_(
        target.drawCircle(convertVector(position), (double)radius);
    );
}

void render_line(plugin::PVec2f start, plugin::PVec2f end,
                 plugin::PRGBA color, const plugin::PRenderMode *render_mode) {
    BODY_RENDER_(
        target.drawLine(convertVector(start), convertVector(end));
    );
}

void render_triangle(plugin::PVec2f p1, plugin::PVec2f p2,
                     plugin::PVec2f p3, plugin::PRGBA color,
                     const plugin::PRenderMode *render_mode) {
    BODY_RENDER_(
        target.drawTriangle(convertVector(p1), convertVector(p2), convertVector(p3));
    );
}

void render_rectangle(plugin::PVec2f p1, plugin::PVec2f p2, plugin::PRGBA color,
                      const plugin::PRenderMode *render_mode) {
    BODY_RENDER_(
        target.drawRect(Rect<double>::se(convertVector(p1), convertVector(p2)));
    );
}

void render_pixels(plugin::PVec2f position, const plugin::PRGBA *data,
                   size_t width, size_t height,
                   const plugin::PRenderMode *render_mode) {
    assert(render_mode);
    assert(data);

    Canvas *activeCanvas = MyApp::getInstance().toolMgr.getActiveCanvas();
    if (!activeCanvas) {
        return;
    }

    abel::gui::Texture &target =
        (render_mode->draw_policy == plugin::PPDP_ACTIVE) ?
        activeCanvas->activeLayer().getTexture() :
        activeCanvas->activeLayer().getPreview();

    if (render_mode->blend == plugin::PPBM_COPY) {
        target.setOverwrite();
    }

    abel::gui::Texture tmp{(unsigned)width, (unsigned)height};
    memcpy(tmp.getBufRGBA(false, true), data, width * height * sizeof(uint32_t));
    tmp.flushBuf();

    target.embed(Rect<double>::wh((double)position.x, (double)position.y,
                                  (double)width, (double)height), tmp);

    target.resetOverwrite();
}

#undef BODY_RENDER_


plugin::PAppInterface createNativeApp() {
    plugin::PAppInterface result{};

    result.std_version = plugin::PSTD_VERSION;

    result.reserved = nullptr;
    result.extensions.enable = ext_enable;
    result.extensions.get_func = ext_get_func;

    result.general.feature_level = 0 /*| plugin::PFL_SETTINGS_SUPPORT*/;
    result.general.log               = general_log;
    result.general.get_absolute_time = general_get_absolute_time;
    result.general.release_pixels    = general_release_pixels;
    result.general.get_color         = general_get_color;
    result.general.get_size          = general_get_size;

    result.target.get_pixels = target_get_pixels;
    result.target.get_size   = target_get_size;

    result.render.circle    = render_circle;
    result.render.line      = render_line;
    result.render.pixels    = render_pixels;
    result.render.rectangle = render_rectangle;
    result.render.triangle  = render_triangle;

    memset(&result.settings, 0, sizeof(result.settings));
    memset(&result.shader,   0, sizeof(result.shader));

    return result;
}

}  // namespace _myimpl


plugin::PAppInterface PluginMgr::nativeApp = createNativeApp();
#pragma endregion nativeApp


#pragma region Plugin
using gpi_func_ptr_t = const plugin::PPluginInterface *(FAR WINAPI *)();


bool Plugin::isPluginCandidateFile(const std::fs::path &path) {
    return std::fs::is_regular_file(path) &&
           path.extension().compare(".dll") == 0;
}

void Plugin::load(const std::fs::path &dllPath, const plugin::PAppInterface *nativeApp) {
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
        (gpi_func_ptr_t)GetProcAddress(dllHandle, plugin::PGET_INTERFACE_FUNC);

    if (!get_plugin_interface) {
        BAD_PLUGIN_();
        throw error("The plugin dll doesn't provide a get_plugin_interface() function");
    }

    nativePlugin = get_plugin_interface();

    if (!nativePlugin) {
        BAD_PLUGIN_();
        throw error("get_plugin_interface() returned nullptr");
    }

    if (nativePlugin->std_version != plugin::PSTD_VERSION) {
        BAD_PLUGIN_();
        ERR("Expected version 0x%8x, got 0x%8x", plugin::PSTD_VERSION, nativePlugin->std_version);
        throw error("Plugin of unsupported version");
    }

    if (!(nativePlugin->general.init &&
          nativePlugin->general.deinit)) {
        // Not all are checked here, but this is something
        BAD_PLUGIN_();
        throw error("Plugin missing essential functions");
    }

    DBG("Successfully loaded plugin from \"%ls\" with handle 0x%p. Initializing...", dllPath.c_str(), nativePlugin);

    init(nativeApp);

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

void Plugin::init(const plugin::PAppInterface *nativeApp) {
    assert(nativePlugin);
    assert(nativePlugin->general.init);

    plugin::PPluginStatus status = nativePlugin->general.init(nativeApp);

    if (status != plugin::PPS_OK) {
        throw error("Plugin failed to initialize itself");
    }

    REQUIRE(nativePlugin->general.get_info);
    info = nativePlugin->general.get_info();
    REQUIRE(info);
    REQUIRE(info->interface == nativePlugin);
    REQUIRE(info->std_version == nativePlugin->std_version);

    MyApp &app = MyApp::getInstance();

    REQUIRE(nativePlugin->general.on_tick);
    app.sigTick += [on_tick = nativePlugin->general.on_tick](double deltaT) {
        on_tick(deltaT);

        return false;
    };

    REQUIRE(nativePlugin->general.on_settings_update);
    app.toolMgr.sigConfigChanged += [on_settings_update =
                                     nativePlugin->general.on_settings_update](ToolManager &) {
        on_settings_update();

        return false;
    };

    switch (info->type) {
    case plugin::PPT_TOOL: {
        REQUIRE(nativePlugin->tool.on_press);
        REQUIRE(nativePlugin->tool.on_release);
        REQUIRE(nativePlugin->tool.on_move);

        app.getToolsWidget().addToolButton(
            app.toolMgr.addTool(new PluginTool(&app.toolMgr, *this)),
            info->name
        );
    } break;

    case plugin::PPT_EFFECT: {
        REQUIRE(nativePlugin->effect.apply);

        app.getToolsWidget().addEffectButton(
            app.toolMgr.addEffect(new PluginEffect(&app.toolMgr, *this)),
            info->name
        );
    } break;

    NODEFAULT
    }

    DBG("Successfully initialized plugin '%s' (%s) by '%s'.\n"
        "Brief description:\n"
        "\"%s\"\n",
        info->name,
        info->type == plugin::PPT_TOOL ? "tool" : "effect",
        info->author,
        info->description);
}

void Plugin::deinit() {
    if (!nativePlugin) {
        return;
    }

    assert(nativePlugin->general.deinit);

    nativePlugin->general.deinit();
}

void Plugin::onToolPress(const Vector2d &pos) const {
    assert(info->type == plugin::PPT_TOOL);

    nativePlugin->tool.on_press(convertVector(pos));
}

void Plugin::onToolRelease(const Vector2d &pos) const {
    assert(info->type == plugin::PPT_TOOL);

    nativePlugin->tool.on_release(convertVector(pos));
}

void Plugin::onToolMove(const Vector2d &pos0, const Vector2d &pos1) const {
    assert(info->type == plugin::PPT_TOOL);

    nativePlugin->tool.on_move(convertVector(pos0), convertVector(pos1));
}

void Plugin::onEffectApply() const {
    assert(info->type == plugin::PPT_EFFECT);

    nativePlugin->effect.apply();
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
    plugins.appendEmplace(new Plugin(dllPath, &nativeApp));
}

void PluginMgr::deinitAll() {
    for (auto &plugin : plugins) {
        plugin->deinit();
    }
}
#pragma endregion PluginMgr


#endif