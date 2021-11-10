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


template <typename T, typename = void>
struct arg_type {
    using type = T &;
};

template <typename T>
struct arg_type<T, std::enable_if_t<std::is_fundamental_v<T> ||
                                    std::is_pointer_v    <T> ||
                                    std::is_reference_v  <T>>> {
    using type = T;
};

template <typename T>
using arg_type_t = typename arg_type<T>::type;


}


#endif // ACL_TYPE_TRAITS
