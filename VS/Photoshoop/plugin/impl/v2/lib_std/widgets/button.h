#pragma once


#include "../widget_std.h"


namespace plugin {

class Button : virtual public Widget {
public:
    using HandlerType = std::function<void()>;

    virtual void set_handler(const HandlerType &handler_) = 0;
    virtual HandlerType &get_handler() = 0;
};

}
