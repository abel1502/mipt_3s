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
// TODO: Implement all of these


EVENT_CLS_DECL_(Render) {
    EVENT_CLS_DEMANDS_MODIFICATION_

    Rect<double> region;  // Each widget, when forwarding this event to its children,
                          // should reset the region to be its own, so that they get to have relative addresses
    Texture &target;

    constexpr EVENT_CLS_NAME(Render)(const Rect<double> &region_, Texture &target_) :
        region{region_}, target{target_} {}

    constexpr EVENT_CLS_NAME(Render) createSubEvent(const Rect<double> &subRegion_) const {
        EVENT_CLS_NAME(Render) subEvent{*this};

        subEvent.region = region.relRect(subRegion_, true);  // TODO: Maybe the other way?

        return subEvent;
    }
};

EVENT_CLS_DECL_(Move) {
    // TODO: EVENT_CLS_DEMANDS_MODIFICATION_
};

/*/// May involve movement as well
EVENT_CLS_DECL_(Resize) {
    EVENT_CLS_DEMANDS_MODIFICATION_

    Rect<double> newRegion;  // This refers to the new size of the widget that gets this event

    constexpr EVENT_CLS_NAME(Resize)(const Rect<double> &newRegion_) :
        newRegion{newRegion_} {}

    constexpr EVENT_CLS_NAME(Resize) createSubEvent(const Rect<double> &subNewRegion_) const {
        EVENT_CLS_NAME(Resize) subEvent{*this};

        subEvent.newRegion = subNewRegion_;

        return subEvent;
    }
};*/

EVENT_CLS_DECL_(FocusUpdate) {};

EVENT_CLS_DECL_(Start) {};

EVENT_CLS_DECL_(Exit) {};

EVENT_CLS_DECL_(MouseClick) {
    EVENT_CLS_DEMANDS_MODIFICATION_

    Vector2d pos;
    MouseAttrs attrs;
    MouseBtn button;
    MouseClickType type;

    constexpr EVENT_CLS_NAME(MouseClick)(const Vector2d &pos_, const MouseAttrs &attrs_, MouseBtn button_, MouseClickType type_) :
        pos{pos_}, attrs{attrs_}, button{button_}, type{type_} {}

    constexpr EVENT_CLS_NAME(MouseClick) createSubEvent(const Rect<double> &region_) const {
        EVENT_CLS_NAME(MouseClick) subEvent{*this};

        subEvent.pos -= region_.getStart();

        return subEvent;
    }
};

EVENT_CLS_DECL_(MouseMove) {
    EVENT_CLS_DEMANDS_MODIFICATION_

    Vector2d pos0;
    Vector2d pos1;
    MouseAttrs attrs;


    constexpr EVENT_CLS_NAME(MouseMove)(const Vector2d &pos0_, const Vector2d &pos1_, const MouseAttrs &attrs_) :
        pos0{pos0_}, pos1{pos1_}, attrs{attrs_} {}

    constexpr EVENT_CLS_NAME(MouseMove) createSubEvent(const Rect<double> &region_) const {
        EVENT_CLS_NAME(MouseMove) subEvent{*this};

        subEvent.pos0 -= region_.getStart();
        subEvent.pos1 -= region_.getStart();

        return subEvent;
    }

};

EVENT_CLS_DECL_(Keyboard) {};

// TODO: Use type erasure here to allow the users to customize this
EVENT_CLS_DECL_(User) {};


}

#undef EVENT_CLS_DEMANDS_MODIFICATION_
#undef EVENT_CLS_DECL_

#endif // AGF_EVENTS_H
