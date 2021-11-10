#include <cstdio>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <new>
#include <type_traits>
#include <utility>
#include <valarray>


#if 0
#define DBG(FMT, ...) \
    printf("[%s#%d: " FMT "]\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define DBG(FMT, ...)
#endif


using value_t = long long;

constexpr value_t MOD = 1'000'000'007ll;


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

std::vector<value_t> sieveMinDivisors(value_t cnt) {
    ++cnt;

    std::vector<value_t> result(cnt, 1);

    value_t limit = sqrt(cnt);
    for (value_t cur = 2; cur <= limit; ++cur) {
        if (result[cur] > 1)
            continue;

        for (value_t next = cur; next < cnt; next += cur) {
            if (result[next] == 1)
                result[next] = cur;
        }
    }

    for (value_t cur = limit; cur < cnt; ++cur) {
        if (result[cur] == 1)
            result[cur] = cur;
    }

    return result;
}


struct Task {
    unsigned n = 0, k = 0;
    std::vector<value_t> arr{};
    std::vector<value_t> factorials{};


    void read() {
        DBG("Reading");

        std::cin >> n >> k;

        arr.resize(n);
        for (unsigned i = 0; i < n; ++i) {
            std::cin >> arr[i];
        }
    }

    void calcFactorials() {
        factorials.assign(n + 1, 1);

        for (unsigned i = 2; i < factorials.size(); ++i) {
            factorials[i] = modulo(factorials[i - 1] * i);
        }
    }

    value_t calcCnk(value_t n_, value_t k_) const {
        return modulo(modulo(factorials[n_] * inv(factorials[k_])) * inv(factorials[n_ - k_]));
    }

    value_t calcStirling(value_t n_, value_t k_) const {
        value_t ans = 0;

        for (unsigned i = 0; i < k_; ++i) {
            ans = modulo(ans + (i % 2 ? -1 : 1) * modulo(calcCnk(k_, i) * binPow(k_ - i, n_)));
        }

        return modulo(ans * inv(factorials[k_]));
    }

    value_t calcTotalWt() const {
        value_t wtAll = 0;
        for (value_t val : arr) {
            wtAll = modulo(wtAll + val);
        }

        return modulo(wtAll * modulo(calcStirling(n, k) + modulo((n - 1) * calcStirling(n - 1, k))));
    }

    void solve() {
        read();
        calcFactorials();
        std::cout << calcTotalWt() << "\n";
    }
};



int main() {
    Task task{};

    task.solve();

    return 0;
}

