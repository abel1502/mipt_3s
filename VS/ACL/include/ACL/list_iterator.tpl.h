#if !defined(ACL_LIST_ITERATOR_TPL_GUARD) || !defined(NAME_) || !defined(CONST_)
#error "DON'T use this file in your own projects!!! Use list.h instead"
#endif

#ifndef EXTRA_
#define EXTRA_
#endif

class NAME_ : public std::iterator<std::bidirectional_iterator_tag , CONST_ T> {
    using _base = std::iterator<std::bidirectional_iterator_tag , CONST_ T>;
public:
    using typename _base::iterator_category;
    using typename _base::value_type;
    using typename _base::difference_type;
    using typename _base::pointer;
    using typename _base::reference;


    constexpr NAME_() :
        lst{nullptr}, node{nullptr} {}

    constexpr NAME_(CONST_ list *lst_, CONST_ Node *node_) :
        lst{lst_}, node{node_} {

        assert(lst && node);
    }

    NAME_ &operator++() {
        assert(lst && node);
        idx_t nextIdx = node->next;
        REQUIRE(nextIdx != BAD_IDX);  // The list is corrupt otherwise
        node = &lst->buf[nextIdx];
        return *this;
    }

    NAME_ operator++(int) {
        assert(lst && node);
        NAME_ backup = *this;
        ++*this;
        return backup;
    }

    NAME_ &operator--() {
        assert(lst && node);
        if (node->isFree()) {
            throw error("Backwards iteration isn't supported for free nodes");
        }

        idx_t prevIdx = node->prev;
        REQUIRE(prevIdx != BAD_IDX);  // The list is corrupt otherwise
        node = &lst->buf[prevIdx];
        return *this;
    }

    NAME_ operator--(int) {
        assert(lst && node);
        NAME_ backup = *this;
        --*this;
        return backup;
    }

    reference operator*() CONST_ {
        assert(lst && node);
        return node->getVal();
    }

    pointer operator->() CONST_ {
        assert(lst && node);
        return &**this;  // I know this looks weird, but trust me)
    }

    constexpr bool operator==(const NAME_ &other) const {
        return node == other.node;
    }

    constexpr bool operator!=(const NAME_ &other) const {
        return !(*this == other);
    }

    EXTRA_

protected:
    CONST_ list *lst;
    CONST_ Node *node;

    friend class list;

};

friend class NAME_;

#undef NAME_
#undef CONST_
#undef EXTRA_
