#ifndef ABEL_MATH_GETNEUTRAL_H
#define ABEL_MATH_GETNEUTRAL_H

#include <ACL/general.h>
#include <type_traits>


namespace abel {
namespace math {


/// Overload this for your type
template <typename T>
T getNeutral() {
    static_assert(std::is_arithmetic<T>::value);

    return (T)1;
}


}
}


#endif // ABEL_MATH_GETNEUTRAL_H
