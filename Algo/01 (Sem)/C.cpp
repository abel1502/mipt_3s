#include <cstdio>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>


using std::vector;


constexpr unsigned long long MOD = 1000000007;


unsigned long long binPow(unsigned long long base, unsigned long long power) {
    unsigned long long result = 1;

    while (power) {
        if (power & 1) {
            result = (result * base) % MOD;
            --power;
        }

        base = (base * base) % MOD;
        power >>= 1;
    }
    /*while (power) {
        //printf(">> %llu * %llu ** %llu\n", result, base, power);

        if (!(power & 1)) {
            base = base * base;
            base %= MOD;
            power >>= 1;
        } else {
            result *= base;
            result %= MOD;
            --power;
        }
    }*/
    //printf("----\n");

    return result;
}


inline unsigned long long inv(unsigned long long num) {
    return binPow(num, MOD - 2);
}


int main() {
    unsigned n = 0, l = 0, r = 0;
    int res = scanf("%u %u %u", &n, &l, &r);
    assert(res == 3);

    vector<unsigned long long> factorials(n + 1);
    factorials[0] = 1;
    for (unsigned i = 0; i < n; ++i) {
        factorials[i + 1] = (factorials[i] * (i + 1)) % MOD;
    }

    vector<unsigned long long> invFactorials(n + 1);
    invFactorials[n] = inv(factorials[n]);
    for (unsigned i = n; i > 0; --i) {
        invFactorials[i - 1] = (invFactorials[i] * i) % MOD;
    }

    unsigned long long result = 0;
    for (unsigned i = 0, k = l; i < r && k <= n; i++, k += l) {
        result += (((factorials[n] * invFactorials[n - k]) % MOD) * invFactorials[k]) % MOD;
        result %= MOD;
    }

    printf("%llu\n", result);

    return 0;
}
