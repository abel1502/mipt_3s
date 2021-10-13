#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <set>
#include <vector>


using value_t = unsigned long long;


inline bool areCoprime(value_t a, value_t b) {
    if (!((a | b) & 1))
        return false;

    while (a) {
        while (!(a & 1))  a >>= 1;

        if (b >= a)  b -= a;

        std::swap(a, b);
    }

    return b == 1;
}


inline value_t readll() {
    static constexpr unsigned SIZE = 1000;
    static char buf[SIZE + 1] = "";

    fgets(buf, SIZE, stdin);

    value_t res = 0;
    char *s = buf;
    while (*s >= '0' && *s <= '9') {
        res = res * 10 + *s++ - '0';
    }

    return res;
}

inline unsigned readu() {
    static constexpr unsigned SIZE = 1000;
    static char buf[SIZE + 1] = "";

    fgets(buf, SIZE, stdin);

    unsigned res = 0;
    char *s = buf;
    while (*s >= '0' && *s <= '9') {
        res = res * 10 + *s++ - '0';
    }

    return res;
}


int main() {
    unsigned n = 0;
    unsigned cntCoprime = 0;
    value_t last = 0,
            cur  = 0;

    n = readu();
    last = readll();
    for (unsigned i = 1; i < n; ++i) {
        cur = readll();

        cntCoprime += areCoprime(last, cur);

        last = cur;
    }

    printf("%u\n", n - cntCoprime - 1);
}
