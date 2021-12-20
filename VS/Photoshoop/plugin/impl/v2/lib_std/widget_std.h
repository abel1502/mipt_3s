#pragma once


#include <functional>

#include "types_std.h"
#include "events_std.h"
#include <ACL/gui/rect.h>


namespace plugin {

struct WBody {
    Vec2f position;
    Vec2f size;

    WBody(const Vec2f &position = {0, 0}, const Vec2f &size = {0, 0}) : position(position), size(size) {}

    WBody(const abel::gui::Rect<double> &region) :
        position{region.getPos()}, size{region.getDiag()} {}

    operator abel::gui::Rect<double>() const {
        return abel::gui::Rect<double>::wh(position, size);
    }
};

class RenderTarget;

class Widget {
public:
    virtual ~Widget() {}

    virtual void set_position(const Vec2f &position_) = 0;
    virtual void set_size(const Vec2f &size_) = 0;

    virtual WBody get_body() = 0;
    virtual void set_body(const WBody &body_) = 0;

    virtual Widget *get_parent() const = 0;
    virtual void set_parent(Widget *parent_) = 0;

    virtual RenderTarget *get_texture() = 0;
    virtual void set_texture(RenderTarget *texture_) = 0;

    virtual bool is_active() = 0;
    virtual bool is_inside(const Vec2f &pos) = 0;

    virtual bool add_child(Widget *child) = 0;

    virtual void set_to_delete() = 0; // set to true -> app will try to delete it as soon as possible from its side
                                      // after once setting to true you can not use this widget anymore, it can
                                      // already be deleted
    virtual bool delete_child(Widget *child) = 0;
    virtual bool delete_from_parent() = 0;

    virtual void on_render          (const Event::Render          &event) = 0;
    virtual void on_tick            (const Event::Tick            &event) = 0;
    virtual void on_mouse_press     (const Event::MousePress      &event) = 0;
    virtual void on_mouse_release   (const Event::MouseRelease    &event) = 0;
    virtual void on_mouse_move      (const Event::MouseMove       &event) = 0;
    virtual void on_key_down        (const Event::KeyDown         &event) = 0;
    virtual void on_key_up          (const Event::KeyUp           &event) = 0;
    virtual void on_text_enter      (const Event::TextEnter       &event) = 0;
    virtual void on_scroll          (const Event::Scroll          &event) = 0;
    virtual void on_hide            (const Event::Hide            &event) = 0;
    virtual void on_show            (const Event::Show            &event) = 0;

    virtual void hide()  = 0;
    virtual void show()  = 0;
    virtual void focus() = 0;

    virtual void set_caption(const char *text, size_t font_size, const Vec2f *pos = nullptr) = 0;
    virtual void set_base_color(const RGBA &color) = 0;

    // You are obliged to return nullptr from this in your plugins
    virtual void *get_extra_data(void *arg) = 0;
};

}
