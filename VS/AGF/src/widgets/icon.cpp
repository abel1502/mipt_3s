#include <AGF/llgui.h>
#include <ACL/general.h>
#include <AGF/widgets/icon.h>
#include <AGF/application.h>
#include <cassert>


namespace abel::gui::widgets {


Icon::Icon(Widget *parent_, const Rect<double> &region_) :
    Base(parent_, region_) {}

void Icon::setTexture(const Texture &source) {
    if (!hasTexture()) {
        texture = new Texture((unsigned)region.w(), (unsigned)region.h());
    }

    texture->embed(texture->getRect(), source);
}

void Icon::resetTexture() {
    texture = nullptr;
}

EVENT_HANDLER_IMPL(Icon, Render) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    if (!hasTexture()) {
        return EventStatus::skip();
    }

    event.target.embed(region, *texture);

    return EventStatus::done();
}

}
