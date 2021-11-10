#ifndef AGF_WIDGETS_WINDOW_MANAGER
#define AGF_WIDGETS_WINDOW_MANAGER

#include <AGF/widget.h>
#include <AGF/widgets/group.h>
#include <AGF/widgets/static_group.h>
#include <AGF/widgets/rectangle.h>
#include <AGF/widgets/label.h>
#include <AGF/widgets/button.h>


namespace abel::gui::widgets {

class WindowManager;
class Window;

}

extern template
class abel::unique_ptr<abel::gui::widgets::Window>;


namespace abel::gui::widgets {


namespace _impl {

class Header : public StaticGroup<Label, Button, Button> {
public:
    using Base = StaticGroup<Label, Button, Button>;
    EVENT_HANDLER_USING(Base)

    friend class Window;

    static constexpr unsigned
        HEADER_HEIGHT = 30,

        LABEL_TPAD = 0,
        LABEL_BPAD = 0,
        LABEL_LPAD = 20,
        BTN_TPAD = 0,
        BTN_BPAD = 0,
        BTN_WIDTH = 60,
        BTN_CLOSE_RPAD = LABEL_LPAD,
        BTN_MINIMIZE_RPAD = 0,

        BTN_CLOSE_RDX = BTN_CLOSE_RPAD,
        BTN_CLOSE_LDX = BTN_CLOSE_RDX + BTN_WIDTH,
        BTN_MINIMIZE_RDX = BTN_CLOSE_LDX + BTN_MINIMIZE_RPAD,
        BTN_MINIMIZE_LDX = BTN_MINIMIZE_RDX + BTN_WIDTH,
        LABEL_RPAD = BTN_MINIMIZE_LDX + 20,

        MIN_WIDTH = LABEL_RPAD + LABEL_LPAD;


    Header(Window *parent_, const Rect<double> &region_, const char *title_);

protected:
    SGRP_DECLARE_BINDING_I(title, 0)
    SGRP_DECLARE_BINDING_I(closeBtn, 1)
    SGRP_DECLARE_BINDING_I(minimizeBtn, 2)

};

class Borders : public StaticGroup<Rectangle> {
public:
    using Base = StaticGroup<Rectangle>;
    EVENT_HANDLER_USING(Base)

    static constexpr unsigned
        BORDER_WIDTH = 5;

    friend class Window;


    Borders(Window *parent_, const Rect<double> &region_);

protected:
    SGRP_DECLARE_BINDING_I(body, 0)

};

}  // namespace _impl


class WindowManager : public GroupOf<Window> {
public:
    using Base = GroupOf<Window>;
    EVENT_HANDLER_USING(Base)

    // friend class Window;


    WindowManager(Widget *parent_, const Rect<double> &region_) :
        Base(parent_, region_) {}

    template <typename C = Window, typename ... As>
    inline std::enable_if_t<std::is_base_of_v<Window, C>, C> &
    createWindow(Rect<double> relRegion, As &&... args) {
        // relRegion passed by value, since we have to pad it anyway

        static constexpr unsigned BORDER_WIDTH = -_impl::Borders::BORDER_WIDTH;
        static constexpr unsigned HEADER_HEIGHT = -_impl::Header::HEADER_HEIGHT;

        return createChild<C>(relRegion.pad(HEADER_HEIGHT, HEADER_HEIGHT, BORDER_WIDTH, HEADER_HEIGHT),
                              std::forward<As>(args)...);
    }

protected:
    using Base::addChild;

    //

};


class Window : public StaticGroup<Widget, _impl::Header, _impl::Borders> {
public:
    using Content = Widget;
    using Base = StaticGroup<Content, _impl::Header, _impl::Borders>;
    EVENT_HANDLER_USING(Base)

    friend class WindowManager;


    inline const WindowManager *getParentPtr() const { return dynamic_cast<WindowManager *>(parent); }
    inline       WindowManager *getParentPtr()       { return dynamic_cast<WindowManager *>(parent); }
    inline const WindowManager &getParent   () const { return *getParentPtr(); }
    inline       WindowManager &getParent   ()       { return *getParentPtr(); }

    SGRP_DECLARE_BINDING_I(contents, 0)

protected:
    SGRP_DECLARE_BINDING_I(header, 1)
    SGRP_DECLARE_BINDING_I(borders, 2)

    Window(WindowManager *parent_, const Rect<double> &region_,
           const char *title_, Content *contents_) :
        Base(parent_, region_, contents_,
             new _impl::Header(this, Rect<double>::wh(region_.x(), region_.y(), region_.w(), _impl::Header::HEADER_HEIGHT),
                               title_),
             new _impl::Borders(this, region_)
             ) {

            REQUIRE(region.h() >= _impl::Header::HEADER_HEIGHT + _impl::Borders::BORDER_WIDTH);
            REQUIRE(region.w() >= _impl::Borders::BORDER_WIDTH * 2 + _impl::Header::MIN_WIDTH);
        }

};


}


#endif // AGF_WIDGETS_WINDOW_MANAGER