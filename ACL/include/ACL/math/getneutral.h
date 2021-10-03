#ifndef ABEL_MATH_GETNEUTRAL_H
#define ABEL_MATH_GETNEUTRAL_H

#include <ACL/general.h>
#include <ACL/type_traits.h>


namespace abel {
namespace math {


/// Overload this for your type
template <typename T>
T getNeutral() {
    static_assert(std::is_arithmetic_v<T>);

    return (T)1;
}


}
}


#endif // ABEL_MATH_GETNEUTRAL_H
