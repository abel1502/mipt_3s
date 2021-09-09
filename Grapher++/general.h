#ifndef GENERAL_H_GUARD
#define GENERAL_H_GUARD


#define ERR(msg, ...) dbg_(true,  1, __func__, __LINE__, msg, ##__VA_ARGS__)
#define DBG(msg, ...) dbg_(false, 2, __func__, __LINE__, msg, ##__VA_ARGS__)


#define TMPVARNAME$_HELPER(A, B)    A ## B
#define TMPVARNAME  TMPVARNAME$_HELPER(tmp_, __LINE__)

// The TRY macro is intended for use to forward any error (i.e. non-zero) return code
// from an expression out of the context function. Useful for work with ctor's in the
// new format.
#define TRY(STMT)  TRY_C(STMT, )

// Try not (for when success is represented with a non-zero value - a pointer, for example)
#define TRY_N(STMT)  TRY_NC(STMT, )

// Boolean try
#define TRY_B(STMT)  TRY_BC(STMT, )

// Try with cleanup
#define TRY_C(STMT, CLEANUP)  {                         \
    auto TMPVARNAME = (STMT);                           \
    if (TMPVARNAME) {                                   \
        if (verbosity >= 3) {                           \
            ERR("Error caught in \"%s\"", #STMT);       \
        }                                               \
        CLEANUP;                                        \
        return TMPVARNAME;                              \
    }                                                   \
}

// Try not with cleanup
#define TRY_NC(STMT, CLEANUP)  {                        \
    auto TMPVARNAME = (STMT);                           \
    if (!TMPVARNAME) {                                  \
        if (verbosity >= 3) {                           \
            ERR("Error caught in \"%s\"", #STMT);       \
        }                                               \
        CLEANUP;                                        \
        return TMPVARNAME;                              \
    }                                                   \
}

// Boolean try with cleanup
#define TRY_BC(STMT, CLEANUP)  {    \
    if (STMT) {                     \
        CLEANUP;                    \
        return true;                \
    }                               \
}


// TODO: Throw
#define REQUIRE(STMT)                                   \
    if (!(STMT)) {                                      \
        ERR("Requirement not satisfied: %s\n", #STMT);  \
        abort();                                        \
    }


#define NODEFAULT                       \
    default:                            \
        ERR("Shouldn't be reachable");  \
        abort();


extern int verbosity;

void dbg_(bool isError, int level, const char *funcName, int lineNo, const char *msg, ...);

unsigned long long randLL();


#endif // GENERAL_H_GUARD

