#ifndef AGF_EVENTS_H
#define AGF_EVENTS_H

#include <AGF/widget_event.h>
#include <ACL/general.h>
#include <AGF/llgui_pre.h>
#include <AGF/mouse.h>


#define EVENT_CLS_NAME(EVENT_NAME) \
    EVENT_NAME##Event

#define EVENT_CLS_DECL_(EVENT_NAME) \
    struct EVENT_CLS_NAME(EVENT_NAME) : public WidgetEvent

#define EVENT_CLS_DEMANDS_MODIFICATION_ \
    static constexpr bool demands_modification = true;


namespace abel::gui {


EVENT_CLS_DECL_(Render) {
    EVENT_CLS_DEMANDS_MODIFICATION_

    Rect<double> region;  // The (suggested) limiting bounds, by default, the borders of the parent widget
    Texture &target;


    constexpr RenderEvent(const Rect<double> &region_, Texture &target_) noexcept :
        region{region_}, target{target_} {}

    constexpr RenderEvent createSubEvent(const Rect<double> &region_) const noexcept {
        RenderEvent subEvent{*this};

        subEvent.region = region_;

        return subEvent;
    }
};

EVENT_CLS_DECL_(Move) {
    Vector2d delta;


    constexpr MoveEvent(const Vector2d &delta_) noexcept :
        delta{delta_} {}
};

/*/// May involve movement as well
ResizeEvent {
    EVENT_CLS_DEMANDS_MODIFICATION_

    Rect<double> newRegion;  // This refers to the new size of the widget that gets this event


    constexpr ResizeEvent(const Rect<double> &newRegion_) :
        newRegion{newRegion_} {}

    constexpr ResizeEvent createSubEvent(const Rect<double> &subNewRegion_) const {
        ResizeEvent subEvent{*this};

        subEvent.newRegion = subNewRegion_;

        return subEvent;
    }
};*/

EVENT_CLS_DECL_(FocusUpdate) {
    bool focus;


    constexpr FocusUpdateEvent(bool focus_) noexcept :
        focus{focus_} {}
};

EVENT_CLS_DECL_(Start) {};

EVENT_CLS_DECL_(Exit) {};

EVENT_CLS_DECL_(MouseClick) {
    Vector2d pos;
    MouseAttrs attrs;
    MouseBtn button;
    MouseClickType type;


    constexpr MouseClickEvent(const Vector2d &pos_, const MouseAttrs &attrs_,
                              MouseBtn button_, MouseClickType type_) noexcept :
        pos{pos_}, attrs{attrs_}, button{button_}, type{type_} {}
};

EVENT_CLS_DECL_(MouseMove) {
    Vector2d pos0;
    Vector2d pos1;
    MouseAttrs attrs;


    constexpr MouseMoveEvent(const Vector2d &pos0_, const Vector2d &pos1_,
                             const MouseAttrs &attrs_) noexcept :
        pos0{pos0_}, pos1{pos1_}, attrs{attrs_} {}

    constexpr Vector2d getDelta() const noexcept {
        return pos1 - pos0;
    }
};

EVENT_CLS_DECL_(KeyPress) {};

EVENT_CLS_DECL_(KeyboardInput) {};

// TODO: Use type erasure here to allow the users to customize this
EVENT_CLS_DECL_(User) {};


}

#undef EVENT_CLS_DEMANDS_MODIFICATION_
#undef EVENT_CLS_DECL_

#endif // AGF_EVENTS_H
