#define JUST_DEFINE_PLUGINS_VER
#include "../../loader.h"


#if PHOTOSHOOP_PLUGINS_VER == 2
#include <AGF/llgui.h>
#include "../../loader.h"
#define PHOTOSHOOP_PLUGINS_WIDGETS_GUARD
#include "widgets.h"
#undef PHOTOSHOOP_PLUGINS_WIDGETS_GUARD
#include <AGF/helpers/widget_ref.h>
#include "../../../app.h"
#include "../../../canvas.h"


#pragma region PluginProxy
PluginProxyWidget::PluginProxyWidget(abel::gui::Widget *parent_,
                                     const Rect<double> &region_,
                                     pwidget_t *pwidget_,
                                     bool owning_) :
    Base(parent_, region_),
    pwidget{pwidget_},
    owning{owning_} {

    suppressRefocus = true;

    MyApp::getInstance().sigTick += [inst = abel::gui::WidgetRefTo{this}](double deltaT) {
        if (!inst) {
            return true;
        }

        inst->getPWidget().on_tick(plugin::Event::Tick{deltaT});

        if (inst->knownChild && inst->knownChild->isDead()) {
            inst->die();
        }

        return false;
    };

    sigDeath += [this](abel::gui::Widget &) {
        if (knownChild) {
            knownChild->die();
        }
    };
}

EVENT_HANDLER_IMPL(PluginProxyWidget, abel::gui::Render) {
    EventStatus status = Widget::dispatchEvent(event);
    assert(status.shouldHandle(status.NODE));

    pwidget->on_render(plugin::Event::Render{});

    plugin::RenderTarget *customTexture = pwidget->get_texture();
    if (customTexture) {
        abel::gui::Texture *texture = extractTexture(*customTexture);
        REQUIRE(texture);

        event.target.embed(region, *texture);
    }

    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        if (customTexture && it->get() == knownChild) {
            continue;
        }

        status = dispatchToChild(**it, event);

        if (status.update()) {
            return status;
        }
    }

    return status;
}

EVENT_HANDLER_IMPL(PluginProxyWidget, abel::gui::MouseClick) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    constexpr plugin::MouseButton BTN_CVT[3] = {plugin::left, plugin::middle, plugin::right};
    plugin::MouseButton btn = BTN_CVT[(unsigned)event.button];

    Vector2d localPos = event.pos - region.getPos();

    if (hitTest(event.pos) ||
        MyApp::getInstance().isMouseCaptured(this)) {

        if (event.type == abel::gui::MouseClickType::Down) {
            pwidget->on_mouse_press(plugin::Event::MousePress{localPos, btn});
            // try {
            //     MyApp::getInstance().captureMouse(this);
            // } catch (abel::require_error) {
            // }
        } else {
            pwidget->on_mouse_release(plugin::Event::MouseRelease{localPos, btn});
            // try {
            //     MyApp::getInstance().releaseMouse(this);
            // } catch (abel::require_error) {
            // }
        }

        // return EventStatus::stop(EventStatus::TREE);
        return EventStatus::done();
    }

    return status;
}

EVENT_HANDLER_IMPL(PluginProxyWidget, abel::gui::MouseMove) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    // To avoid duplicates
    if (hitTest(event.pos0) ||
        hitTest(event.pos1) ||
        MyApp::getInstance().isMouseCaptured(this)) {

        pwidget->on_mouse_move(plugin::Event::MouseMove{event.pos0 - region.getPos(),
                                                        event.pos1 - region.getPos()});

        return EventStatus::done();
    }

    return status;
}

EVENT_HANDLER_IMPL(PluginProxyWidget, abel::gui::MouseScroll) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    if (hitTest(event.pos) ||
        MyApp::getInstance().isMouseCaptured(this)) {

        pwidget->on_scroll(plugin::Event::Scroll{Vector2d{0, (double)event.delta / 120.},
                                                 event.pos - region.getPos()});
    }

    return status;
}

plugin::Keyboard::Key convertKeyCode(abel::gui::Key key) {
    static std::map<abel::gui::Key, plugin::Keyboard::Key> dict{};

    if (dict.empty()) {
        dict[abel::gui::Key::A ] = plugin::Keyboard::Key::a   ;
        dict[abel::gui::Key::B ] = plugin::Keyboard::Key::b   ;
        dict[abel::gui::Key::C ] = plugin::Keyboard::Key::c   ;
        dict[abel::gui::Key::D ] = plugin::Keyboard::Key::d   ;
        dict[abel::gui::Key::E ] = plugin::Keyboard::Key::e   ;
        dict[abel::gui::Key::F ] = plugin::Keyboard::Key::f   ;
        dict[abel::gui::Key::G ] = plugin::Keyboard::Key::g   ;
        dict[abel::gui::Key::H ] = plugin::Keyboard::Key::h   ;
        dict[abel::gui::Key::I ] = plugin::Keyboard::Key::i   ;
        dict[abel::gui::Key::J ] = plugin::Keyboard::Key::j   ;
        dict[abel::gui::Key::K ] = plugin::Keyboard::Key::k   ;
        dict[abel::gui::Key::L ] = plugin::Keyboard::Key::l   ;
        dict[abel::gui::Key::M ] = plugin::Keyboard::Key::m   ;
        dict[abel::gui::Key::N ] = plugin::Keyboard::Key::n   ;
        dict[abel::gui::Key::O ] = plugin::Keyboard::Key::o   ;
        dict[abel::gui::Key::P ] = plugin::Keyboard::Key::p   ;
        dict[abel::gui::Key::Q ] = plugin::Keyboard::Key::q   ;
        dict[abel::gui::Key::R ] = plugin::Keyboard::Key::r   ;
        dict[abel::gui::Key::S ] = plugin::Keyboard::Key::s   ;
        dict[abel::gui::Key::T ] = plugin::Keyboard::Key::t   ;
        dict[abel::gui::Key::U ] = plugin::Keyboard::Key::u   ;
        dict[abel::gui::Key::V ] = plugin::Keyboard::Key::v   ;
        dict[abel::gui::Key::W ] = plugin::Keyboard::Key::w   ;
        dict[abel::gui::Key::X ] = plugin::Keyboard::Key::x   ;
        dict[abel::gui::Key::Y ] = plugin::Keyboard::Key::y   ;
        dict[abel::gui::Key::Z ] = plugin::Keyboard::Key::z   ;

        dict[abel::gui::Key::N0] = plugin::Keyboard::Key::num0;
        dict[abel::gui::Key::N1] = plugin::Keyboard::Key::num1;
        dict[abel::gui::Key::N2] = plugin::Keyboard::Key::num2;
        dict[abel::gui::Key::N3] = plugin::Keyboard::Key::num3;
        dict[abel::gui::Key::N4] = plugin::Keyboard::Key::num4;
        dict[abel::gui::Key::N5] = plugin::Keyboard::Key::num5;
        dict[abel::gui::Key::N6] = plugin::Keyboard::Key::num6;
        dict[abel::gui::Key::N7] = plugin::Keyboard::Key::num7;
        dict[abel::gui::Key::N8] = plugin::Keyboard::Key::num8;
        dict[abel::gui::Key::N9] = plugin::Keyboard::Key::num9;

        dict[abel::gui::Key::Numpad0] = plugin::Keyboard::Key::numpad0;
        dict[abel::gui::Key::Numpad1] = plugin::Keyboard::Key::numpad1;
        dict[abel::gui::Key::Numpad2] = plugin::Keyboard::Key::numpad2;
        dict[abel::gui::Key::Numpad3] = plugin::Keyboard::Key::numpad3;
        dict[abel::gui::Key::Numpad4] = plugin::Keyboard::Key::numpad4;
        dict[abel::gui::Key::Numpad5] = plugin::Keyboard::Key::numpad5;
        dict[abel::gui::Key::Numpad6] = plugin::Keyboard::Key::numpad6;
        dict[abel::gui::Key::Numpad7] = plugin::Keyboard::Key::numpad7;
        dict[abel::gui::Key::Numpad8] = plugin::Keyboard::Key::numpad8;
        dict[abel::gui::Key::Numpad9] = plugin::Keyboard::Key::numpad9;

        dict[abel::gui::Key::F1 ] = plugin::Keyboard::Key::f1 ;
        dict[abel::gui::Key::F2 ] = plugin::Keyboard::Key::f2 ;
        dict[abel::gui::Key::F3 ] = plugin::Keyboard::Key::f3 ;
        dict[abel::gui::Key::F4 ] = plugin::Keyboard::Key::f4 ;
        dict[abel::gui::Key::F5 ] = plugin::Keyboard::Key::f5 ;
        dict[abel::gui::Key::F6 ] = plugin::Keyboard::Key::f6 ;
        dict[abel::gui::Key::F7 ] = plugin::Keyboard::Key::f7 ;
        dict[abel::gui::Key::F8 ] = plugin::Keyboard::Key::f8 ;
        dict[abel::gui::Key::F9 ] = plugin::Keyboard::Key::f9 ;
        dict[abel::gui::Key::F10] = plugin::Keyboard::Key::f10;
        dict[abel::gui::Key::F11] = plugin::Keyboard::Key::f11;
        dict[abel::gui::Key::F12] = plugin::Keyboard::Key::f12;
        dict[abel::gui::Key::F13] = plugin::Keyboard::Key::f13;
        dict[abel::gui::Key::F14] = plugin::Keyboard::Key::f14;
        dict[abel::gui::Key::F15] = plugin::Keyboard::Key::f15;

        dict[abel::gui::Key::Escape     ] = plugin::Keyboard::Key::escape   ;
        dict[abel::gui::Key::Ctrl       ] = plugin::Keyboard::Key::lctrl    ;
        dict[abel::gui::Key::Shift      ] = plugin::Keyboard::Key::lshift   ;
        dict[abel::gui::Key::Alt        ] = plugin::Keyboard::Key::lalt     ;
        // dict[abel::gui::Key::           ] = plugin::Keyboard::Key::rctrl    ;
        // dict[abel::gui::Key::           ] = plugin::Keyboard::Key::rshift   ;
        // dict[abel::gui::Key::           ] = plugin::Keyboard::Key::ralt     ;
        dict[abel::gui::Key::LWin       ] = plugin::Keyboard::Key::lsuper   ;
        dict[abel::gui::Key::RWin       ] = plugin::Keyboard::Key::rsuper   ;
        // dict[abel::gui::Key::           ] = plugin::Keyboard::Key::menu     ;
        dict[abel::gui::Key::LBracket   ] = plugin::Keyboard::Key::lbracket ;
        dict[abel::gui::Key::RBracket   ] = plugin::Keyboard::Key::rbracket ;
        dict[abel::gui::Key::Semicolon  ] = plugin::Keyboard::Key::semicolon;
        dict[abel::gui::Key::Comma      ] = plugin::Keyboard::Key::comma    ;
        dict[abel::gui::Key::Period     ] = plugin::Keyboard::Key::period   ;
        dict[abel::gui::Key::Quote      ] = plugin::Keyboard::Key::quote    ;
        dict[abel::gui::Key::Slash      ] = plugin::Keyboard::Key::slash    ;
        dict[abel::gui::Key::Backslash  ] = plugin::Keyboard::Key::backslash;
        dict[abel::gui::Key::Tilde      ] = plugin::Keyboard::Key::tilde    ;
        dict[abel::gui::Key::Plus       ] = plugin::Keyboard::Key::equal    ;
        dict[abel::gui::Key::Minus      ] = plugin::Keyboard::Key::hyphen   ;
        dict[abel::gui::Key::Space      ] = plugin::Keyboard::Key::space    ;
        dict[abel::gui::Key::Return     ] = plugin::Keyboard::Key::enter    ;
        dict[abel::gui::Key::Backspace  ] = plugin::Keyboard::Key::backspace;
        dict[abel::gui::Key::Tab        ] = plugin::Keyboard::Key::tab      ;
        dict[abel::gui::Key::Pageup     ] = plugin::Keyboard::Key::pageup   ;
        dict[abel::gui::Key::Pagedown   ] = plugin::Keyboard::Key::pagedown ;
        dict[abel::gui::Key::End        ] = plugin::Keyboard::Key::end      ;
        dict[abel::gui::Key::Home       ] = plugin::Keyboard::Key::home     ;
        dict[abel::gui::Key::Insert     ] = plugin::Keyboard::Key::insert   ;
        dict[abel::gui::Key::Delete     ] = plugin::Keyboard::Key::del      ;
        dict[abel::gui::Key::Add        ] = plugin::Keyboard::Key::add      ;
        dict[abel::gui::Key::Subtract   ] = plugin::Keyboard::Key::subtract ;
        dict[abel::gui::Key::Multiply   ] = plugin::Keyboard::Key::multiply ;
        dict[abel::gui::Key::Divide     ] = plugin::Keyboard::Key::divide   ;
        dict[abel::gui::Key::Left       ] = plugin::Keyboard::Key::left     ;
        dict[abel::gui::Key::Right      ] = plugin::Keyboard::Key::right    ;
        dict[abel::gui::Key::Up         ] = plugin::Keyboard::Key::up       ;
        dict[abel::gui::Key::Down       ] = plugin::Keyboard::Key::down     ;
        dict[abel::gui::Key::Pause      ] = plugin::Keyboard::Key::pause    ;
    }

    if (dict.find(key) == dict.end()) {
        return plugin::Keyboard::Key::none;
    }

    return dict[key];
}

EVENT_HANDLER_IMPL(PluginProxyWidget, abel::gui::KeyPress) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    plugin::Keyboard::Key key = convertKeyCode(event.key);

    if (event.type == abel::gui::KeyPressType::Down) {
        pwidget->on_key_down(plugin::Event::KeyDown{key});
    } else {
        pwidget->on_key_up(plugin::Event::KeyUp{key});
    }

    return status;
}

EVENT_HANDLER_IMPL(PluginProxyWidget, abel::gui::KeyboardInput) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    // Screw the repeat count
    pwidget->on_text_enter(plugin::Event::TextEnter{(uint32_t)event.wchr});

    return status;
}

EVENT_HANDLER_IMPL(PluginProxyWidget, abel::gui::VisibilityUpdate) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    if (event.hidden) {
        pwidget->on_hide(plugin::Event::Hide{});
    } else {
        pwidget->on_show(plugin::Event::Show{});
    }

    return status;
}

PluginProxyWidget::pwidget_t *PluginProxyWidget::getPWidgetParent() {
    // TODO: Implement?
    return nullptr;
}

abel::gui::widgets::Label &PluginProxyWidget::getCaption() {
    if (!caption) {
        caption = new abel::gui::widgets::Label(this, region, "");
        addChild(caption);
    }

    assert(caption);
    return *caption;
}

void PluginProxyWidget::registerKnownChild(abel::gui::Widget *knownChild_) {
    REQUIRE(!knownChild && knownChild_);

    assert(children.isEmpty());
    knownChild = knownChild_;
    addChild(knownChild);
}

PluginProxyWidget::~PluginProxyWidget() {
    // Screw cleanup, it won't work half the times

    // if (owning) {
    //     delete[] pwidget;
    // }
}
#pragma endregion PluginProxy


#pragma region MyPWidget
MyPWidget::MyPWidget(PluginProxyWidget *proxyWidget_) :
    proxyWidget{proxyWidget_} {

    assert(proxyWidget);

    proxyWidget->setPWidget(this);
}

void MyPWidget::set_position(const plugin::Vec2f &position_) {
    proxyWidget->dispatchEvent(abel::gui::MoveEvent{Vector2d{position_} - proxyWidget->getRegion().getPos()});
}

void MyPWidget::set_size(const plugin::Vec2f &size_) {
    proxyWidget->getRegion().setDiag(Vector2d{size_});
}

plugin::WBody MyPWidget::get_body() {
    return proxyWidget->getRegion();
}

void MyPWidget::set_body(const plugin::WBody &body_) {
    set_position(body_.position);
    set_size(body_.size);
}

plugin::Widget *MyPWidget::get_parent() const {
    return proxyWidget->getPWidgetParent();
}

void MyPWidget::set_parent(plugin::Widget *parent_) {
    throw notimpl_error("Can't set parents");
}

plugin::RenderTarget *MyPWidget::get_texture() {
    return textureOverride;
}

void MyPWidget::set_texture(plugin::RenderTarget *texture_) {
    textureOverride = texture_;
}

bool MyPWidget::is_active() {
    // TODO: Maybe isHidden instead?
    return proxyWidget->testParentsHidden();
}

bool MyPWidget::is_inside(const plugin::Vec2f &pos) {
    return proxyWidget->hitTest(pos);
}

bool MyPWidget::add_child(plugin::Widget *child) {
    if (!child) {
        return false;
    }

    void *extra = child->get_extra_data(nullptr);

    if (!extra) {
        // TODO: Maybe owning, in fact
        proxyWidget->addChild(new PluginProxyWidget(nullptr, child->get_body(), child, false));
        return true;
    }

    PluginProxyWidget *ppw = static_cast<PluginProxyWidget *>(extra);
    if (!ppw->checkMagic()) {
        ERR("A plugin is trying to mess with get_extra_data");
        REQUIRE(false);
    }

    proxyWidget->addChild(ppw);

    return true;
}

void MyPWidget::set_to_delete() {
    proxyWidget->die();
}

bool MyPWidget::delete_child(plugin::Widget *child) {
    ERR("Child deletion attempted");
    return false;
}

bool MyPWidget::delete_from_parent() {
    ERR("Immediate deletion attempted");
    return false;
}

void MyPWidget::on_render          (const plugin::Event::Render          &event) {}
void MyPWidget::on_tick            (const plugin::Event::Tick            &event) {}
void MyPWidget::on_mouse_press     (const plugin::Event::MousePress      &event) {}
void MyPWidget::on_mouse_release   (const plugin::Event::MouseRelease    &event) {}
void MyPWidget::on_mouse_move      (const plugin::Event::MouseMove       &event) {}
void MyPWidget::on_key_down        (const plugin::Event::KeyDown         &event) {}
void MyPWidget::on_key_up          (const plugin::Event::KeyUp           &event) {}
void MyPWidget::on_text_enter      (const plugin::Event::TextEnter       &event) {}
void MyPWidget::on_scroll          (const plugin::Event::Scroll          &event) {}
void MyPWidget::on_hide            (const plugin::Event::Hide            &event) {}
void MyPWidget::on_show            (const plugin::Event::Show            &event) {}

void MyPWidget::hide() {
    proxyWidget->setHidden(true);
}

void MyPWidget::show() {
    proxyWidget->setHidden(false);
}

void MyPWidget::focus() {
    ERR("Widget asked for focus");
    // throw notimpl_error("Can't focus widgets");
}

void MyPWidget::set_caption(const char *text,
                            size_t /*font_size*/,
                            const plugin::Vec2f */*pos*/) {
    abel::gui::widgets::Label & label = proxyWidget->getCaption();
    label.setText(text);
}

void MyPWidget::set_base_color(const plugin::RGBA &color) {
    // Not supported
}

void *MyPWidget::get_extra_data(void *arg) {
    return proxyWidget;
}
#pragma endregion MyPWidget


#pragma region Collection
#pragma region MyPButton
MyPButton::MyPButton(PluginProxyWidget *proxyWidget_, const char *caption) :
    MyPWidget(proxyWidget_) {

    native_wt *btn = new native_wt(
        nullptr,
        Rect<double>::wh(Vector2d{}, proxyWidget->getRegion().getDiag()),
        caption
    );

    btn->sigClick += [this]() {
        auto &cb = get_handler();

        if (cb) {
            cb();
        }

        return false;
    };

    proxyWidget->registerKnownChild(btn);
}

void MyPButton::set_handler(const HandlerType &handler_) {
    handler = handler_;
}

MyPButton::HandlerType &MyPButton::get_handler() {
    return handler;
}

void MyPButton::set_caption(const char *text, size_t font_size, const plugin::Vec2f *pos) {
    native_wt *btn = proxyWidget->getKnownChild<native_wt>();

    assert(btn);

    btn->getLabel().setText(text);
}
#pragma endregion MyPButton

#pragma region MyPColorPicker
MyPColorPicker::MyPColorPicker(PluginProxyWidget *proxyWidget_) :
    MyPWidget(proxyWidget_) {

    native_wt *cp = new native_wt(
        nullptr,
        Rect<double>::wh(Vector2d{}, proxyWidget->getRegion().getDiag()),
        false  // To make it local, i.e. not linked to the app's toolMgr
    );

    cp->sigChanged += [this](const Color &color, float alpha) {
        auto &cb = get_handler();

        if (cb) {
            cb(color.pack(alpha));
        }

        return false;
    };

    proxyWidget->registerKnownChild(cp);
}

void MyPColorPicker::set_handler(const HandlerType &handler_) {
    handler = handler_;
}

MyPColorPicker::HandlerType &MyPColorPicker::get_handler() {
    return handler;
}

plugin::RGBA MyPColorPicker::get_color() {
    native_wt *cp = proxyWidget->getKnownChild<native_wt>();

    assert(cp);

    return cp->getColor().pack((float)cp->getAlpha());
}

void MyPColorPicker::set_color(plugin::RGBA color) {
    native_wt *cp = proxyWidget->getKnownChild<native_wt>();

    assert(cp);

    cp->setColor(Color{color});
    cp->setAlpha((double)color.a / 255.);
}
#pragma endregion MyPColorPicker

#pragma region MyPLabel
MyPLabel::MyPLabel(PluginProxyWidget *proxyWidget_, const char *text) :
    MyPWidget(proxyWidget_) {

    native_wt *label = new native_wt(
        nullptr,
        Rect<double>::wh(Vector2d{}, proxyWidget->getRegion().getDiag()),
        text
    );

    proxyWidget->registerKnownChild(label);
}

std::string_view MyPLabel::get_text() const {
    native_wt *label = proxyWidget->getKnownChild<native_wt>();

    assert(label);

    return label->getText();
}

void MyPLabel::set_text(std::string_view text) {
    native_wt *label = proxyWidget->getKnownChild<native_wt>();

    assert(label);

    // A temporary copy
    std::string tmp{text};
    label->setText(tmp.data());
}

void MyPLabel::set_caption(const char *text, size_t font_size, const plugin::Vec2f *pos) {
    return set_text(text);
}
#pragma endregion MyPLabel

#pragma region MyPSlider
MyPSlider::MyPSlider(PluginProxyWidget *proxyWidget_, Type type_) :
    MyPWidget(proxyWidget_), type{type_} {

    switch (type) {
    case Type::X: {
        native_h_wt *slider = new native_h_wt(
            nullptr,
            Rect<double>::wh(Vector2d{}, proxyWidget->getRegion().getDiag()),
            0, 1
        );

        slider->sigChanged += [this](Vector2d value) {
            auto &cb = get_handler();

            if (cb) {
                cb(value);
            }

            return false;
        };

        proxyWidget->registerKnownChild(slider);
    } break;

    case Type::Y: {
        native_v_wt *slider = new native_v_wt(
            nullptr,
            Rect<double>::wh(Vector2d{}, proxyWidget->getRegion().getDiag()),
            0, 1
        );

        slider->sigChanged += [this](Vector2d value) {
            auto &cb = get_handler();

            if (cb) {
                cb(value);
            }

            return false;
        };

        proxyWidget->registerKnownChild(slider);
    } break;

    case Type::D2: {
        native_2d_wt *slider = new native_2d_wt(
            nullptr,
            Rect<double>::wh(Vector2d{}, proxyWidget->getRegion().getDiag()),
            Rect<double>::se(0, 0, 1, 1)
        );

        slider->sigChanged += [this](Vector2d value) {
            auto &cb = get_handler();

            if (cb) {
                cb(value);
            }

            return false;
        };

        proxyWidget->registerKnownChild(slider);
    } break;

    NODEFAULT
    }
}

void MyPSlider::set_handler(const HandlerType &handler_) {
    handler = handler_;
}

MyPSlider::HandlerType &MyPSlider::get_handler() {
    return handler;
}

plugin::Vec2f MyPSlider::get_fraction_2d() {
    switch (type) {
    case Type::X: {
        native_h_wt *slider = proxyWidget->getKnownChild<native_h_wt>();

        assert(slider);

        return Vector2d{slider->getValue(), 0};
    } break;

    case Type::Y: {
        native_v_wt *slider = proxyWidget->getKnownChild<native_v_wt>();

        assert(slider);

        return Vector2d{0, slider->getValue()};
    } break;

    case Type::D2: {
        native_2d_wt *slider = proxyWidget->getKnownChild<native_2d_wt>();

        assert(slider);

        return slider->getValue();
    } break;

    NODEFAULT
    }
}

void MyPSlider::set_fraction_2d(const plugin::Vec2f &frac) {
    switch (type) {
    case Type::X: {
        native_h_wt *slider = proxyWidget->getKnownChild<native_h_wt>();

        assert(slider);

        return slider->setValue(frac.x);
    } break;

    case Type::Y: {
        native_v_wt *slider = proxyWidget->getKnownChild<native_v_wt>();

        assert(slider);

        return slider->setValue(frac.y);
    } break;

    case Type::D2: {
        native_2d_wt *slider = proxyWidget->getKnownChild<native_2d_wt>();

        assert(slider);

        return slider->setValue(frac);
    } break;

    NODEFAULT
    }
}

float MyPSlider::get_fraction() {
    plugin::Vec2f val = get_fraction_2d();

    return (val.x + val.y) / 2;
}

void MyPSlider::set_fraction(float frac) {
    set_fraction_2d(plugin::Vec2f{frac, frac});
}
#pragma endregion MyPSlider

#pragma region MyPTextField
MyPTextField::MyPTextField(PluginProxyWidget *proxyWidget_) :
    MyPWidget(proxyWidget_) {

    native_wt *field = new native_wt(
        nullptr,
        Rect<double>::wh(Vector2d{}, proxyWidget->getRegion().getDiag()),
        ""
    );

    proxyWidget->registerKnownChild(field);
}

void MyPTextField::set_handler(const HandlerType &handler_) {
    handler = handler_;
}

MyPTextField::HandlerType &MyPTextField::get_handler() {
    return handler;
}

std::string_view MyPTextField::get_text() const {
    native_wt *field = proxyWidget->getKnownChild<native_wt>();

    assert(field);

    return field->getText();
}

void MyPTextField::set_text(std::string_view text) {
    native_wt *field = proxyWidget->getKnownChild<native_wt>();

    assert(field);

    // A temporary copy
    std::string tmp{text};
    field->setText(tmp.data());
}
#pragma endregion MyPTextField

#pragma region MyPWindow
MyPWindow::MyPWindow(PluginProxyWidget *proxyWidget_, const char *name) :
    MyPWidget(proxyWidget_) {

    set_name(name);
}

void MyPWindow::set_show_handler(const HandlerType &handler_) {
    showHandler = handler_;
}

MyPWindow::HandlerType &MyPWindow::get_show_handler() {
    return showHandler;
}

void MyPWindow::set_hide_handler(const HandlerType &handler_) {
    hideHandler = handler_;
}

MyPWindow::HandlerType &MyPWindow::get_hide_handler() {
    return hideHandler;
}

bool MyPWindow::set_name(const char *name) {
    auto *wnd = getTrueWindow();

    if (!wnd) {
        return false;
    }

    wnd->getTitleLabel().setText(name);

    return true;
}

const char *MyPWindow::get_name() {
    auto *wnd = getTrueWindow();

    if (!wnd) {
        return nullptr;
    }

    return wnd->getTitleLabel().getText();
}

void MyPWindow::on_hide(const plugin::Event::Hide &) {
    auto &cb = get_hide_handler();

    if (cb) {
        cb();
    }
}

void MyPWindow::on_show(const plugin::Event::Show &) {
    auto &cb = get_show_handler();

    if (cb) {
        cb();
    }
}

void MyPWindow::hide() {
    auto *wnd = getTrueWindow();

    if (wnd) {
        wnd->setHidden(true);
    }
}

void MyPWindow::show() {
    auto *wnd = getTrueWindow();

    if (wnd) {
        wnd->setHidden(false);
    }
}

void MyPWindow::focus() {
    auto *wnd = getTrueWindow();

    if (wnd) {
        wnd->bringToFront();
    }
}

abel::gui::widgets::Window *MyPWindow::getTrueWindow() {
    return dynamic_cast<abel::gui::widgets::Window *>(proxyWidget->getParent());
}
#pragma endregion MyPWindow
#pragma endregion Collection


#endif