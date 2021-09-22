#ifndef ABEL_MATH_BINPOW_H
#define ABEL_MATH_BINPOW_H

#include <ACL/general.h>
#include <ACL/math/getneutral.h>


namespace abel {
namespace math {


template <typename T>
T binPow(T base, unsigned power) {
    T result = getNeutral<T>();

    while (power) {
        if (!(power & 1)) {
            base = base * base;
            power >>= 1;
        } else {
            result *= base;
            --power;
        }
    }

    return result;
}


}
}


#endif // ABEL_MATH_BINPOW_H
