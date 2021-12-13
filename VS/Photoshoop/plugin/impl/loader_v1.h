#pragma once

#ifndef PHOTOSHOOP_PLUGINS_IMPL_GUARD
#error "This file should not be included manually"
#endif


#if PHOTOSHOOP_PLUGINS_VER == 1

#include <AGF/llgui_pre.h>
#include <AGF/impl/windows.h>
#include <ACL/gui/rect.h>
#include <ACL/gui/color.h>
#include <ACL/math/vector.h>
#include <ACL/general.h>
#include <ACL/vector.h>
#include <ACL/unique_ptr.h>
#include <filesystem>


using abel::gui::Rect;
using abel::gui::Color;
using abel::math::Vector2d;


namespace plugin {
#include "v1/plugin_std.hpp"
}

namespace std {
namespace fs = filesystem;
}


class Plugin;
class PluginMgr;


class Plugin {
public:
    DECLARE_ERROR(error, abel::error);


    inline Plugin(const std::fs::path &dllPath, const plugin::PAppInterface *nativeApp) {
        load(dllPath, nativeApp);
    }

    Plugin(const Plugin &other) = delete;
    Plugin &operator=(const Plugin &other) = delete;

    inline Plugin(Plugin &&other) noexcept;
    inline Plugin &operator=(Plugin &&other) noexcept;

    ~Plugin();

    static bool isPluginCandidateFile(const std::fs::path &path);

    const plugin::PPluginInterface *getNativePlugin() const {
        return nativePlugin;
    }

    void onToolPress(const Vector2d &pos) const;
    void onToolRelease(const Vector2d &pos) const;
    void onToolMove(const Vector2d &pos0, const Vector2d &pos1) const;

    void onEffectApply() const;

    void deinit();

protected:
    HMODULE dllHandle = NULL;
    const plugin::PPluginInterface *nativePlugin = nullptr;
    const plugin::PPluginInfo *info = nullptr;


    void load(const std::fs::path &dllPath, const plugin::PAppInterface *nativeApp);

    void init(const plugin::PAppInterface *nativeApp);

};

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


class PluginMgr {
public:
    static constexpr char PLUGINS_DIR[] = "plugins/";


    PluginMgr() = default;

    void loadAll();

    void load(const std::fs::path &dllPath);

    Plugin &find(plugin::PPluginInterface *nativePlugin) {
        for (auto &plugin : plugins) {
            if (plugin->getNativePlugin() == nativePlugin) {
                return *plugin;
            }
        }

        throw Plugin::error("Plugin not found");
    }

    inline const Plugin &find(const plugin::PPluginInterface *nativePlugin) const {
        return const_cast<PluginMgr *>(this)->find(nativePlugin);
    }

    void deinitAll();

protected:
    static plugin::PAppInterface nativeApp;


    abel::vector<abel::unique_ptr<Plugin>> plugins{};

};


#endif