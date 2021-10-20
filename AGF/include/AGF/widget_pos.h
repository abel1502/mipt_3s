#ifndef AGF_WIDGET_POS_H
#define AGF_WIDGET_POS_H


#include <ACL/gui/rect.h>
#include <ACL/gui/coords.h>
#include <ACL/math/vector.h>
#include <ACL/type_traits.h>


namespace abel::gui {

using abel::math::Vector2;
using abel::math::Vector2d;
using abel::math::Vector2i;


// TODO: Finish


#if 0
template <typename T>
class WidgetPos {
    static_assert(std::is_signed<T>::value);

public:
    using type = T;

    enum Anchor : unsigned {
        A_TOP    = 0b0001;
        A_LEFT   = 0b0010;
        A_BOTTOM = 0b0100;
        A_RIGHT  = 0b1000;
    };

    struct AbsPos {
        unsigned anchors = A_TOP | A_LEFT;

        union {
            T offsets[4];

            struct {
                T top    = 0;
                T left   = 0;
                T bottom = 0;
                T right  = 0;
            };
        };
    };

    struct RelPos {
        //
    };

protected:
    //

};
#endif


}


#endif // AGF_WIDGET_POS_H
