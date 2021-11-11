#ifndef ACL_GENERAL_H
#define ACL_GENERAL_H

#include <stdexcept>
#include <string>
#include <typeinfo>
#include <cstdlib>
#include <utility>


#define MACROFUNC(...) do {__VA_ARGS__} while (0)
#define MACROCOMMA  ,
#define MACROEMPTY
#define MACROARGS(...)  __VA_ARGS__


#define ERR(msg, ...)  abel::dbg_(true,  1, __func__, __LINE__, msg, ##__VA_ARGS__)
#define DBG(msg, ...)  abel::dbg_(false, 2, __func__, __LINE__, msg, ##__VA_ARGS__)


#if defined(_WIN32) || defined(WIN32)
#define PAUSE()  system("pause")
#elif defined(__unix__)
#define PAUSE()  system("read -n1 -r -p \"Press any key to continue...\"")
#else
#warning "ACL: Weird OS, PAUSE not available (yet)"
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
        throw abel::require_error("");                  \
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
    } catch (const abel::error &e) {                                        \
        ERR("Uncaught ACL error (%s): %s", typeid(e).name(), e.what());     \
        return (EXC_RETVAL);                                                \
    } catch (const std::exception& e) {                                     \
        ERR("Uncaught C++ error (%s): %s", typeid(e).name(), e.what());     \
        return (EXC_RETVAL);                                                \
    } catch (...) {                                                         \
        ERR("Uncaught unknown...?");                                        \
        return (EXC_RETVAL);                                                \
    }


namespace abel {


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

inline double randDouble(double max = 1.) {
    constexpr unsigned DISCRETENESS = 1'000'000;

    return max * (double)randLL(DISCRETENESS) / (double)DISCRETENESS;
}

inline double randDouble(double min, double max) {

    return randDouble(max - min) + min;
}


// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wfloat-equal"

constexpr bool isZero(double val) {
    return val == 0.;
}

constexpr double EPSILON = 1e-30;

constexpr int sgnDbl(double val) {
    if (val >=  EPSILON) return  1;
    if (val <= -EPSILON) return -1;
    return 0;
}

constexpr int cmpDbl(double a, double b) {
    return sgnDbl(a - b);
}

// #pragma GCC diagnostic pop


}


#endif // ACL_GENERAL_H