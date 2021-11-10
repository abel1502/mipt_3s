#include <cstdio>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>


using value_t = long long;

constexpr value_t MOD = 1'000'000'007ull;


// std::swap isn't constexpr on codeforces for some reason
constexpr void swap(value_t &a, value_t &b) {
    value_t tmp = a;
    a = b;
    b = tmp;
}

constexpr value_t modulo(value_t val) {
    return ((val % MOD) + MOD) % MOD;
}

constexpr value_t modulo(value_t val, value_t mod) {
    return (val % mod + mod) % mod;
}

constexpr value_t binPow(value_t base, value_t power) {
    unsigned long long result = 1;

    while (power) {
        if (power & 1) {
            result = modulo(result * base);
            --power;
        }

        base = modulo(base * base);
        power >>= 1;
    }

    return result;
}

constexpr value_t inv(value_t num) {
    return binPow(num, MOD - 2);
}

constexpr bool areCoprime(value_t a, value_t b) {
    if (!((a | b) & 1))
        return false;

    while (a) {
        a >>= __builtin_ctzll(a);

        if (b >= a)  b -= a;

        swap(a, b);
    }

    return b == 1;
}

constexpr value_t gcd(value_t a, value_t b) {
    unsigned ctz = std::min(__builtin_ctzll(a), __builtin_ctzll(b));

    a >>= ctz;
    b >>= ctz;

    while (a) {
        a >>= __builtin_ctzll(a);

        if (b >= a)  b -= a;

        swap(a, b);
    }

    return b << ctz;
}


int main() {
    value_t a = 0,
            b = 0,
            c = 0,
            d = 0;

    std::cin >> a >> b >> c >> d;

    value_t tmp = modulo(modulo(a * d) +
                         modulo(b * c));

    tmp = modulo(tmp * inv(modulo(b * d)));

    std::cout << tmp << "\n";

    return 0;
}
