#include <cstdio>
//#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <new>
#include <type_traits>
#include <utility>
#include <array>


#if 0
#define DBG(FMT, ...) \
    printf("[%s#%d: " FMT "]\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define DBG(FMT, ...)
#endif

#define assert(STMT) \
    if (!(STMT)) abort();


using value_t = long long;

constexpr value_t MOD = 1'000'000'007ll;


std::vector<bool> sieveMinDivisors(value_t cnt) {
    ++cnt;

    std::vector<bool> result(cnt, true);
    result[0] = false;
    result[1] = false;

    value_t limit = std::sqrt(cnt) + 1;
    for (value_t cur = 2; cur <= limit; ++cur) {
        if (!result[cur])
            continue;

        for (value_t next = cur * cur; next < cnt; next += cur) {
            result[next] = false;
        }
    }

    return result;
}


// General idea taken from https://euler.stephan-brumme.com/toolbox/#primecount-lehmer
struct Task {
    static constexpr value_t CACHE_MISS = -1ll;
    static constexpr size_t CACHE_PHI_SIZE_0 = 30'000ull;
    static constexpr size_t CACHE_PHI_SIZE_1 = 200ull;
    static constexpr size_t CACHE_PI_SIZE = 1'000'000ull;


    std::vector<value_t> firstPrimes{};
    std::array<std::array<value_t, CACHE_PHI_SIZE_1>, CACHE_PHI_SIZE_0> cachePhi{};
    std::array<value_t, CACHE_PI_SIZE> cachePi{};


    void calcPrimes() {
        auto minDivisors = sieveMinDivisors(10'000'000);

        for (value_t i = 2; i < (value_t)minDivisors.size(); ++i) {
            if (minDivisors[i])
                firstPrimes.push_back(i);
        }
    }

    inline value_t getCachePhi(value_t limit, value_t nPrimes) const {
        if (limit   >= (value_t)CACHE_PHI_SIZE_0 ||
            nPrimes >= (value_t)CACHE_PHI_SIZE_1)
            return CACHE_MISS;

        return cachePhi[limit][nPrimes] - 1;
    }

    inline value_t setCachePhi(value_t limit, value_t nPrimes, value_t val) {
        if (limit   >= (value_t)CACHE_PHI_SIZE_0 ||
            nPrimes >= (value_t)CACHE_PHI_SIZE_1)
            return val;

        cachePhi[limit][nPrimes] = val + 1;

        return val;
    }

    value_t phi(value_t limit, value_t nPrimes);

    inline value_t getCachePi(value_t limit) const {
        if (limit >= (value_t)CACHE_PI_SIZE)
            return CACHE_MISS;

        return cachePi[limit] - 1;
    }

    inline value_t setCachePi(value_t limit, value_t val) {
        if (limit >= (value_t)CACHE_PI_SIZE)
            return val;

        cachePi[limit] = val + 1;

        return val;
    }

    value_t pi(value_t limit);

    void solve() {
        calcPrimes();
        value_t n = 0;
        scanf("%lld", &n);
        printf("%lld\n", pi(n));
    }
};


// The number of numbers in [1, limit] that are coprime with the first n primes
value_t Task::phi(value_t limit, value_t nPrimes) {
    /*switch (limit) {
    case 0:
        return limit;
    case 1:
        return (limit - limit / 2);
    case 2:
        return (limit - limit / 2 - limit /3 + limit / 6);
    default:
        break;
    }*/

    if (limit == 0)
        return limit;

    value_t val = getCachePhi(limit, nPrimes);
    if (val != CACHE_MISS) {
        return val;
    }

    assert(nPrimes < (value_t)firstPrimes.size());

    // return setCachePhi(limit, nPrimes, phi(limit, nPrimes - 1) - phi(limit / firstPrimes[nPrimes - 1], nPrimes - 1));

    if (limit < firstPrimes[nPrimes] * firstPrimes[nPrimes])
        return setCachePhi(limit, nPrimes, pi(limit) - nPrimes + 1);

    value_t result = 0ll;
    for (; nPrimes > 1;
         --nPrimes,
         result -= phi(limit / firstPrimes[nPrimes], nPrimes)) {

        if (limit < firstPrimes[nPrimes])
            return setCachePhi(limit, nPrimes, result + 1);
    }

    return result + (limit - limit / 2);
}

// The number of primes in [1, limit]
value_t Task::pi(value_t limit) {
    value_t val = getCachePi(limit);
    if (val != CACHE_MISS) {
        return val;
    }

    if (limit < firstPrimes.back()) {
        return setCachePi(limit, (value_t)(std::distance(firstPrimes.begin(), std::upper_bound(firstPrimes.begin(), firstPrimes.end(), limit))));
    }

    value_t lim2 = pi(std::sqrt(limit)),
            lim3 = pi(std::cbrt(limit)),
            lim4 = pi(std::pow(limit, 0.25));

    assert(lim2 < (value_t)firstPrimes.size());

    value_t result = phi(limit, lim4) + (lim2 + lim4 - 2) * (lim2 - lim4 + 1) / 2;
    for (value_t i = lim4; i < lim2; ++i) {
        value_t limP = limit / firstPrimes[i];
        result -= pi(limP);

        if (i < lim3) {
            value_t tmp = pi(std::sqrt(limP));
            for (value_t j = i; j < tmp; ++j) {
                result -= pi(limP / firstPrimes[j]) - j;
            }
        }
    }

    return setCachePi(limit, result);
}


static Task task{};


int main() {
    task.solve();

    return 0;
}


