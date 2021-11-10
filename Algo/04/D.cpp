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


// I might as well try this olympic encapsulation, I guess.
// It seems easier than hoarding stuff in main()
struct Task {
    static constexpr value_t MAX_VAL = 100'000;
    static constexpr value_t MAX_RESULT = 10'000'000;  // Just in case
    static std::vector<value_t> minDivisors;


    unsigned n = 0;
    unsigned idx = 0;
    std::vector<value_t> arr{};
    std::vector<bool> usedPrimes = std::vector<bool>(MAX_RESULT + 1, false);


    void read() {
        DBG("Reading");

        std::cin >> n;

        arr.resize(n);
        for (unsigned i = 0; i < n; ++i) {
            std::cin >> arr[i];
        }
    }

    value_t findClosestFree(value_t val) {
        while (!isFree(val))
            ++val;

        markUsed(val);
        return val;
    }

    bool isFree(value_t val) const {
        while (val != 1) {
            if (usedPrimes[minDivisors[val]])
                return false;

            val /= minDivisors[val];
        }

        return true;
    }

    void markUsed(value_t val) {
        while (val != 1) {
            usedPrimes[minDivisors[val]] = true;
            val /= minDivisors[val];
        }
    }

    void printFreePrimes() {
        for (idx = 0; idx < n && isFree(arr[idx]); markUsed(arr[idx++])) {
            std::cout << arr[idx] << " ";
        }

        if (idx == n) {
            std::cout << "\n";
            return;
        }

        arr[idx] = findClosestFree(arr[idx]);
        markUsed(arr[idx]);
        std::cout << arr[idx++] << " ";

        value_t p = 2;

        for (; idx < n; ++idx) {
            while (minDivisors[p] < p || usedPrimes[p])
                ++p;

            std::cout << p++ << " ";
        }

        std::cout << "\n";
    }

    void solve() {
        read();
        printFreePrimes();
    }
};

std::vector<value_t> Task::minDivisors = sieveMinDivisors(MAX_RESULT);



int main() {
    Task task{};

    task.solve();

    return 0;
}




