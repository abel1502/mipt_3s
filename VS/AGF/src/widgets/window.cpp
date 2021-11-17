#include <AGF/llgui.h>
#include <AGF/widgets/window.h>
#include <AGF/application.h>


namespace abel::gui::widgets {


namespace _impl {

EVENT_HANDLER_IMPL(CloseBtn, Render) {
    return renderCustomized(event, Style::EL_CLOSE_BTN, true);
}

EVENT_HANDLER_IMPL(MinimizeBtn, Render) {
    return renderCustomized(event, Style::EL_MINIMIZE_BTN, true);
}

Header::Header(Window *parent_, const Rect<double> &region_, const char *title_) :
    Base(parent_, region_,
         new Label(this, region_.padded(LABEL_LPAD, LABEL_RPAD, LABEL_TPAD, LABEL_BPAD),
                   title_, 16, DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS),
         new CloseBtn(this, Rect<double>::se(region_.x1() - BTN_CLOSE_LDX, region_.y0() + BTN_TPAD,
                                           region_.x1() - BTN_CLOSE_RDX, region_.y1() - BTN_BPAD)),
         new MinimizeBtn(this, Rect<double>::se(region_.x1() - BTN_MINIMIZE_LDX, region_.y0() + BTN_TPAD,
                                           region_.x1() - BTN_MINIMIZE_RDX, region_.y1() - BTN_BPAD))
         ) {}

EVENT_HANDLER_IMPL(Header, MouseMove) {
    if (grabbed) {
        sigDrag(event.getDelta());
        return EventStatus::stop(EventStatus::TREE);
    }

    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    return status;
}

EVENT_HANDLER_IMPL(Header, MouseClick) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE))
        return status;

    if (!region.contains(event.pos) && !Application::getInstance().isMouseCaptured(this))
        return status;

    if (event.button != decltype(event.button)::Left)
        return status;

    if (event.type == decltype(event.type)::Down) {
        onMouseDown(event);
    } else {
        onMouseUp(event);
    }

    return EventStatus::stop(EventStatus::TREE);
}

EVENT_HANDLER_IMPL(Header, Render) {
    getStyle().drawElement(event.target, region, Style::EL_WND_HEADER, Style::ELS_NORMAL);

    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    return EventStatus::done();
}

void Header::onMouseDown(const MouseClickEvent &) {
    assert(!grabbed);  // The other way shouldn't be possible

    grabbed = true;
    sigDragStateChange(grabbed);

    Application::getInstance().captureMouse(this);
}

void Header::onMouseUp(const MouseClickEvent &) {
    if (!grabbed)
        return;

    grabbed = false;
    sigDragStateChange(grabbed);

    Application::getInstance().releaseMouse(this);
}

bool Header::setStyle(StyleManager::StyleHandle newHandle) {
    if (Base::setStyle(newHandle))
        return true;

    region.h(getStyle().wndHeaderHeight);  // TODO: Static resize!

    return false;
}


Borders::Borders(Window *parent_, const Rect<double> &region_) :
    Base(parent_, region_) {}

EVENT_HANDLER_IMPL(Borders, MouseClick) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE))
        return status;

    if (region.contains(event.pos))
        return EventStatus::stop(EventStatus::TREE);

    return EventStatus::skip();
}

EVENT_HANDLER_IMPL(Borders, Render) {
    getStyle().drawElement(event.target, region, Style::EL_WND_FRAME, Style::ELS_NORMAL);

    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    return EventStatus::done();
}


}  // namespace _impl


WindowManager::WindowManager(Widget *parent_, const Rect<double> &region_) :
    Base(parent_, region_) {}

void WindowManager::toggleVisibility(Window *window) {
    // TODO: Perhaps remove the check?
    if (children.findByValue(window) == children.end()) {
        throw error("Specified window doesn't belong to this manager");
    }

    window->hidden ^= 1;
    Application::getInstance().demandRedraw();
}

void WindowManager::close(Window *window) {
    // TODO: Perhaps remove the check?
    if (children.findByValue(window) == children.end()) {
        throw error("Specified window doesn't belong to this manager");
    }

    Application::getInstance().enqueueAction([this, window](Application &app) {
        auto iter = children.findByValue(window);
        if (iter == children.end())
            return;  // Apparently, the window was already deleted

        children.erase(iter);
        // app.demandRedraw();
    });
}


Window::Window(WindowManager *parent_, const Rect<double> &region_,
       const char *title_, Content *contents_) :
    Base(parent_, region_, contents_,
         nullptr,
         nullptr) {

    double headerHeight = getStyle().wndHeaderHeight;
    double borderWidth  = getStyle().wndBorderWidth;

    REQUIRE(region.h() >= headerHeight     + borderWidth);
    REQUIRE(region.w() >= headerHeight * 2 + borderWidth);

    header(new _impl::Header(this, Rect<double>::wh(region.x(), region.y(), region.w(), headerHeight), title_));
    borders(new _impl::Borders(this, Rect<double>::se(region.x0(), region.y0() + headerHeight, region.x1(), region.y1())));

    contents().staticShift(Vector2d(borderWidth, headerHeight));

    header().closeBtn().sigClick += [this]() {
        getParent().close(this);

        /*Application::getInstance().enqueueAction([&manager = getParent(), this](Application &app) {
            manager.close(this);
        });*/

        return true;
    };

    header().minimizeBtn().sigClick += [this]() {
        // getParent().minimize(this);

        Application::getInstance().enqueueAction([&manager = getParent(), this](Application &app) {
            manager.minimize(this);
        });

        return false;
    };

    header().sigDrag += [this](const Vector2d &delta) {
        dispatchEvent(MoveEvent(delta));
    };

    header().sigDragStateChange += [this](bool state) {
        dragged = state;

        if (!state) {
            cachedTexture = nullptr;
        }
    };
}

EVENT_HANDLER_IMPL(Window, Render) {
    if (cacheOnDrag && dragged && cachedTexture) {
        EventStatus status = Widget::dispatchEvent(event);

        if (!status.shouldHandle(status.NODE)) {
            return status;
        }

        event.target.embed(region, *cachedTexture);

        return EventStatus::done();
    }

    EventStatus status = Base::dispatchEvent(event);

    if (cacheOnDrag && dragged && !cachedTexture) {
        cachedTexture = new Texture((unsigned)region.w(), (unsigned)region.h());

        cachedTexture->embedPart(cachedTexture->getRect(), event.target, region);

        // abel::gui::Window::getInstance().getRect()
        // cachedTexture->clear(Color{0.8f});
        // cachedTexture->embedPart(getParent().getRegion().clamped(cachedTexture->getRect()),
        //                          event.target, region);
    }

    return status;
}

Window::EmptyBody::EmptyBody(Window *parent_) :
    Widget(parent_, Rect<double>::wh(0, 0, 0, 0)) {}

}


template
class abel::unique_ptr<abel::gui::widgets::Window>;


