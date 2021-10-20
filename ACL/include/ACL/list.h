#ifndef ACL_LIST_H
#define ACL_LIST_H

#include <ACL/general.h>
#include <ACL/vector.h>
#include <utility>
#include <cstdlib>
#include <optional>

#pragma GCC error "unfinished"


namespace abel {


template <typename T>
class list {
public:
    DECLARE_ERROR(error, abel::error);

    using idx_t = unsigned;


    list() :
        buf{} {}

protected:
    static constexpr idx_t BAD_IDX = -1u;

    enum NodeType {
        T_ROOT,
        T_FREE,
        T_USED,
    };

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

            value = std::move(value);
        }
    };


    vector<Node> buf;

};


}


#endif // ACL_LIST_H
