#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <type_traits>
#include <initializer_list>
#define _USE_MATH_DEFINES
#include <cmath>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <cstdio>
#include <cstdarg>
#include <cerrno>
// #include <format>


//================================================================================

#pragma GCC diagnostic ignored "-Wsuggest-final-types"
#pragma GCC diagnostic ignored "-Wsuggest-final-methods"


#define MACROFUNC(...) do {__VA_ARGS__} while (0)


#define MACROCOMMA  ,


#define MACROARGS(...)  __VA_ARGS__


#define ERR(msg, ...)  dbg_(true,  1, __func__, __LINE__, msg, ##__VA_ARGS__)
#define DBG(msg, ...)  dbg_(false, 2, __func__, __LINE__, msg, ##__VA_ARGS__)


#if defined(_WIN32) || defined(WIN32)
#define PAUSE()  system("pause")
#elif defined(__unix__)
#define PAUSE()  system("read -n1 -r -p \"Press any key to continue...\"")
#else
#pragma GCC warning "ACL: Weird OS, PAUSE not available (yet)"
#endif


#define IS_REACHED(...)  DBG("<"); __VA_ARGS__; DBG(">");


#define DECLARE_ERROR(NAME, PARENT)                     \
    class NAME : public PARENT {                        \
    public:                                             \
        NAME(const char *msg) : PARENT(msg) {}          \
        NAME(const std::string &msg) : PARENT(msg) {}   \
    };


#define REQUIRE(STMT) MACROFUNC(                        \
    if (!(STMT)) {                                      \
        ERR("Requirement not satisfied: %s\n", #STMT);  \
        throw require_error("");                  \
    })


#define NODEFAULT                       \
    default: {                          \
        ERR("Shouldn't be reachable");  \
        abort();                        \
    }


#define MAIN_TRY(...)  MAIN_TRY_R(1, __VA_ARGS__)


#define MAIN_TRY_R(EXC_RETVAL, ...)                                         \
    try {                                                                   \
        __VA_ARGS__                                                         \
    } catch (const error &e) {                                        \
        ERR("Uncaught ACL error (%s): %s", typeid(e).name(), e.what());     \
        return (EXC_RETVAL);                                                \
    } catch (const std::exception& e) {                                     \
        ERR("Uncaught C++ error (%s): %s", typeid(e).name(), e.what());     \
        return (EXC_RETVAL);                                                \
    } catch (...) {                                                         \
        ERR("Uncaught unknown...?");                                        \
        return (EXC_RETVAL);                                                \
    }

DECLARE_ERROR(error, std::runtime_error)
DECLARE_ERROR(require_error, error)


extern int verbosity;

void dbg_(bool isError, int level, const char *funcName, int lineNo, const char *msg, ...);


extern unsigned long long randSeed;

inline void srand(unsigned long long seed) {
    randSeed = seed;
}

unsigned long long randLL();

inline unsigned long long randLL(unsigned long long max) {
    return randLL() % max;
}

inline double randDouble(double max = 1.d) {
    constexpr unsigned DISCRETENESS = 1'000'000;

    return max * (double)randLL(DISCRETENESS) / (double)DISCRETENESS;
}

inline double randDouble(double min, double max) {

    return randDouble(max - min) + min;
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"

constexpr bool isZero(double val) {
    return val == 0.d;
}

constexpr double EPSILON = 1e-6d;

constexpr int sgnDbl(double val) {
    if (val >=  EPSILON) return  1;
    if (val <= -EPSILON) return -1;
    return 0;
}

constexpr int cmpDbl(double a, double b) {
    return sgnDbl(a - b);
}

#pragma GCC diagnostic pop

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

unsigned long long randSeed = 123;

unsigned long long randLL() {
    randSeed ^= randSeed << 21;
    randSeed ^= randSeed >> 35;
    randSeed ^= randSeed << 4;

    return randSeed;
}

//================================================================================

template <typename T>
T clamp(const T &val, const T &border0, const T &border1) {
    const T &from = border0 < border1 ? border0 : border1;
    const T &to   = &from == &border0 ? border1 : border0;  // To avoid an extra comparison

    if (val < from)
        return from;
    if (val > to)
        return to;
    return val;
}

//================================================================================

// Vector2
#define NAME_   Vector2
#define N_      2
#define EXTRA_                                                  \
    constexpr T &x() noexcept {                                 \
        return values[0];                                       \
    }                                                           \
                                                                \
    constexpr const T &x() const noexcept {                     \
        return values[0];                                       \
    }                                                           \
                                                                \
    constexpr T &y() noexcept {                                 \
        return values[1];                                       \
    }                                                           \
                                                                \
    constexpr const T &y() const noexcept {                     \
        return values[1];                                       \
    }                                                           \
                                                                \
    constexpr Vector2(T new_x, T new_y) noexcept :              \
        values{new_x, new_y} {}                                 \
                                                                \
    /* Vector multiplication */                                 \
    constexpr T operator&(const Vector2 &other) const noexcept {\
        return x() * other.y() - y() * other.x();               \
    }                                                           \
                                                                \
    constexpr Vector2 &orthogonalize() noexcept {               \
        std::swap(x(), y());                                    \
        y() *= (T)-1;                                           \
                                                                \
        return *this;                                           \
    }                                                           \
                                                                \
    constexpr Vector2 orthogonalized() const noexcept {         \
        Vector2 result{*this};                                  \
                                                                \
        return result.orthogonalize();                          \
    }                                                           \
                                                                \
    constexpr Vector2 &rotateRadians(double radians) noexcept { \
        T new_x = (T)(cos(radians) * x() - sin(radians) * y()); \
        y() =     (T)(sin(radians) * x() + cos(radians) * y()); \
        x() = new_x;                                            \
                                                                \
        return *this;                                           \
    }                                                           \
                                                                \
    constexpr Vector2 &rotateDegrees(double degrees) noexcept { \
        rotateRadians(degrees / 180.d * M_PI);                  \
                                                                \
        return *this;                                           \
    }                                                           \
                                                                \
    constexpr Vector2 rotatedRadians(double radians) const      \
                                                     noexcept { \
        Vector2 result{*this};                                  \
                                                                \
        return result.rotateRadians(radians);                   \
    }                                                           \
                                                                \
    constexpr Vector2 rotatedDegrees(double degrees) const      \
                                                     noexcept { \
        return rotatedRadians(degrees / 180.d * M_PI);          \
    }                                                           \
                                                                \
    static constexpr Vector2 fromPhiRadiansR(double phiRadians, \
                                             double r)          \
                                                noexcept {      \
        return Vector2{r * std::cos(phiRadians),                \
                       r * std::sin(phiRadians)};               \
    }                                                           \
                                                                \
    static constexpr Vector2 fromPhiDegreesR(double phiDegrees, \
                                             double r)          \
                                                noexcept {      \
        return fromPhiRadiansR(phiDegrees / 180.d * M_PI, r);   \
    }

#if !defined(EXTRA_)
#define EXTRA_
#endif

template <typename T>
class NAME_ {
    static_assert(1 <= N_ && N_ <= 4);
    //static_assert(std::is_signed_v<T>);  // May theoretically be removed, because constexpr would still work

public:
    using TYPE = T;
    using ARG_TYPE = T;
    static constexpr unsigned DIM = N_;
    static constexpr NAME_<T> ZERO{};

    T values[DIM];
    unsigned idx = 0;


    constexpr NAME_() noexcept :
        values{} {}

    constexpr NAME_(const NAME_ &other) noexcept = default;

    // Because for N == 1 this is equivalent to the complete constructor
    #if N_ > 1
    constexpr NAME_(ARG_TYPE val) noexcept :
        values{} {

        for (unsigned i = 0; i < DIM; ++i) {
            values[i] = val;
        }
    }
    #endif

    constexpr NAME_ &operator=(const NAME_ &other) noexcept = default;

    constexpr NAME_ &operator+=(const NAME_ &other) noexcept {
        for (unsigned i = 0; i < DIM; ++i) {
            values[i] += other.values[i];
        }

        return *this;
    }

    constexpr NAME_ &operator-=(const NAME_ &other) noexcept {
        for (unsigned i = 0; i < DIM; ++i) {
            values[i] -= other.values[i];
        }

        return *this;
    }

    template <typename U>
    constexpr NAME_ &operator*=(U other) noexcept {
        static_assert(std::is_arithmetic_v<U>);

        for (unsigned i = 0; i < DIM; ++i) {
            values[i] = (T)(values[i] * other);
        }

        return *this;
    }

    template <typename U>
    constexpr NAME_ &operator/=(U other) noexcept {
        static_assert(std::is_arithmetic_v<U>);

        for (unsigned i = 0; i < DIM; ++i) {
            values[i] = (T)(values[i] / other);
        }

        return *this;
    }

    constexpr NAME_ &operator*=(ARG_TYPE other) noexcept {
        for (unsigned i = 0; i < DIM; ++i) {
            values[i] *= other;
        }

        return *this;
    }


    constexpr NAME_ &operator/=(ARG_TYPE other) noexcept {
        for (unsigned i = 0; i < DIM; ++i) {
            values[i] /= other;
        }

        return *this;
    }

    /// Scalar multiplication
    constexpr T operator*(const NAME_ &other) const noexcept {
        T result{};

        for (unsigned i = 0; i < DIM; ++i) {
            result += values[i] * other.values[i];
        }

        return result;
    }

    constexpr NAME_ &normalize() noexcept {
        double length_ = length();

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wfloat-equal"
        if (length_ == 0.d) {
            DBG("Zero-vector normalization. Ignoring");
            return *this;
        }
        #pragma GCC diagnostic pop

        return *this *= 1 / length_;
    }

    constexpr NAME_ normalized() const noexcept {
        NAME_ result{*this};

        return result.normalize();
    }

    constexpr NAME_ &mirrorAgainst(const NAME_ &against) noexcept {
        return *this = mirroredAgainst(against);
    }

    constexpr NAME_ mirroredAgainst(const NAME_ &against) const noexcept {
        return against * (against * *this) * 2 / against.magnitude() - *this;
    }

    constexpr NAME_ &mirrorAlong(const NAME_ &along) noexcept {
        return *this = mirroredAlong(along);
    }

    constexpr NAME_ mirroredAlong(const NAME_ &along) const noexcept {
        return *this - projected(along) * 2;
    }

    constexpr NAME_ &project(const NAME_ &onto) noexcept {
        return *this = projected(onto);
    }

    constexpr NAME_ projected(const NAME_ &onto) const noexcept {
        return onto * (*this * onto) / onto.magnitude();
    }

    constexpr NAME_ &clamp(const NAME_ &from, const NAME_ &to) noexcept {
        for (unsigned i = 0; i < DIM; ++i) {
            values[i] = clamp(values[i], from.values[i], to.values[i]);
        }

        return *this;
    }

    constexpr NAME_ clamped(const NAME_ &from, const NAME_ &to) const noexcept {
        NAME_ result{*this};

        return result.clamp();
    }

    constexpr double length() const noexcept {
        if constexpr (DIM == 1)
            return (double)values[0];
        if constexpr (DIM == 2)
            return std::hypot((double)values[0], (double)values[1]);
        if constexpr (DIM == 3)
            return std::hypot((double)values[0], (double)values[1], (double)values[2]);
        return std::sqrt((double)magnitude());
    }

    constexpr T magnitude() const noexcept {
        T result{};

        for (unsigned i = 0; i < DIM; ++i)
            result += values[i] * values[i];

        return result;
    }

    constexpr bool isZero() const noexcept {
        for (unsigned i = 0; i < DIM; ++i)
            if (sgnDbl(values[i]))
                return false;

        return true;
    }

    constexpr friend NAME_ operator+(NAME_ self, const NAME_ &other) noexcept {
        return self += other;
    }

    constexpr friend NAME_ operator-(NAME_ self, const NAME_ &other) noexcept {
        return self -= other;
    }

    constexpr friend NAME_ operator*(NAME_ self, ARG_TYPE other) noexcept {
        return self *= other;
    }

    constexpr friend NAME_ operator/(NAME_ self, ARG_TYPE other) noexcept {
        return self /= other;
    }

    constexpr friend NAME_ operator*(ARG_TYPE other, NAME_ self) noexcept {
        return self *= other;
    }

    constexpr friend NAME_ operator-(NAME_ self) noexcept {
        return self *= -1.d;
    }

    constexpr int operator<=>(const NAME_ &other) const {
        for (unsigned i = 0; i < DIM; ++i) {
            int res = 0;

            if constexpr (std::is_floating_point_v<T>) {
                res = cmpDbl(values[i], other.values[i]);
            } else {
                // To avoid narrowing conversion
                auto _res = values[i] <=> other.values[i];

                if (_res < 0)
                    res = -1;
                else if (_res > 0)
                    res = 1;
            }

            if (res)
                return res;
        }

        return 0;
    }

    constexpr bool operator==(const NAME_ &other) const = default;

    template <typename U>
    constexpr explicit operator NAME_<U>() const noexcept {
        NAME_<U> result{};

        for (unsigned i = 0; i < DIM; ++i)
            result.values[i] = (U)values[i];

        return result;
    }

    static NAME_ readNew(std::istream &in = std::cin) {
        NAME_ result{};

        result.read(in);

        return result;
    }

    void read(std::istream &in = std::cin) {
        for (unsigned i = 0; i < DIM; ++i) {
            in >> values[i];
        }
    }

    void write(bool newLine = true, std::ostream &out = std::cout) const {
        for (unsigned i = 0; i < DIM; ++i) {
            out << values[i] << " ";
        }

        if (newLine) {
            out << "\n";
        }
    }

    EXTRA_

};

#undef N_
#undef NAME_
#undef EXTRA_


template <typename T>
inline std::istream &operator>>(std::istream &in, Vector2<T> &result) {
    result.read(in);

    return in;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &out, const Vector2<T> &value) {
    value.write(false, out);

    return out;
}


using num_t = long long;
// using num_t = double;
using Vec2i = Vector2<num_t>;
using Vec2d = Vector2<double>;


std::vector<Vec2i> getConvexHull(std::vector<Vec2i> &points) {
    // This was std::min, which broke everything...
    Vec2i minPoint = *std::min_element(points.begin(), points.end());

    std::sort(points.begin(), points.end(),
              [minPoint](const Vec2i &a, const Vec2i &b) {
                  int cross = sgnDbl((a - minPoint) & (b - minPoint));
                  return  cross  > 0 ||
                         (cross == 0 && (a - minPoint).magnitude() < (b - minPoint).magnitude());
              });

    std::vector<Vec2i> convexHull{};

    for (const auto &cur : points) {
        while (convexHull.size() >= 2) {
            const Vec2i &last1 = convexHull[convexHull.size() - 1],
                        &last2 = convexHull[convexHull.size() - 2];

            if (sgnDbl((last1 - last2) & (cur - last2)) <= 0) {
                convexHull.pop_back();
            } else {
                break;
            }
        }

        convexHull.push_back(cur);
    }

    assert(convexHull.size() >= 3);

    return convexHull;
}


bool testSegIntersection(Vec2i a0, Vec2i a1, Vec2i b0, Vec2i b1) {
    using num = num_t;
    using Vec = Vec2i;

    Vec segA = a1 - a0,
        segB = b1 - b0;

    num resA = (segA & (b0 - a0)) * (segA & (b1 - a0)),
        resB = (segB & (a0 - b0)) * (segB & (a1 - b0));

    DBG("segA %lld %lld   segB %lld %lld", segA.x(), segA.y(), segB.x(), segB.y());
    DBG("resA %lld  resB %lld", resA, resB);

    if (resA != 0 && resB != 0) {
        return resA < 0 && resB < 0;
    }

    if ((segA & segB) == 0) {
        if (((b0 - a0) & segA) != 0)
            return false;

        const num na0 = 0,
                  na1 = segA.magnitude(),
                  nb0 = (b0 - a0) * segA,
                  nb1 = (b1 - a0) * segA;

        DBG("%lld %lld %lld %lld", na0, na1, nb0, nb1);

        return (nb0 == clamp(nb0, na0, na1) || nb1 == clamp(nb1, na0, na1));
    }

    if (resA == 0 && resB == 0)
        return true;

    if ((segA & (b0 - a0)) == 0) {
        const num na0 = 0,
                  na1 = segA.magnitude(),
                  nb  = (b0 - a0) * segA;
        return nb == clamp(nb, na0, na1) ;
    }

    if ((segA & (b1 - a0)) == 0) {
        const num na0 = 0,
                  na1 = segA.magnitude(),
                  nb  = (b1 - a0) * segA;
        return nb == clamp(nb, na0, na1) ;
    }

    if ((segB & (a0 - b0)) == 0) {
        const num nb0 = 0,
                  nb1 = segB.magnitude(),
                  na  = (a0 - b0) * segB;
        return na == clamp(na, nb0, nb1) ;
    }

    if ((segB & (a1 - b0)) == 0) {
        const num nb0 = 0,
                  nb1 = segB.magnitude(),
                  na  = (a1 - b0) * segB;
        return na == clamp(na, nb0, nb1) ;
    }

    REQUIRE(false);
}


static void solve() {
    unsigned n = 0;
    std::cin >> n;

    std::vector<Vec2i> points(n);
    for (auto &cur : points) {
        std::cin >> cur;
    }

    //
}

//================================================================================


int main() {
    verbosity = 0;

    solve();

    return 0;
}








