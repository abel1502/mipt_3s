#ifndef ACL_UNIQUE_PTR_H
#define ACL_UNIQUE_PTR_H

#include <ACL/general.h>
#include <memory>  // For default_delete


namespace abel {


template <typename T, typename Deleter = std::default_delete<T>>
class unique_ptr {
public:
    DECLARE_ERROR(error, abel::error)

    using type         = T;
    using deleter_type = Deleter;
    using pointer      = T *;


    constexpr unique_ptr() noexcept :
        ptr{nullptr} {}

    constexpr unique_ptr(std::nullptr_t) noexcept :
        ptr{nullptr} {}

    constexpr unique_ptr(pointer ptr_) noexcept :
        ptr{ptr_} {}

    unique_ptr(const unique_ptr &other) = delete;
    unique_ptr &operator=(const unique_ptr &other) = delete;

    constexpr unique_ptr(unique_ptr &&other) noexcept :
        ptr{other.ptr} {

        other.ptr = nullptr;
    }

    constexpr unique_ptr &operator=(unique_ptr &&other) noexcept {
        std::swap(ptr, other.ptr);

        return *this;
    }

    constexpr unique_ptr &operator=(std::nullptr_t) {
        reset();

        return *this;
    }

    constexpr unique_ptr &operator=(pointer ptr_) {
        reset(ptr_);

        return *this;
    }

    ~unique_ptr() {
        get_deleter()(ptr);
        // delete ptr;
        ptr = nullptr;
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

    static constexpr deleter_type get_deleter() noexcept { return deleter_type{}; }
    // constexpr const deleter_type &get_deleter() const noexcept { return deleter_type{}; }
    // constexpr       deleter_type &get_deleter()       noexcept { return deleter_type{}; }

    constexpr pointer release() noexcept {
        pointer result = ptr;
        ptr = 0;
        return result;
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
    void emplace(As &&... args) {
        reset(new T(std::forward<As>(args)...));
    }

    template <typename ... As>
    static unique_ptr createEmplace(As &&... args) {
        return unique_ptr(new T(std::forward<As>(args)...));
    }

    // TODO: Comparisons?

protected:
    pointer ptr;

};


// TODO: Implement
template <typename T, typename Deleter>
class unique_ptr<T[], Deleter>;


}


template <typename T>
constexpr void std::swap(abel::unique_ptr<T> &a, abel::unique_ptr<T> &b) noexcept {
    a.swap(b);
}


#endif // ACL_UNIQUE_PTR_H