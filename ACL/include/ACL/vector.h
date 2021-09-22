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

        resize(DEFAULT_CAPACITY);
    }

    vector(std::initializer_list<T> values) :
        buf{nullptr},
        size{0},
        capacity{0} {

        resize(values.size());

        extend(values);
    }

    vector(unsigned new_size) :
        vector() {

        resize(new_size);
        size = new_size;
    }

    vector(unsigned new_size, const T &value) :
        vector() {

        resize(new_size);
        for (unsigned i = 0; i < new_size; ++i) {
            append(value);
        }
    }

    vector(const vector &other) :
        vector() {

        resize(other.capacity);
        for (unsigned i =0; i < other.size; ++i) {
            append(other[i]);
        }
    }

    vector &operator=(const vector &other) {
        destroy();

        resize(other.capacity);

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

    void append() {
        if (size >= capacity) {
            resize(capacity * 2);

            assert(capacity > size);
        }

        T *newSpot = new (&buf[size++]) T();
        assert(newSpot == &buf[size - 1]);
    }

    void append(const T &value) {
        if (size >= capacity) {
            resize(capacity * 2);

            assert(capacity > size);
        }

        T *newSpot = new (&buf[size++]) T(value);
        assert(newSpot == &buf[size - 1]);
    }

    void append(const T &&value) {
        if (size >= capacity) {
            resize(capacity * 2);

            assert(capacity > size);
        }

        T *newSpot = new (&buf[size++]) T(std::move(value));
        assert(newSpot == &buf[size - 1]);
    }

    void extend(unsigned count) {
        for (unsigned i = 0; i < count; ++i) {
            append(T{});
        }
    }

    void extend(std::initializer_list<T> values) {
        for (const T &val : values) {
            append(val);
        }
    }

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

protected:
    T *buf;
    unsigned size;
    unsigned capacity;

    void resize(unsigned new_capacity) {
        if (new_capacity == 0)
            new_capacity = DEFAULT_CAPACITY;

        if (new_capacity <= capacity)
            return;

        T *newBuf = (T *)calloc(new_capacity, sizeof(T));

        if (buf) {
            for (unsigned i = 0; i < size; ++i) {
                newBuf[i] = std::move(buf[i]);
                buf[i].~T();
            }

            free(buf);
        }

        buf = newBuf;
        capacity = new_capacity;
    }

    void destroy() noexcept {
        for (unsigned i = 0; i < size; ++i) {
            buf[i].~T();
        }
        free(buf);

        buf = nullptr;
        size = 0;
        capacity = 0;
    }

};

}


#endif // ACL_VECTOR_H
