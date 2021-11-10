#include <cstdio>
#include <cassert>
#define _USE_MATH_DEFINES
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
#include <complex>


#if 0
#define DBG(FMT, ...) \
    printf("[%s#%d: " FMT "]\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define DBG(FMT, ...)
#endif


using value_t = long long;
using complex = std::complex<double>;

constexpr value_t MOD = 1'000'000'007ll;


// std::swap isn't constexpr on codeforces for some reason
template <typename T>
constexpr void swap(T &a, T &b) {
    T tmp = a;
    a = b;
    b = tmp;
}

template <typename T>
constexpr int sign(const T &val) {
    T zero{};

    if (val < zero)  return -1;
    if (val > zero)  return  1;
    return  0;
}

constexpr value_t modulo(value_t val) {
    return ((val % MOD) + MOD) % MOD;
}

constexpr value_t modulo(value_t val, value_t mod) {
    return (val % mod + mod) % mod;
}

constexpr value_t binPow(value_t base, value_t power) {
    value_t result = 1;

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


struct Polynomial {
    std::vector<value_t> values{};

    using fft_t = std::valarray<complex>;


    static unsigned getMinSize(unsigned sizeA, unsigned sizeB) {
        unsigned size = std::max(sizeA, sizeB);
        unsigned result = 1 << (sizeof(result) * 8 - __builtin_clrsb(size));

        assert(result >= size);
        assert(!(result & (result - 1)));
        return result;
    }

    fft_t getFFTArr(unsigned size) const {
        assert(size >= values.size());

        fft_t result(size);

        for (unsigned i = 0; i < values.size(); ++i) {
            result[i] = values[i];
        }

        fft<false>(result);

        return result;
    }

    void fromFFTArr(fft_t fftArr) {
        fft<true>(fftArr);

        values.assign(fftArr.size(), 0);

        for (unsigned i = 0; i < fftArr.size(); ++i) {
            double tmp = fftArr[i].real();

            // A form of rounding suggested by a friend
            values[i] = (value_t)(tmp + sign(tmp) * 0.5d);
        }

        trimHighZeros();
    }

    void trimHighZeros() {
        unsigned lastGood = values.size() - 1;

        for (; lastGood != -1u && !values[lastGood]; --lastGood);

        values.resize(lastGood + 1);
    }

    template <bool inverted = false>
    static void fft(fft_t &fftArr) {
        for (unsigned i = 1, j = 0; i < fftArr.size(); ++i) {
            unsigned pow2 = fftArr.size() >> 1;

            while (j >= pow2) {
                j -= pow2;
                pow2 >>= 1;
            }

            j += pow2;

            if (j > i)
                ::swap(fftArr[i], fftArr[j]);
        }

        assert(!(fftArr.size() & (fftArr.size() - 1)));

        for (unsigned step = 2; step <= fftArr.size(); step <<= 1) {
            double angle = (inverted ? -1 : 1) * 2 * M_PI / step;
            complex omegaStep{std::cos(angle), std::sin(angle)};

            for (unsigned i = 0; i < fftArr.size(); i += step) {
                complex omega{1};

                for (unsigned j = 0; j < step / 2; ++j, omega *= omegaStep) {
                    assert(i + j + step / 2 < fftArr.size());
                    complex a = fftArr[i + j];
                    complex b = fftArr[i + step / 2 + j] * omega;

                    fftArr[i + j] = a + b;
                    fftArr[i + step / 2 + j] = a - b;
                }
            }
        }

        if constexpr (!inverted) {
            return;
        }

        fftArr /= fftArr.size();
    }

    Polynomial &operator*=(const Polynomial &other) {
        unsigned fftSize = getMinSize(values.size(), other.values.size());

        fromFFTArr(getFFTArr(fftSize) *= other.getFFTArr(fftSize));

        return *this;
    }

    Polynomial operator*(const Polynomial &other) const {
        Polynomial result{*this};

        return result *= other;
    }


    friend std::istream &operator>>(std::istream &in, Polynomial &poly) {
        unsigned n = 0;
        in >> n;

        poly.values.resize(n + 1);

        for (; n != -1u; --n) {
            in >> poly.values[n];
        }

        return in;
    }

    friend std::ostream &operator<<(std::ostream &out, const Polynomial &poly) {
        unsigned n = poly.values.size() - 1;

        if (n == -1u) {
            out << "0\n";
            return out;
        }

        out << n << " ";
        for (; n != -1u; --n) {
            out << poly.values[n] << " ";
        }
        out << "\n";

        return out;
    }
};


int main() {
    Polynomial a{}, b{};

    std::cin >> a >> b;
    std::cout << a * b;

    return 0;
}

