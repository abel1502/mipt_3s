#include "../widget_std.h"
#include <string>


namespace PUPPY {

class TextField : virtual public Widget {
public:
    using HandlerType = std::function<void(std::string_view)>;

    virtual void set_handler(const HandlerType &handler_) = 0;
    virtual HandlerType &get_handler() = 0;

    virtual std::string_view get_text() const = 0;
    virtual void set_text(std::string_view text) = 0;
};

}
