#ifndef ACL_GENERAL_H
#define ACL_GENERAL_H


#define MACROFUNC(...) do {__VA_ARGS__} while (0)


#define ERR(msg, ...) abel::dbg_(true,  1, __func__, __LINE__, msg, ##__VA_ARGS__)
#define DBG(msg, ...) abel::dbg_(false, 2, __func__, __LINE__, msg, ##__VA_ARGS__)


#define DECLARE_ERROR(NAME, PARENT)                 \
    class NAME : PARENT {                           \
    public:                                         \
        NAME(std::string &msg) : PARENT(msg) {}     \
        NAME(const char *msg)  : PARENT(msg) {}     \
    };


#define REQUIRE(STMT) MACROFUNC(                        \
    if (!(STMT)) {                                      \
        ERR("Requirement not satisfied: %s\n", #STMT);  \
        throw new abel::require_error("");              \
    })


#define NODEFAULT                       \
    default:                            \
        ERR("Shouldn't be reachable");  \
        abort();


namespace abel {


extern int verbosity;

void dbg_(bool isError, int level, const char *funcName, int lineNo, const char *msg, ...);

unsigned long long randLL();


}


#endif // ACL_GENERAL_H
