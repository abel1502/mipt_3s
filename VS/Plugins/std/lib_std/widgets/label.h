#include "../widget_std.h"
#include <string>


namespace PUPPY {

class Label : virtual public Widget {
public:
    virtual std::string_view get_text() const = 0;
    virtual void set_text(std::string_view text) = 0;
};

}
