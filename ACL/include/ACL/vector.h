#ifndef ACL_VECTOR_H
#define ACL_VECTOR_H


#include <ACL/general.h>

#include <algorithm>
#include <initializer_list>
#include <cassert>
#include <cstdlib>
#include <memory>


namespace abel {


template <typename T>
class vector {
public:
    DECLARE_ERROR(error, abel::error)

    using iterator = T *;
    using const_iterator = const T *;

    static constexpr unsigned DEFAULT_CAPACITY = 4;

    vector() :
        buf{nullptr},
        size{0},
        capacity{0} {

        reserve(DEFAULT_CAPACITY);
    }

    vector(std::initializer_list<T> values) :
        buf{nullptr},
        size{0},
        capacity{0} {

        reserve(values.size());

        extend(values);
    }

    vector(unsigned new_size) :
        vector() {

        reserve(new_size);
        size = new_size;
    }

    vector(unsigned new_size, const T &value) :
        vector() {

        reserve(new_size);
        for (unsigned i = 0; i < new_size; ++i) {
            append(value);
        }
    }

    vector(const vector &other) :
        vector() {

        reserve(other.capacity);
        for (unsigned i =0; i < other.size; ++i) {
            append(other[i]);
        }
    }

    vector &operator=(const vector &other) {
        destroy();

        reserve(other.capacity);

        for (unsigned i =0; i < other.size; ++i) {
            append(other[i]);
        }

        return *this;
    }

    vector(vector &&other) noexcept {
        std::swap(buf, other.buf);
        std::swap(size, other.size);
        std::swap(capacity, other.capacity);
    }

    vector &operator=(vector &&other) noexcept {
        std::swap(buf, other.buf);
        std::swap(size, other.size);
        std::swap(capacity, other.capacity);

        return *this;
    }

    ~vector() noexcept {
        destroy();
    }

    T &operator[](int idx) {
        if ((int)-size > idx || idx >= (int)size)
            throw error("Vector subscript by illegal index");

        idx = (idx + size) % size;

        return buf[idx];
    }

    const T &operator[](int idx) const {
        return const_cast<vector *>(this)->operator[](idx);
    }

    /// The following methods return whether the buffer was moved

    #define DECLARE_APPEND_BODY_(CTOR_ARGS) {           \
        bool moved = false;                             \
                                                        \
        if (size >= capacity) {                         \
            moved = reserve(capacity * 2);              \
                                                        \
            assert(capacity > size);                    \
        }                                               \
                                                        \
        T *newSpot = new (&buf[size++]) T(CTOR_ARGS);   \
        assert(newSpot == &buf[size - 1]);              \
                                                        \
        return moved;                                   \
    }

    bool append() {
        DECLARE_APPEND_BODY_()
    }

    bool append(const T &value) {
        DECLARE_APPEND_BODY_(value)
    }

    bool append(T &&value) {
        DECLARE_APPEND_BODY_(std::move(value))
    }

    template <typename ... Ts>
    bool appendEmplace(Ts ... args) {
        DECLARE_APPEND_BODY_(args...)
    }

    #undef DECLARE_APPEND_BODY_

    #define DECLARE_EXTEND_BODY_(FOR_CODE, APPEND_CALL) {       \
        bool moved = false;                                     \
                                                                \
        for (FOR_CODE) {                                        \
            /* Intentionally not boolean or, because we   */    \
            /* don't want shortcutting to take place here */    \
            moved |= !!(APPEND_CALL);                           \
        }                                                       \
                                                                \
        return moved;                                           \
    }

    bool extend(unsigned count) {
        DECLARE_EXTEND_BODY_(unsigned i = 0; i < count; ++i, appendEmplace())
    }

    bool extend(std::initializer_list<T> values) {
        DECLARE_EXTEND_BODY_(const T &val : values, append(val))
    }

    #undef DECLARE_EXTEND_BODY_

    T popVal() {
        if (isEmpty())
            throw error("Pop from empty vector");

        assert(buf);

        T result = std::move(buf[--size]);

        buf[size].~T();

        return result;
    }

    void pop() {
        if (isEmpty())
            throw error("Pop from empty vector");

        assert(buf);

        buf[--size].~T();
    }

    bool isEmpty() const noexcept {
        return size == 0;
    }

    unsigned getSize() const noexcept {
        return size;
    }

    unsigned getCapacity() const noexcept {
        return capacity;
    }

    /// Warning: Don't save the pointer!
    const T *getBuf() const noexcept {
        return buf;
    }

    /// Warning: Don't save the pointer!
    T *getBuf() noexcept {
        return buf;
    }

    constexpr const_iterator begin() const noexcept {
        return buf;
    }

    constexpr iterator begin() noexcept {
        return buf;
    }

    constexpr const_iterator end() const noexcept {
        return buf + size;
    }

    constexpr iterator end() noexcept {
        return buf + size;
    }

    /// Returns whether the buffer was moved
    bool reserve(unsigned new_capacity) {
        if (new_capacity == 0)
            new_capacity = DEFAULT_CAPACITY;

        if (new_capacity <= capacity)
            return false;

        T *newBuf = (T *)calloc(new_capacity, sizeof(T));

        if (!newBuf) {
            throw error("Buffer allocation failed");
        }

        if (buf) {
            for (unsigned i = 0; i < size; ++i) {
                newBuf[i] = std::move(buf[i]);
                buf[i].~T();
            }

            free(buf);
        }

        buf = newBuf;
        capacity = new_capacity;

        return true;
    }

    void clear() {
        destroy();
        reserve(DEFAULT_CAPACITY);
    }

protected:
    T *buf;
    unsigned size;
    unsigned capacity;

    void destroy() noexcept {
        if (buf) {
            for (unsigned i = 0; i < size; ++i) {
                buf[i].~T();
            }

            free(buf);
        }

        buf = nullptr;
        size = 0;
        capacity = 0;
    }

};

}


#endif // ACL_VECTOR_H
