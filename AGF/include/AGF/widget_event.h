#ifndef AGF_WIDGET_EVENT_H
#define AGF_WIDGET_EVENT_H

#include <ACL/general.h>


namespace abel::gui {


class WidgetEvent {
public:
    static constexpr bool demands_modification = false;
    // TODO: ?

protected:
    WidgetEvent() = default;
};


}


#endif // AGF_WIDGET_EVENT_H
