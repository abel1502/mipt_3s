#include <AGF/widget_event.h>
#include <ACL/general.h>
#include <AGF/llgui_pre.h>


#define DECL_EVENT_CLS_(EVENT_NAME) \
    struct EVENT_NAME##Event : public WidgetEvent


namespace abel::gui::inline events {
// TODO: Implement all of these


DECL_EVENT_CLS_(Render) {
    Rect<double> region;
    Texture &target;
};

DECL_EVENT_CLS_(Move) {};

/// May involve movement as well
DECL_EVENT_CLS_(Resize) {
    Rect<double> newRegion;
};

DECL_EVENT_CLS_(ActiveStatusUpdate) {};

DECL_EVENT_CLS_(Start) {};

DECL_EVENT_CLS_(Exit) {};

DECL_EVENT_CLS_(MouseClick) {};

DECL_EVENT_CLS_(MouseMove) {};

DECL_EVENT_CLS_(Keyboard) {};

// TODO: Use type erasure here to allow the users to customize this
DECL_EVENT_CLS_(User) {};


}

#undef DECL_EVENT_CLS_

