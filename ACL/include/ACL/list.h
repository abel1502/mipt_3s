#ifndef ACL_LIST_H
#define ACL_LIST_H

#include <ACL/general.h>
#include <ACL/vector.h>
#include <utility>
#include <cstdlib>
#include <optional>
#include <initializer_list>
#include <iterator>
#include <cassert>


namespace abel {


template <typename T>
class list {
public:
    DECLARE_ERROR(error, abel::error);

    using idx_t = unsigned;
    static constexpr idx_t BAD_IDX = -1u;

    //--------------------------------------------------------------------------------
    // Node
    struct Node {
        idx_t prev = BAD_IDX;
        idx_t next = BAD_IDX;

        std::optional<T> value;  // TODO: My own implementation?


        constexpr Node() {}

        constexpr Node(const Node &other) = delete;
        constexpr Node &operator=(const Node &other) = delete;

        constexpr Node(Node &&other) = default;
        constexpr Node &operator=(Node &&other) = default;

        constexpr bool isFree() const noexcept { return prev == BAD_IDX; }
        constexpr bool isUsed() const noexcept { return value.has_value(); }

        constexpr const T &getVal() const {
            if (!isUsed())
                throw error("Can't retrieve value of unused item");

            return value.value();
        }

        constexpr T &getVal() {
            return const_cast<T &>(const_cast<const Node *>(this)->getVal());
        }

        template <typename ... As>
        constexpr T &emplaceVal(As &&... args) {
            return value.emplace(std::forward<As>(args)...);
        }

        constexpr void markFree(idx_t nextFree) noexcept {
            prev = BAD_IDX;
            next = nextFree;

            value.reset();
        }

        /// Has to be node with idx 0!!!
        constexpr void markRoot() noexcept {
            next = prev = 0;

            value.reset();
        }

        constexpr void markUsed(idx_t prev_, idx_t next_, const T &value_) noexcept {
            prev = prev_;
            next = next_;

            value = value;
        }

        constexpr void markUsed(idx_t prev_, idx_t next_, T &&value_) noexcept {
            prev = prev_;
            next = next_;

            value = std::move(value_);
        }

        template <typename ... As>
        constexpr void markUsed(idx_t prev_, idx_t next_, As &&... args) noexcept {
            prev = prev_;
            next = next_;

            emplaceVal(std::forward<As>(args)...);
        }
    };
    //--------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------
    // Iterators
    #define ACL_LIST_ITERATOR_TPL_GUARD

    #define NAME_ iterator
    #define CONST_
    #include <ACL/list_iterator.tpl.h>

    #define NAME_ const_iterator
    #define CONST_ const
    #define EXTRA_                              \
        constexpr NAME_(const iterator &iter) : \
            lst{iter.lst}, node{iter.node} {}
    #include <ACL/list_iterator.tpl.h>

    #undef ACL_LIST_ITERATOR_TPL_GUARD
    //--------------------------------------------------------------------------------


    list() :
        buf(1) {
        clear();
    }

    list(std::initializer_list<T> values) :
        list() {

        extendBack(values.begin(), values.end());
    }

    template <typename InputIt>
    list(InputIt &&begin_, InputIt &&end_) :
        list() {

        extendBack(std::forward<InputIt>(begin_), std::forward<InputIt>(end_));
    }

          iterator begin()       noexcept { return ++end(); }
    const_iterator begin() const noexcept { return ++end(); }
          iterator end  ()       noexcept { return iterator(this, &buf[0]); }
    const_iterator end  () const noexcept { return iterator(this, &buf[0]); }
    const T &front() const { return *++begin(); }
          T &front()       { return *++begin(); }
    const T &back () const { return *--begin(); }
          T &back ()       { return *--begin(); }

    void clear() {
        buf[0].markRoot();
        size = 0;
        firstFree = BAD_IDX;

        markRangeFree(1);
    }

    //--------------------------------------------------------------------------------
    // Inserts
    // TODO: Comments with actual eventual headers!
    #define DECLARE_INSERT_BODY_(TARGETS_INIT, MARK_USED_ARGS) {    \
        const idx_t idx = claimFree();                              \
        TARGETS_INIT                                                \
        if (target0 > buf.getSize() || target1 > buf.getSize())     \
            throw error("Target index out of range");               \
        buf[idx].markUsed(target0, target1, MARK_USED_ARGS);        \
        buf[target0].next = idx;                                    \
        buf[target1].prev = idx;                                    \
        size++;                                                     \
    }

    #define DIB_AFTER_(TARGET, MARK_USED_ARGS)                      \
        DECLARE_INSERT_BODY_(                                       \
            const idx_t target0 = (TARGET);                         \
            const idx_t target1 = buf[target0].next;,               \
            MARK_USED_ARGS                                          \
        )

    #define DIB_BEFORE_(TARGET, MARK_USED_ARGS)                     \
        DECLARE_INSERT_BODY_(                                       \
            const idx_t target1 = (TARGET);                         \
            const idx_t target0 = buf[target1].prev;,               \
            MARK_USED_ARGS                                          \
        )

    #define DECLARE_INSERTS_(NAME, CHECKS, BODY_MACRO, TARGET, ...) \
        void insert##NAME(__VA_ARGS__) {                            \
            CHECKS;                                                 \
            BODY_MACRO((TARGET), T{})                               \
        }                                                           \
                                                                    \
        void insert##NAME(__VA_ARGS__ __VA_OPT__ (,)                \
                          const T &value) {                         \
            CHECKS;                                                 \
            BODY_MACRO((TARGET), value)                             \
        }                                                           \
                                                                    \
        void insert##NAME(__VA_ARGS__ __VA_OPT__ (,)                \
                          T &&value) {                              \
            CHECKS;                                                 \
            BODY_MACRO((TARGET), std::move(value))                  \
        }                                                           \
                                                                    \
        template <typename ... Ts>                                  \
        void insert##NAME##Emplace(__VA_ARGS__ __VA_OPT__ (,)       \
                                   Ts &&... args) {                 \
            CHECKS;                                                 \
            BODY_MACRO((TARGET), std::forward<Ts>(args)...)         \
        }

    #define DECLARE_INSERTS_BIDIR_(NAME0, NAME1, CHECKS, TARGET, ...)       \
        DECLARE_INSERTS_(NAME0, CHECKS, DIB_BEFORE_, (TARGET), __VA_ARGS__) \
        DECLARE_INSERTS_(NAME1, CHECKS, DIB_AFTER_,  (TARGET), __VA_ARGS__)

    DECLARE_INSERTS_BIDIR_(Back, Front, , 0)

    DECLARE_INSERTS_BIDIR_(Before, After, demandValidIter(iter),
                           ptrToIdx(iter.node), const const_iterator &iter)

    DECLARE_INSERTS_BIDIR_(Before, After, demandValidIter(iter),
                           ptrToIdx(iter.node), const iterator &iter)

    protected:
    DECLARE_INSERTS_BIDIR_(Before, After, , node, idx_t node)
    public:

    #undef DECLARE_INSERTS_BIDIR_
    #undef DECLARE_INSERTS_
    #undef DIB_BEFORE_
    #undef DIB_AFTER_
    #undef DECLARE_INSERT_BODY_
    //--------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------
    // Extends
    // TODO: Comments with actual eventual headers!
    #define DEB_(NAME, FOR_CODE, ...) {         \
        for (FOR_CODE) {                        \
            insert##NAME(idx, ##__VA_ARGS__);   \
        }                                       \
    }

    #define DECLARE_EXTENDS_(NAME)                                              \
        template <typename ... As>                                              \
        inline void extend##NAME(const const_iterator &iter, As &&... args) {   \
            demandValidIter(iter);                                              \
            extend##NAME(ptrToIdx(iter.node), std::forward<As>(args)...);       \
        }                                                                       \
                                                                                \
        protected:                                                              \
        void extend##NAME(idx_t idx, unsigned count) {                          \
            DEB_(NAME, unsigned i = 0; i < count; ++i)                          \
        }                                                                       \
                                                                                \
        void extend##NAME(idx_t idx, unsigned count, const T &value) {          \
            DEB_(NAME, unsigned i = 0; i < count; ++i, value)                   \
        }                                                                       \
                                                                                \
        void extend##NAME(idx_t idx, std::initializer_list<T> values) {         \
            DEB_(NAME, auto val : values, std::move(val))                       \
        }                                                                       \
                                                                                \
        template <typename InputIt>                                             \
        void extend##NAME(idx_t idx, InputIt begin_, InputIt end_) {                  \
            DEB_(NAME, ; begin_ != end_; ++begin_, *begin_)                     \
        }                                                                       \
        public:

    #define DECLARE_EXTEND_BORDER_(NAME, UNDERLYING_NAME)           \
        template <typename ... As>                                  \
        inline void extend##NAME(As &&... args) {                   \
            extend##UNDERLYING_NAME(0, std::forward<As>(args)...);  \
        }

    DECLARE_EXTEND_BORDER_(Back, Before)
    DECLARE_EXTEND_BORDER_(Front, After)
    DECLARE_EXTENDS_(Before)
    DECLARE_EXTENDS_(After)

    #undef DECLARE_EXTEND_BORDER_
    #undef DECLARE_EXTENDS_
    #undef DEB_
    //--------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------
    // Erases
    inline void erase(const const_iterator &iter) { demandValidIter(iter); erase(ptrToIdx(iter.node)); }

    protected:
    void erase(idx_t idx) {
        assert(idx < buf.getSize());

        if (!buf[idx].isUsed()) {
            throw error("Can't erase an unused cell");
        }

        idx_t prev = buf[idx].prev;
        idx_t next = buf[idx].next;
        assert(prev != BAD_IDX && next != BAD_IDX);

        buf[next].prev = prev;
        buf[prev].next = next;
        returnUsed(idx);
    }
    public:
    //--------------------------------------------------------------------------------

    inline void swapElems(const const_iterator &iterA,
                          const const_iterator &iterB) {
        demandValidIter(iterA);
        demandValidIter(iterB);
        return swapElems(ptrToIdx(iterA.node), ptrToIdx(iterB.node));
    }

    inline void swapFront(const const_iterator &iter) {
        demandValidIter(iter);
        return swapElems(buf[0].next, ptrToIdx(iter.node));
    }

    inline void swapBack(const const_iterator &iter) {
        demandValidIter(iter);
        return swapElems(buf[0].prev, ptrToIdx(iter.node));
    }

    protected:
    void swapElems(idx_t idxA, idx_t idxB) {
        assert(idxA < buf.getSize() && idxB < buf.getSize());

        if (!buf[idxA].isUsed() || !buf[idxB].isUsed())
            throw error("Can't swap anything but used elements");

        if (idxA == idxB)
            return;

        idx_t prevA = buf[idxA].prev,
              prevB = buf[idxB].prev,
              nextA = buf[idxA].next,
              nextB = buf[idxB].next;

        std::swap(buf[prevA].next, buf[prevB].next);
        std::swap(buf[nextA].prev, buf[nextB].prev);
        std::swap(buf[ idxA].prev, buf[ idxB].prev);
        std::swap(buf[ idxA].next, buf[ idxB].next);
    }

    public:

    constexpr bool isEmpty() const noexcept {
        return size == 0;
    }

    constexpr unsigned getSize() const noexcept {
        return size;
    }

    bool validateIter(const const_iterator &iter) const {
        return iter.lst == this && buf.testWithin(iter.node);
    }

protected:
    vector<Node> buf;
    unsigned size = 0;
    idx_t firstFree = BAD_IDX;
    // bool inArrayMode = true;


    void ensureFree() {
        if (firstFree != BAD_IDX)
            return;

        buf.append();
    }

    void ensureFree(unsigned cnt) {
        idx_t lastFree = BAD_IDX;

        for (idx_t curFree = firstFree;
             curFree != BAD_IDX && cnt > 0;
             curFree = buf[curFree].next, --cnt) {

            lastFree = curFree;
        }

        if (cnt == 0)
            return;

        buf.extend(cnt);

        markRangeFree(buf.getSize() - cnt, BAD_IDX, lastFree);
    }

    void markRangeFree(idx_t from, idx_t to = BAD_IDX, idx_t lastFree = BAD_IDX) {
        if (to == BAD_IDX) {
            to = buf.getSize();
        }

        if (buf.getSize() <= from || from <= to)
            return;

        if (lastFree != BAD_IDX) {
            assert(buf[lastFree].isFree() && buf[lastFree].next == BAD_IDX);
            buf[lastFree].markFree(from);
            buf[to - 1].markFree(BAD_IDX);
        } else {
            buf[to - 1].markFree(firstFree);
            firstFree = from;
        }

        for (idx_t i = from; i < to - 1; ++i) {
            buf[i].markFree(i + 1);
        }
    }

    idx_t claimFree() {
        ensureFree();

        idx_t result = firstFree;
        firstFree = buf[firstFree].next;
        return result;
    }

    void returnUsed(idx_t idx) {
        buf[idx].markFree(firstFree);
        firstFree = idx;
    }

    inline idx_t refToIdx(const Node &node) const {
        return buf.refToIdx(node);
    }

    inline idx_t ptrToIdx(const Node *node) const {
        return buf.ptrToIdx(node);
    }

    inline void demandValidIter(const const_iterator &iter) {
        if (!validateIter(iter))
            throw error("Corrupt iterator");
    }

};


}


#endif // ACL_LIST_H
