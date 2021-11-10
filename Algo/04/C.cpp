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

std::vector<value_t> sieveMinDivisors(value_t cnt) {
    ++cnt;

    std::vector<value_t> result(cnt, 0);

    value_t limit = sqrt(cnt) + 1;
    for (value_t cur = 2; cur < limit; ++cur) {
        if (result[cur])
            continue;

        for (value_t next = cur; next < cnt; next += cur) {
            result[next] = result[next] ? result[next] : cur;
        }
    }

    return result;
}


struct Segment {
    static constexpr unsigned INF = -1u;

    unsigned left = 0, right = INF;

    constexpr unsigned length() const noexcept {
        return right - left;
    }

    constexpr bool operator<(const Segment &other) const noexcept {
        return length() < other.length();
    }

    constexpr bool isInf() const noexcept {
        return right == INF;
    }
};


int main() {
    unsigned n = 0;

    std::cin >> n;

    std::vector<value_t> arr(n);
    for (unsigned i = 0; i < n; ++i) {
        std::cin >> arr[i];
    }

    unsigned cntOnes = 0;
    Segment bestSeg{};
    Segment curSeg{0, 0};

    for (curSeg.left = 0; curSeg.left < n; ++curSeg.left) {
        assert(arr[curSeg.left] > 0);
        if (arr[curSeg.left] == 1) {
            cntOnes++;
        }

        value_t segGcd = arr[curSeg.left];
        for (curSeg.right = curSeg.left + 1; curSeg.right < n && segGcd != 1; ++curSeg.right) {
            segGcd = gcd(segGcd, arr[curSeg.right]);
        }

        curSeg.right--;

        if (segGcd == 1 && curSeg < bestSeg) {
            bestSeg = curSeg;
        }
    }

    if (bestSeg.isInf()) {
        std::cout << "-1";
    } else if (bestSeg.length() == 0) {
        std::cout << n - cntOnes;
    } else {
        std::cout << n + bestSeg.length() - 1;
    }

    std::cout << "\n";

    return 0;
}



