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
                   title_),
         new CloseBtn(this, Rect<double>::se(region_.x1() - BTN_CLOSE_LDX, region_.y0() + BTN_TPAD,
                                             region_.x1() - BTN_CLOSE_RDX, region_.y1() - BTN_BPAD)),
         new MinimizeBtn(this, Rect<double>::se(region_.x1() - BTN_MINIMIZE_LDX, region_.y0() + BTN_TPAD,
                                                region_.x1() - BTN_MINIMIZE_RDX, region_.y1() - BTN_BPAD))
         ) {}

EVENT_HANDLER_IMPL(Header, MouseMove) {
    return mt.processEvent(event, Base::dispatchEvent(event));
}

EVENT_HANDLER_IMPL(Header, MouseClick) {
    return mt.processEvent(event, Base::dispatchEvent(event));
}

EVENT_HANDLER_IMPL(Header, MouseScroll) {
    return mt.processEvent(event, Base::dispatchEvent(event));
}

EVENT_HANDLER_IMPL(Header, Render) {
    EventStatus status = Widget::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    getStyle().drawElement(event.target, region, Style::EL_WND_HEADER, Style::ELS_NORMAL);

    REQUIRE(areChildrenSet());
    return _dispatchToChildren(event);
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
    return handleMouseOpaque(event, Base::dispatchEvent(event));
}

EVENT_HANDLER_IMPL(Borders, MouseMove) {
    return handleMouseOpaque(event, Base::dispatchEvent(event));
}

EVENT_HANDLER_IMPL(Borders, MouseScroll) {
    return handleMouseOpaque(event, Base::dispatchEvent(event));
}

EVENT_HANDLER_IMPL(Borders, Render) {
    EventStatus status = Widget::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    getStyle().drawElement(event.target, region, Style::EL_WND_FRAME, Style::ELS_NORMAL);

    return EventStatus::done();
}


}  // namespace _impl


WindowManager::WindowManager(Widget *parent_, const Rect<double> &region_) :
    Base(parent_, region_) {}


Window::Window(WindowManager *parent_, const Rect<double> &region_,
       const char *title_, Content *contents_) :
    Base(parent_, region_, contents_,
         nullptr,
         nullptr) {

    double headerHeight = getStyle().wndHeaderHeight;
    double borderWidth  = getStyle().wndBorderWidth;

    REQUIRE(region.h() >= headerHeight     + borderWidth);
    REQUIRE(region.w() >= headerHeight * 2 + borderWidth);

    header (new _impl::Header (this, Rect<double>::wh(region.x(), region.y(), region.w(), headerHeight), title_));
    borders(new _impl::Borders(this, Rect<double>::se(region.x0(), region.y0() + headerHeight, region.x1(), region.y1())));

    contents().staticShift(Vector2d(borderWidth, headerHeight));

    header().closeBtn().sigClick += [this]() {
        if (isEssential()) {
            return false;
        }

        die();  // Dispatched lazily

        return true;
    };

    header().minimizeBtn().sigClick += [this]() {
        Application::getInstance().enqueueAction([&manager = getParent(), this](Application &app) {
            setHidden(!isHidden());
        });

        return false;
    };

    header().mt.sigDrag += [this](MouseBtn btn, const MouseMoveEvent &event) {
        if (btn == MouseBtn::Left) {
            dispatchEvent(MoveEvent(event.getDelta()));
            Application::getInstance().demandRedraw();
        }

        return false;
    };

    header().mt.sigDragStateChange += [this](MouseBtn btn, MouseAttrs attrs, bool state) {
        if (btn == MouseBtn::Left) {
            dragged = state;

            if (!state) {
                cachedTexture = nullptr;
            }
        }

        return false;
    };
}

EVENT_HANDLER_IMPL(Window, Render) {
    if (cacheOnDrag && dragged) {
        if (!cachedTexture) {
            cachedTexture = new Texture{region};

            cachedTexture->setFillColor(Color::WHITE, 0.f);
            cachedTexture->clear();

            Base::dispatchEvent(RenderEvent{*cachedTexture});

            // abel::gui::Window::getInstance().getRect()
            // cachedTexture->clear(Color{0.8f});
            // cachedTexture->embedPart(getParent().getRegion().clamped(cachedTexture->getRect()),
            //                          event.target, region);
        }

        EventStatus status = Widget::dispatchEvent(event);

        if (!status.shouldHandle(status.NODE)) {
            return status;
        }

        event.target.embed(region, *cachedTexture);

        return EventStatus::done();
    }

    EventStatus status = Base::dispatchEvent(event);

    return status;
}

Window::EmptyBody::EmptyBody(Window *parent_) :
    Widget(parent_, Rect<double>::wh(0, 0, 0, 0)) {}

}


template
class abel::unique_ptr<abel::gui::widgets::Window>;


