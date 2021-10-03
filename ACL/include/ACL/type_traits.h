#ifndef ACL_TYPE_TRAITS
#define ACL_TYPE_TRAITS

#include <type_traits>


namespace abel {


#define CHANGE_SIGNEDNESS_(FROM, TO)                                \
    template <typename T, typename = void>                          \
    struct make_##TO {                                              \
        using type = T;                                             \
    };                                                              \
                                                                    \
    template <typename T>                                           \
    struct make_##TO<T, std::enable_if_t<std::is_##FROM##_v<T>>> {  \
        using type = std::make_##TO##_t<T>;                         \
    };                                                              \
                                                                    \
    template <typename T>                                           \
    using make_##TO##_t = typename make_##TO<T>::type;

CHANGE_SIGNEDNESS_(unsigned, signed)

CHANGE_SIGNEDNESS_(signed, unsigned)

#undef CHANGE_SIGNEDNESS_


}


#endif // ACL_TYPE_TRAITS
