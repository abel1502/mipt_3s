#ifndef ACL_GENERAL_H
#define ACL_GENERAL_H


#include <stdexcept>
#include <string>


#define MACROFUNC(...) do {__VA_ARGS__} while (0)


#define ERR(msg, ...) abel::dbg_(true,  1, __func__, __LINE__, msg, ##__VA_ARGS__)
#define DBG(msg, ...) abel::dbg_(false, 2, __func__, __LINE__, msg, ##__VA_ARGS__)


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
    default:                            \
        ERR("Shouldn't be reachable");  \
        abort();

#define MAIN_TRY(...)  MAIN_TRY_R(1, __VA_ARGS__)


#define MAIN_TRY_R(EXC_RETVAL, ...)                 \
    try {                                           \
        __VA_ARGS__                                 \
    } catch (const abel::error &e) {                \
        ERR("Uncaught ACL error: %s", e.what());    \
        return (EXC_RETVAL);                        \
    } catch (const std::exception& e) {             \
        ERR("Uncaught C++ error: %s", e.what());    \
        return (EXC_RETVAL);                        \
    }


namespace abel {


DECLARE_ERROR(error, std::runtime_error)
DECLARE_ERROR(require_error, error)


extern int verbosity;


void dbg_(bool isError, int level, const char *funcName, int lineNo, const char *msg, ...);

unsigned long long randLL();

inline unsigned long long randLL(unsigned long long max) {
    return randLL() % max;
}


}


#endif // ACL_GENERAL_H
