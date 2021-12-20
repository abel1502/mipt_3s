#include "../widget_std.h"


namespace PUPPY {

class Window : virtual public Widget {
public:
    using HandlerType = std::function<void()>;

    virtual void set_show_handler(HandlerType &handler_) = 0;
    virtual HandlerType &get_show_handler() = 0;

    virtual void set_hide_handler(HandlerType &handler_) = 0;
    virtual HandlerType &get_hide_handler() = 0;

    virtual bool set_name(const char *name) = 0;
    virtual const char *get_name() = 0; // name returned is only valid for a "short time" (but not less then till the end of function
                                        // in wich called
};

}
