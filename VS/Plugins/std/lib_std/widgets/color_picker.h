#include "../widget_std.h"


namespace PUPPY {

class ColorPicker : virtual public Widget {
public:
    using HandlerType = std::function<void(RGBA)>;

    virtual void set_handler(const HandlerType &handler_) = 0;
    virtual HandlerType &get_handler() = 0;

    virtual RGBA get_color() = 0;
    virtual void set_color(RGBA color) = 0;
};

}
