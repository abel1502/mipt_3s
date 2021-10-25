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
    Rect<double> region;
    Texture &target;

    constexpr EVENT_CLS_NAME(Render)(const Rect<double> &region_, Texture &target_) :
        region{region_}, target{target_} {}
};

EVENT_CLS_DECL_(Move) {};

/// May involve movement as well
EVENT_CLS_DECL_(Resize) {
    Rect<double> newRegion;
};

EVENT_CLS_DECL_(ActiveStatusUpdate) {};

EVENT_CLS_DECL_(Start) {};

EVENT_CLS_DECL_(Exit) {};

EVENT_CLS_DECL_(MouseClick) {
    Vector2d pos;
    MouseAttrs attrs;
    MouseBtn button;
    MouseClickType type;
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
