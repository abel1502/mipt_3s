#include <cstdio>
#include <cstdarg>
#include <cerrno>

#include <ACL/general.h>


namespace abel {


int verbosity = 0;

void dbg_(bool isError, int level, const char *funcName, int lineNo, const char *msg, ...) {
    va_list args = {};
    va_start(args, msg);

    if (verbosity >= level) {
        fprintf(stderr, "[%s in %s() on #%d] ", isError ? "ERROR" : "DBG", funcName, lineNo);
        vfprintf(stderr, msg, args);
        fprintf(stderr, "\n");
        if (errno != 0 && isError) {
            perror("System error");
        }
    }

    va_end(args);
}

unsigned long long randLL() {
    static unsigned long long rnd = 123;

    rnd ^= rnd << 21;
    rnd ^= rnd >> 35;
    rnd ^= rnd << 4;

    return rnd;
}


}


#include "ACL/math/vector.h"

abel::math::Vector2i vec1 = abel::math::Vector2i::ZERO + abel::math::Vector2i::ZERO;
abel::math::Vector2i vec2(1, 2);
abel::math::Vector2i vec3 = -vec2;
