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


namespace abel::gui {
// TODO: Implement all of these



EVENT_CLS_DECL_(Render) {
    Rect<double> region;  // Each widget, when forwarding this event to its children,
                          // should reset the region to be its own, so that they get to have relative addresses
    Texture &target;

    constexpr EVENT_CLS_NAME(Render)(const Rect<double> &region_, Texture &target_) :
        region{region_}, target{target_} {}

    constexpr EVENT_CLS_NAME(Render) createSubEvent(const Rect<double> &subRegion_) {
        EVENT_CLS_NAME(Render) subEvent{*this};

        subEvent.region = subRegion_;

        return subEvent;
    }
};

EVENT_CLS_DECL_(Move) {};

/// May involve movement as well
EVENT_CLS_DECL_(Resize) {
    Rect<double> newRegion;  // This refers to the new size of the widget that gets this event

    constexpr EVENT_CLS_NAME(Resize)(const Rect<double> &newRegion_) :
        newRegion{newRegion_} {}

    constexpr EVENT_CLS_NAME(Resize) createSubEvent(const Rect<double> &subNewRegion_) {
        EVENT_CLS_NAME(Resize) subEvent{*this};

        subEvent.newRegion = subNewRegion_;

        return subEvent;
    }
};

EVENT_CLS_DECL_(ActiveStatusUpdate) {};

EVENT_CLS_DECL_(Start) {};

EVENT_CLS_DECL_(Exit) {};

EVENT_CLS_DECL_(MouseClick) {
    Vector2d pos;
    MouseAttrs attrs;
    MouseBtn button;
    MouseClickType type;

    constexpr EVENT_CLS_NAME(MouseClick)(const Vector2d &pos_, const MouseAttrs &attrs_, MouseBtn button_, MouseClickType type_) :
        pos{pos_}, attrs{attrs_}, button{button_}, type{type_} {}

    constexpr EVENT_CLS_NAME(MouseClick) createSubEvent(const Vector2d &pos_) {
        EVENT_CLS_NAME(MouseClick) subEvent{*this};

        subEvent.pos = pos_;

        return subEvent;
    }
};

EVENT_CLS_DECL_(MouseMove) {
    Vector2d pos0;
    Vector2d pos1;
    MouseAttrs attrs;
};

EVENT_CLS_DECL_(Keyboard) {};

// TODO: Use type erasure here to allow the users to customize this
EVENT_CLS_DECL_(User) {};


}

#undef EVENT_CLS_DECL_

#endif // AGF_EVENTS_H
