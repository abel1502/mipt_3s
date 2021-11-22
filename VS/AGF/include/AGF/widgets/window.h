#ifndef AGF_WIDGETS_WINDOW_MANAGER
#define AGF_WIDGETS_WINDOW_MANAGER

#include <AGF/widget.h>
#include <AGF/widgets/group.h>
#include <AGF/widgets/static_group.h>
#include <AGF/widgets/rectangle.h>
#include <AGF/widgets/label.h>
#include <AGF/widgets/button.h>
#include <AGF/helpers/mouse_tracker.h>


namespace abel::gui::widgets {

class WindowManager;
class Window;

}

extern template
class abel::unique_ptr<abel::gui::widgets::Window>;


namespace abel::gui::widgets {


namespace _impl {

class CloseBtn : public Button {
public:
    inline CloseBtn(Widget *parent_, const Rect<double> &region_) :
        Button(parent_, region_, "") {}

    EVENT_HANDLER_OVERRIDE(Render);

};

class MinimizeBtn : public Button {
public:
    inline MinimizeBtn(Widget *parent_, const Rect<double> &region_) :
        Button(parent_, region_, "") {}

    EVENT_HANDLER_OVERRIDE(Render);

};

class Header : public StaticGroup<Label, CloseBtn, MinimizeBtn> {
public:
    using Base = StaticGroup<Label, CloseBtn, MinimizeBtn>;
    EVENT_HANDLER_USING(Base);

    friend class abel::gui::widgets::Window;

    static constexpr double
        LABEL_TPAD = 0,
        LABEL_BPAD = 0,
        LABEL_LPAD = 10,
        BTN_TPAD = 5,
        BTN_BPAD = 5,
        BTN_WIDTH = 40,
        BTN_CLOSE_RPAD = 5,
        BTN_MINIMIZE_RPAD = 5,

        BTN_CLOSE_RDX = BTN_CLOSE_RPAD,
        BTN_CLOSE_LDX = BTN_CLOSE_RDX + BTN_WIDTH,
        BTN_MINIMIZE_RDX = BTN_CLOSE_LDX + BTN_MINIMIZE_RPAD,
        BTN_MINIMIZE_LDX = BTN_MINIMIZE_RDX + BTN_WIDTH,
        LABEL_RPAD = BTN_MINIMIZE_LDX + LABEL_LPAD,

        MIN_WIDTH = LABEL_RPAD + LABEL_LPAD;


    Header(Window *parent_, const Rect<double> &region_, const char *title_);

    EVENT_HANDLER_OVERRIDE(MouseMove);

    EVENT_HANDLER_OVERRIDE(MouseClick);

    EVENT_HANDLER_OVERRIDE(Render);

    virtual bool setStyle(StyleManager::StyleHandle newHandle) override;

protected:
    MouseTracker mt{this};  // Exposed to the window


    SGRP_DECLARE_BINDING_T(title, Label)
    SGRP_DECLARE_BINDING_T(closeBtn, CloseBtn)
    SGRP_DECLARE_BINDING_T(minimizeBtn, MinimizeBtn)

};

class Borders : public Widget {
public:
    using Base = Widget;
    EVENT_HANDLER_USING(Widget);

    friend class abel::gui::widgets::Window;


    Borders(Window *parent_, const Rect<double> &region_);

    EVENT_HANDLER_OVERRIDE(MouseClick);

    EVENT_HANDLER_OVERRIDE(Render);

protected:
    //

};

}  // namespace _impl


class WindowManager : public GroupOf<Window> {
public:
    using Base = GroupOf<Window>;
    EVENT_HANDLER_USING(Base);

    // friend class Window;


    WindowManager(Widget *parent_, const Rect<double> &region_);

    template <typename C = Window, typename ... As>
    inline std::enable_if_t<std::is_base_of_v<Window, C>, C &>
    createWindow(Rect<double> relRegion, As &&... args) {
        // relRegion passed by value, since we have to pad it anyway

        double borderWidth  = -getStyle().wndBorderWidth;
        double headerHeight = -getStyle().wndHeaderHeight;

        return dynamic_cast<C &>(addChild(
            new C(nullptr, relRegion.pad(headerHeight,
                                         headerHeight,
                                         borderWidth,
                                         headerHeight),
                  std::forward<As>(args)...)
        ));
    }

    void close(Window *window);

protected:
    using Base::createChild;

};


class Window : public StaticGroup<Widget, _impl::Header, _impl::Borders> {
public:
    using Content = Widget;
    using Base = StaticGroup<Content, _impl::Header, _impl::Borders>;
    EVENT_HANDLER_USING(Base);

    friend class WindowManager;
    // TODO: ?
    friend class _impl::Header;
    friend class _impl::Borders;

    class EmptyBody : public Widget {
    public:
        EmptyBody(Window * parent_ = nullptr);
    };


    Window(const Window &other) = delete;
    Window &operator=(const Window &other) = delete;

    Window(Window &&other) = default;
    Window &operator=(Window &&other) = default;

    inline const WindowManager *getParentPtr() const { return dynamic_cast<WindowManager *>(parent); }
    inline       WindowManager *getParentPtr()       { return dynamic_cast<WindowManager *>(parent); }
    inline const WindowManager &getParent   () const { return *getParentPtr(); }
    inline       WindowManager &getParent   ()       { return *getParentPtr(); }

    SGRP_DECLARE_BINDING_I(contents, 0)

    EVENT_HANDLER_OVERRIDE(Render);

protected:
    bool dragged = false;
    bool cacheOnDrag = true;
    unique_ptr<Texture> cachedTexture = nullptr;


    SGRP_DECLARE_BINDING_I(header, 1)
    SGRP_DECLARE_BINDING_I(borders, 2)

    Window(WindowManager *parent_, const Rect<double> &region_,
           const char *title_, Content *contents_);

};


}


#endif // AGF_WIDGETS_WINDOW_MANAGER
