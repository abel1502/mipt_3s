#define _USE_MATH_DEFINES
#include <cmath>
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
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <cstdio>
#include <cstdarg>
#include <cerrno>
#include <set>
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

constexpr double EPSILON = 1e-8d;

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

    constexpr int cmp(const NAME_ &other) const {
        for (unsigned i = 0; i < DIM; ++i) {
            int res = 0;

            if constexpr (std::is_floating_point_v<T>) {
                res = cmpDbl(values[i], other.values[i]);
            } else {
                // To avoid narrowing conversion
                #if __cplusplus >= 202002L
                auto _res = values[i] <=> other.values[i];
                #else
                auto _res = values[i] - other.values[i];
                #endif

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

    constexpr bool operator==(const NAME_ &other) const { return cmp(other) == 0; }
    constexpr bool operator!=(const NAME_ &other) const { return cmp(other) != 0; }
    constexpr bool operator> (const NAME_ &other) const { return cmp(other) >  0; }
    constexpr bool operator< (const NAME_ &other) const { return cmp(other) <  0; }
    constexpr bool operator>=(const NAME_ &other) const { return cmp(other) >= 0; }
    constexpr bool operator<=(const NAME_ &other) const { return cmp(other) <= 0; }

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


bool testSegIntersection(Vec2i a0, Vec2i a1, Vec2i b0, Vec2i b1) {
    using num = num_t;
    using Vec = Vec2i;

    Vec segA = a1 - a0,
        segB = b1 - b0;

    num resA = (segA & (b0 - a0)) * (segA & (b1 - a0)),
        resB = (segB & (a0 - b0)) * (segB & (a1 - b0));

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


template <typename T>
class Polygon {
public:
    using elem_t = T;
    using vec2_t = Vector2<elem_t>;

    template <typename ... As>
    Polygon(As ... args) :
        points(args...) {}

    inline constexpr size_t size() const {
        return points.size();
    }

    inline vec2_t &operator[](int idx) {
        if (idx < 0)
            idx += (int)size();

        // assert(0 <= idx && (size_t)idx < size());

        return points[idx];
    }

    inline const vec2_t &operator[](int idx) const {
        return const_cast<Polygon *>(this)->operator[](idx);
    }

    inline decltype(auto)  cbegin() const { return points. cbegin(); }
    inline decltype(auto)   begin() const { return points.  begin(); }
    inline decltype(auto)   begin()       { return points.  begin(); }
    inline decltype(auto)    cend() const { return points.   cend(); }
    inline decltype(auto)     end() const { return points.    end(); }
    inline decltype(auto)     end()       { return points.    end(); }
    inline decltype(auto) crbegin() const { return points.crbegin(); }
    inline decltype(auto)  rbegin() const { return points. rbegin(); }
    inline decltype(auto)  rbegin()       { return points. rbegin(); }
    inline decltype(auto)   crend() const { return points.  crend(); }
    inline decltype(auto)    rend() const { return points.   rend(); }
    inline decltype(auto)    rend()       { return points.   rend(); }

    void read(std::istream &in = std::cin) {
        unsigned n = 0;
        in >> n;

        points.resize(n);
        for (auto &cur : points) {
            in >> cur;
        }
    }

    void write(std::ostream &out = std::cout) const {
        out << size() << "\n";

        for (auto &cur : *this) {
            out << cur << "\n";
        }
    }

    unsigned minPointIdx() const {
        return std::min_element(points.begin(), points.end()) - points.begin();
    }

    Polygon convexified() {
        vec2_t minPoint = points[minPointIdx()];

        std::sort(points.begin(), points.end(),
                  [minPoint](const vec2_t &a, const vec2_t &b) {
                      int cross = sgnDbl((a - minPoint) & (b - minPoint));
                      return  cross  > 0 ||
                             (cross == 0 && (a - minPoint).magnitude() < (b - minPoint).magnitude());
                  });

        Polygon convexHull{};

        for (const auto &cur : points) {
            while (convexHull.size() >= 2) {
                const vec2_t &last1 = convexHull[-1],
                             &last2 = convexHull[-2];

                if (sgnDbl((last1 - last2) & (cur - last2)) <= 0) {
                    convexHull.points.pop_back();
                } else {
                    break;
                }
            }

            convexHull.points.push_back(cur);
        }

        assert(convexHull.size() >= 3);

        return convexHull;
    }

    bool isConvex() const {
        vec2_t last2 = points[points.size() - 2];
        vec2_t last1 = points[points.size() - 1];
        int sign = 0;

        for (const auto &cur : points) {
            if ((sign = sgnDbl((last1 - last2) & (cur - last2))) != 0) {
                break;
            }

            last2 = last1;
            last1 = cur;
        }

        for (const auto &cur : points) {
            if (sgnDbl((last1 - last2) & (cur - last2)) == -sign) {
                return false;
            }

            last2 = last1;
            last1 = cur;
        }

        return true;
    }

    bool contains(const vec2_t &target) const {
        constexpr vec2_t ray{1, 0};
        vec2_t last = (*this)[-1];
        unsigned cntEdges = 0;
        for (const auto &cur : *this) {
            vec2_t a = cur,
                   b = last;

            last = cur;

            if (a == b) {
                continue;
            }

            if (a.y() > b.y()) {
                std::swap(a, b);
            }

            if (cmpDbl((b - a).length(), (target - a).length() + (target - b).length()) == 0) {
                return true;
            }

            if (testSegIntersection(target, target + ray * 20002, a, b) &&
                b.y() > target.y() && a.y() <= target.y() &&
                ((target - a) & (b - a)) < 0) {

                ++cntEdges;
            }
        }

        return cntEdges % 2 == 1;
    }

    #if 0
    bool convexContains(const vec2_t &target) const {
        vec2_t last2 = (*this)[-2];
        vec2_t last1 = (*this)[-1];
        int sign = 0;

        for (const auto &cur : *this) {
            if ((sign = sgnDbl((last1 - last2) & (cur - last2))) != 0) {
                break;
            }

            last2 = last1;
            last1 = cur;
        }

        last2 = (*this)[-2];
        last1 = (*this)[-1];
        for (const auto &cur : *this) {
            if (((cur - last1) & (target - last1)) * sign < 0) {
                return false;
            }

            last2 = last1;
            last1 = cur;
        }

        return true;
    }
    #else
    // Gotta go fast-er
    // Also suggested by Max
    bool convexContains(const vec2_t &target) const {
        vec2_t ref = (*this)[0];

        unsigned left  = 0;
        unsigned right = size() - 1;

        while (left + 1 < right) {
            unsigned mid = (left + right) / 2;

            if ((((*this)[mid] - ref) & (target - ref)) < 0) {
                right = mid;
            } else {
                left = mid;
            }
        }

        return triangleContains(ref, (*this)[left], (*this)[right], target);
    }

    protected:
    static inline bool triangleContains(const vec2_t &a,
                                        const vec2_t &b,
                                        const vec2_t &c,
                                        const vec2_t &target) {
        elem_t areaFromTarget = std::abs((target - a) & (target - b))
                             + std::abs((target - b) & (target - c))
                             + std::abs((target - c) & (target - a));

        elem_t area = std::abs((c - a) & (c - b));

        // TODO: cmpDbl
        return areaFromTarget == area;
    }
    public:
    #endif

    Polygon operator+(const Polygon &other) const {
        unsigned idxA =       minPointIdx();
        unsigned idxB = other.minPointIdx();

        unsigned cntA = 0;
        unsigned cntB = 0;

        Polygon result{};
        result.points.push_back((*this)[idxA] +
                                (other)[idxB]);

        while (cntA < size() && cntB < other.size()) {
            unsigned nextIdxA = (idxA + 1) %       size(),
                     nextIdxB = (idxB + 1) % other.size();

            vec2_t edgeA = (*this)[nextIdxA] - (*this)[idxA],
                   edgeB = (other)[nextIdxB] - (other)[idxB];

            elem_t cross = edgeA & edgeB;
            int crossSgn = sgnDbl(cross);

            if (crossSgn == 0) {
                result.points.push_back(result[-1] + edgeA + edgeB);

                idxA = nextIdxA;
                idxB = nextIdxB;
                ++cntA;
                ++cntB;
            } else if (crossSgn < 0) {
                result.points.push_back(result[-1] + edgeB);

                idxB = nextIdxB;
                ++cntB;
            } else /* crossSgn > 0 */ {
                result.points.push_back(result[-1] + edgeA);

                idxA = nextIdxA;
                ++cntA;
            }
        }

        for (; cntA < size(); ++cntA) {
            unsigned nextIdxA = (idxA + 1) % size();
            result.points.push_back(result[-1] + (*this)[nextIdxA] - (*this)[idxA]);
            idxA = nextIdxA;
        }

        for (; cntB < other.size(); ++cntB) {
            unsigned nextIdxB = (idxB + 1) % other.size();
            result.points.push_back(result[-1] + (other)[nextIdxB] - (other)[idxB]);
            idxB = nextIdxB;
        }

        result.points.pop_back();

        return result;
    }


protected:
    std::vector<vec2_t> points{};

};


template <typename T>
inline std::istream &operator>>(std::istream &in, Polygon<T> &result) {
    result.read(in);

    return in;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &out, const Polygon<T> &value) {
    value.write(out);

    return out;
}


template <typename T>
class DynConvexHull {
public:
    using elem_t = T;
    using vec2_t = Vector2<elem_t>;
    using set_t = std::set<vec2_t>;
    using set_iterator_t = typename set_t::iterator;


    DynConvexHull() = default;

    DynConvexHull(const std::vector<vec2_t> &points) :
        DynConvexHull() {

        assert(points.size() >= 3);

        std::vector<vec2_t> topVec{}, bottomVec{};

        for (const vec2_t &cur : points) {
            while (topVec.size() >= 2) {
                const Vec2i &last1 = topVec[topVec.size() - 1],
                            &last2 = topVec[topVec.size() - 2];

                if (sgnDbl((last1 - last2) & (cur - last2)) <= 0) {
                    topVec.pop_back();
                } else {
                    break;
                }
            }

            while (bottomVec.size() >= 2) {
                const Vec2i &last1 = bottomVec[bottomVec.size() - 1],
                            &last2 = bottomVec[bottomVec.size() - 2];

                if (sgnDbl((last1 - last2) & (cur - last2)) >= 0) {
                    bottomVec.pop_back();
                } else {
                    break;
                }
            }

            topVec   .push_back(cur);
            bottomVec.push_back(cur);
        }

        top    = set_t(   topVec.begin(),    topVec.end());
        bottom = set_t(bottomVec.begin(), bottomVec.end());
    }

    void insert(const vec2_t &point) {
        if (contains(point)) {
            return;
        }

        set_iterator_t topPos[2] = {{}, top.lower_bound(point)};
        topPos[0] = topPos[1]--;

        set_iterator_t bottomPos[2] = {{}, bottom.lower_bound(point)};
        bottomPos[0] = bottomPos[1]--;

        if (topPos[0] == top.end()) {
            push<true , true >(topPos   [1], point);
            push<true , false>(bottomPos[1], point);

            top.insert(point);
            bottom.insert(point);
        } else if (topPos[0] == top.begin()) {
            push<false, true >(topPos   [0], point);
            push<false, false>(bottomPos[0], point);

            top.insert(point);
            bottom.insert(point);
        } else if ((*(topPos[1]) - *(bottomPos[1])) &
                   (point        - *(bottomPos[1]))) {
            push<false, false>(bottomPos[0], point);
            push<true , false>(bottomPos[1], point);
            bottom.insert(point);
        } else {
            push<false, true >(   topPos[0], point);
            push<true , true >(   topPos[1], point);
            top.insert(point);
        }
    }

    bool contains(const vec2_t &point) const {
        dump();

        set_iterator_t    topPos = top   .lower_bound(point);
        set_iterator_t bottomPos = bottom.lower_bound(point);

        if (topPos == top.end() || bottomPos == bottom.end()) {
            return false;
        }

        if (*topPos == point || *bottomPos == point) {
            return true;
        }

        if (topPos == top.begin() || bottomPos == bottom.begin()) {
            return false;
        }

        vec2_t topNode     = *  topPos;
        vec2_t bottomNode  = *  bottomPos;

        vec2_t topEdge     = *  topPos;
               topEdge    -= *--topPos;
        vec2_t bottomEdge  = *  bottomPos;
               bottomEdge -= *--bottomPos;

        return (topEdge & (point - topNode)) * (bottomEdge & (point - bottomNode)) <= 0;
    }

    void dump() const {
        if (verbosity < 2)
            return;

        DBG("Top: {");
        for (auto &item : top) {
            std::cout << item << "\n";
        }
        DBG("}");

        DBG("Bottom:");
        for (auto &item : bottom) {
            std::cout << item << "\n";
        }

        DBG("}");
    }


protected:
    set_t top{}, bottom{};


    template <bool IsForward, bool IsTop>
    void push(const set_iterator_t &pos_, const vec2_t &point) {
        using Iter = std::conditional_t<IsForward, typename set_t::reverse_iterator, typename set_t::iterator>;

        constexpr elem_t sign = (elem_t)((IsTop ? 1 : -1) * (IsForward ? 1 : -1));
        set_t &halfHull = IsTop ? top : bottom;
        Iter pos{pos_};
        Iter end{};

        if constexpr (IsForward) {
            end = halfHull.rend();
        } else {
            end = halfHull.end();
        }

        for (; pos != end; /*++pos*/) {
            // A fancy machination
            auto prev = pos++;

            if (pos == end)
                break;

            const vec2_t &vec0 = *prev,
                         &vec1 = *pos;

            if (sgnDbl((vec0 - vec1) & (point - vec1)) * sign > 0) {
                break;
            }

            halfHull.erase(*prev);
        }
    }

};


static void solve() {
    unsigned n = 0;
    std::cin >> n;

    std::vector<Vec2i> points(3);
    for (unsigned i = 0; i < 3; ++i) {
        unsigned cmd = 0;

        std::cin >> cmd >> points[i];
        assert(cmd == 1);
    }

    std::sort(points.begin(), points.end());

    DynConvexHull dch{points};
    for (unsigned i = 3; i < n; ++i) {
        unsigned cmd = 0;
        Vec2i point{};

        std::cin >> cmd >> point;

        switch (cmd) {
        case 1:
            dch.insert(point);
            break;

        case 2:
            std::cout << (dch.contains(point) ? "YES" : "NO") << "\n";
            break;

        NODEFAULT
        }
    }
}

//================================================================================


#ifndef DO_H2
int main() {
    verbosity = 0;

    #ifdef ALGO_LOCAL
    verbosity = 2;
    freopen("H.in", "r", stdin);
    #else
    std::ios_base::sync_with_stdio(false);
    std::cin .tie(nullptr);
    std::cout.tie(nullptr);
    #endif // ALGO_LOCAL

    // std::cout << std::fixed << std::setprecision(8);

    solve();

    return 0;
}
#endif





