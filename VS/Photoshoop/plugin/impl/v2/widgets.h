#pragma once

#ifndef PHOTOSHOOP_PLUGINS_WIDGETS_GUARD
#error "This file should not be included manually"
#endif

// If this far was reached, we must've been included from loader_v2.h
static_assert(PHOTOSHOOP_PLUGINS_VER == 2);

#include <AGF/llgui_pre.h>
#include <AGF/widget.h>
#include <AGF/widgets/all.h>
#include <ACL/type_traits.h>
#include "lib_std/widget_std.h"
#include "lib_std/keyboard_std.h"
#include "lib_std/widgets/collection.h"

// Just in case
class MyPWidget;
class PluginProxyWidget;

#include "../../../app.h"
#include "../../../tools_widget.h"


#pragma region PluginProxy
class PluginProxyWidget : public abel::gui::widgets::Group {
public:
    using Base = abel::gui::widgets::Group;
    EVENT_HANDLER_USING(Base);

    using pwidget_t = plugin::Widget;
    using mypwidget_t = MyPWidget;


    PluginProxyWidget(abel::gui::Widget *parent_,
                      const Rect<double> &region_,
                      pwidget_t *pwidget_,
                      bool owning_ = true);

    inline const pwidget_t &getPWidget() const { return *pwidget; }
    inline       pwidget_t &getPWidget()       { return *pwidget; }

    inline void setPWidget(pwidget_t *pwidget_) {
        pwidget = pwidget_;
    }

    pwidget_t *getPWidgetParent();

    inline const pwidget_t *getPWidgetParent() const {
        return const_cast<PluginProxyWidget *>(this)->getPWidgetParent();
    }

    template <typename T>
    T *getKnownChild() {
        return dynamic_cast<T *>(knownChild);
    }

    template <typename T>
    inline const T *getKnownChild() const {
        return const_cast<PluginProxyWidget *>(this)->getKnownChild<T>();
    }

    abel::gui::widgets::Label &getCaption();

    EVENT_HANDLER_OVERRIDE(abel::gui::Render);
    EVENT_HANDLER_OVERRIDE(abel::gui::MouseClick);
    EVENT_HANDLER_OVERRIDE(abel::gui::MouseMove);
    EVENT_HANDLER_OVERRIDE(abel::gui::MouseScroll);
    EVENT_HANDLER_OVERRIDE(abel::gui::KeyPress);
    EVENT_HANDLER_OVERRIDE(abel::gui::KeyboardInput);
    EVENT_HANDLER_OVERRIDE(abel::gui::VisibilityUpdate);

    inline bool checkMagic() const {
        return magic == MAGIC;
    }

    void registerKnownChild(abel::gui::Widget *knownChild_);

protected:
    static constexpr uint64_t MAGIC = ((uint64_t)'ABEL' << 32) | (uint64_t)'PROX';
    const uint64_t magic = MAGIC;
    pwidget_t *pwidget;
    bool owning;
    abel::gui::Widget *knownChild = nullptr;  // TODO: A way to recognize a child as known
    abel::gui::widgets::Label *caption = nullptr;

};
#pragma endregion PluginProxy


#pragma region MyPWidget
class MyPWidget : virtual public plugin::Widget {
public:
    MyPWidget(PluginProxyWidget *proxyWidget_);

    virtual void set_position(const plugin::Vec2f &position_) override;
    virtual void set_size(const plugin::Vec2f &size_) override;

    virtual plugin::WBody get_body() override;
    virtual void set_body(const plugin::WBody &body_) override;

    virtual plugin::Widget *get_parent() const override;
    virtual void set_parent(plugin::Widget *parent_) override;

    virtual plugin::RenderTarget *get_texture() override;
    virtual void set_texture(plugin::RenderTarget *texture_) override;

    virtual bool is_active() override;
    virtual bool is_inside(const plugin::Vec2f &pos) override;

    virtual bool add_child(plugin::Widget *child) override;

    virtual void set_to_delete() override;
    virtual bool delete_child(plugin::Widget *child) override;
    virtual bool delete_from_parent() override;

    virtual void on_render          (const plugin::Event::Render          &event) override;
    virtual void on_tick            (const plugin::Event::Tick            &event) override;
    virtual void on_mouse_press     (const plugin::Event::MousePress      &event) override;
    virtual void on_mouse_release   (const plugin::Event::MouseRelease    &event) override;
    virtual void on_mouse_move      (const plugin::Event::MouseMove       &event) override;
    virtual void on_key_down        (const plugin::Event::KeyDown         &event) override;
    virtual void on_key_up          (const plugin::Event::KeyUp           &event) override;
    virtual void on_text_enter      (const plugin::Event::TextEnter       &event) override;
    virtual void on_scroll          (const plugin::Event::Scroll          &event) override;
    virtual void on_hide            (const plugin::Event::Hide            &event) override;
    virtual void on_show            (const plugin::Event::Show            &event) override;

    virtual void hide()  override;
    virtual void show()  override;
    virtual void focus() override;

    // Provides a backup implementation for things without a caption normally
    virtual void set_caption(const char *text, size_t font_size, const plugin::Vec2f *pos = nullptr) override;

    virtual void set_base_color(const plugin::RGBA &color) override;

    virtual void *get_extra_data(void *arg) override;

protected:
    PluginProxyWidget *proxyWidget;
    plugin::RenderTarget *textureOverride = nullptr;

};
#pragma endregion MyPWidget


#pragma region Collection
#pragma warning(push)
// Inheritance via dominance is exatly what I'd like
#pragma warning(disable : 4250)

class MyPButton : public MyPWidget, public virtual plugin::Button {
public:
    using native_wt = abel::gui::widgets::Button;


    MyPButton(PluginProxyWidget *proxyWidget_, const char *caption);

    virtual void set_handler(const HandlerType &handler_) override;
    virtual HandlerType &get_handler() override;

    virtual void set_caption(const char *text, size_t font_size, const plugin::Vec2f *pos = nullptr) override;

protected:
    HandlerType handler{};

};

class MyPColorPicker : public MyPWidget, public virtual plugin::ColorPicker {
public:
    using native_wt = ::ColorPicker;


    MyPColorPicker(PluginProxyWidget *proxyWidget_);

    virtual void set_handler(const HandlerType &handler_) override;
    virtual HandlerType &get_handler() override;

    virtual plugin::RGBA get_color() override;
    virtual void set_color(plugin::RGBA color) override;

protected:
    HandlerType handler{};

};

class MyPLabel : public MyPWidget, public virtual plugin::Label {
public:
    using native_wt = abel::gui::widgets::Label;


    MyPLabel(PluginProxyWidget *proxyWidget_, const char *text);

    virtual std::string_view get_text() const override;
    virtual void set_text(std::string_view text) override;

    virtual void set_caption(const char *text, size_t font_size, const plugin::Vec2f *pos = nullptr) override;

};

class MyPSlider : public MyPWidget, public virtual plugin::Slider {
public:
    using native_h_wt  = abel::gui::widgets::SliderH;
    using native_v_wt  = abel::gui::widgets::SliderV;
    using native_2d_wt = abel::gui::widgets::Slider2D;


    MyPSlider(PluginProxyWidget *proxyWidget_, Type type_);

    virtual void set_handler(const HandlerType &handler_) override;
    virtual HandlerType &get_handler() override;

    virtual plugin::Vec2f get_fraction_2d() override;
    virtual void set_fraction_2d(const plugin::Vec2f &frac) override;

    virtual float get_fraction() override;
    virtual void set_fraction(float frac) override;

protected:
    Type type;
    HandlerType handler{};

};

// class MyPTextField

class MyPWindow : public MyPWidget, public virtual plugin::Window {
public:
    MyPWindow(PluginProxyWidget *proxyWidget_, const char *name);

    virtual void set_show_handler(const HandlerType &handler_) override;
    virtual HandlerType &get_show_handler() override;

    virtual void set_hide_handler(const HandlerType &handler_) override;
    virtual HandlerType &get_hide_handler() override;

    virtual bool set_name(const char *name) override;
    virtual const char *get_name() override;

    virtual void on_hide(const plugin::Event::Hide &) override;
    virtual void on_show(const plugin::Event::Show &) override;

    abel::gui::widgets::Window *getTrueWindow();

    inline const abel::gui::widgets::Window *getTrueWindow() const {
        return const_cast<MyPWindow *>(this)->getTrueWindow();
    }

protected:
    HandlerType showHandler{};
    HandlerType hideHandler{};

};

#pragma warning(pop)
#pragma endregion Collection


