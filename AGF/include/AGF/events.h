#include <AGF/widget_event.h>
#include <ACL/general.h>


namespace abel::gui::inline events {


// TODO: Implement all of these


class RenderEvent : public WidgetEvent {};

class MouseClickEvent : public WidgetEvent {};

class MouseMoveEvent : public WidgetEvent {};

class KeyboardEvent : public WidgetEvent {};

class ActiveStatusUpdateEvent : public WidgetEvent {};

class ExitEvent : public WidgetEvent {};

// TODO: Use type erasure here to allow the users to customize this
class UserEvent : public WidgetEvent {};


}
