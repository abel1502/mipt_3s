#include <AGF/llgui.h>
#include "app.h"
#include "tool.h"


Tool::Tool(ToolManager *manager_) :
    manager(manager_) {}


void Tool::applyLine(Layer &layer, const Vector2d &pos0, const Vector2d &pos1) {
    constexpr unsigned NSTEPS = 100;

    Vector2d cur = pos0;
    const Vector2d step = (pos1 - pos0) / NSTEPS;

    for (unsigned i = 0; i < NSTEPS; ++i, cur += step) {
        applyPoint(layer, cur);
    }
}

void Tool::onBegin(const Vector2d &pos) {}

void Tool::onEnd(const Vector2d &pos) {}


Effect::Effect(ToolManager *manager_) :
    manager{manager_} {}

Effect::~Effect() {
    if (settingsWnd) {
        settingsWnd->die();
    }
}

bool Effect::supportsSettings() const {
    return false;
}

Vector2d Effect::getDesiredSettingsWndSize() const {
    constexpr double DEFAULT_SIZE = 150;

    return Vector2d{DEFAULT_SIZE};
}

void Effect::showSettings(bool hidden) {
    if (!supportsSettings()) {
        return;
    }

    if (!settingsWnd) {
        widgets::WindowManager &windowMgr = MyApp::getInstance().getWindowMgrWidget();
        Vector2d size = getDesiredSettingsWndSize();
        Vector2d pos = windowMgr.getRegion().getCenter() - size / 2;
        settingsWnd = windowMgr.createWindow(Rect<double>::wh(pos, size), nullptr);

        populateSettingsWindow();
    }

    settingsWnd->setHidden(hidden);
}

void Effect::populateSettingsWindow() {
    REQUIRE(settingsWnd);
}


