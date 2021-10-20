#ifndef ACL_UNIQUE_PTR_H
#define ACL_UNIQUE_PTR_H

#include <ACL/general.h>


namespace abel {


// TODO: Something like a deleter?
template <typename T>
class unique_ptr {
public:
    DECLARE_ERROR(error, abel::error)

    using type    = T;
    using pointer = T *;


    constexpr unique_ptr() noexcept :
        ptr{nullptr} {}

    constexpr unique_ptr(std::nullptr_t) noexcept :
        ptr{nullptr} {}

    constexpr unique_ptr(pointer ptr_) noexcept :
        ptr{ptr_} {}

    unique_ptr(const unique_ptr &other) = delete;
    unique_ptr &operator=(const unique_ptr &other) = delete;

    constexpr unique_ptr(unique_ptr &&other) :
        ptr{other.ptr} {

        other.ptr = nullptr;
    }

    constexpr unique_ptr &operator=(unique_ptr &&other) {
        std::swap(ptr, other.ptr);
    }

    ~unique_ptr() {
        sizeof(T);
        // TODO: Because of this we need a deleter!!!
        delete ptr;
    }

    constexpr type &operator*() const {
        if (!ptr)
            throw error("null pointer dereferencing");

        return *ptr;
    }

    constexpr pointer operator->() const noexcept {
        return ptr;
    }

    constexpr operator bool() const noexcept {
        return ptr;
    }

    constexpr pointer get() const noexcept {
        return ptr;
    }

    constexpr pointer release() noexcept {
        pointer result = ptr;
        ptr = 0;
        return ptr;
    }

    void reset(pointer newPtr = nullptr) {
        pointer oldPtr = ptr;
        ptr = newPtr;

        if (oldPtr)
            delete oldPtr;
    }

    constexpr void swap(unique_ptr &other) noexcept {
        std::swap(ptr, other.ptr);
    }

    template <typename ... As>
    static unique_ptr emplace(As &&... args) {
        return unique_ptr(new T(std::forward<As>(args)...));
    }

    // TODO: Comparisons?

protected:
    pointer ptr;

};


// TODO: Implement
template <typename T>
class unique_ptr<T[]>;


}


template <typename T>
constexpr void std::swap(abel::unique_ptr<T> &a, abel::unique_ptr<T> &b) noexcept {
    a.swap(b);
}


#endif // ACL_UNIQUE_PTR_H
