#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <vector>
#include <algorithm>
#include <map>
#include <type_traits>


#if 1
#define DBG(FMT, ...) \
    printf("[%s#%d: " FMT "]\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define DBG(FMT, ...)
#endif


struct Trie {
    using idx_t = unsigned;
    static constexpr idx_t BAD_IDX = (idx_t)-1;

    struct Node {
        enum Type {
            T_UNKNOWN,
            T_BAD,
            T_GOOD,
        };

        idx_t parent;
        std::map<char, idx_t> children;
        unsigned isTerm;  /// the number of words ending here, since duplicates seem to be a thing
        char data;
        idx_t link;
        Type type;
        unsigned dfsState;


        Node(idx_t new_parent = 0, char new_data = 0) :
            parent{new_parent}, children{}, isTerm{0}, data{new_data},
            link{BAD_IDX}, type{T_UNKNOWN}, dfsState{0} {}

        inline bool hasChild(char chr) {
            return children.find(chr) != children.end();
        }

        inline idx_t getChild(char chr) {
            auto it = children.find(chr);

            return it != children.end() ? it->second : BAD_IDX;
        }
    };

    std::vector<Node> buf;


    Trie() :
        buf(1) {}

    inline bool isRealChild(idx_t parent, idx_t child) {
        return buf[child].parent == parent;
    }

    inline void insert(const char *str) {
        insert(str, strlen(str));
    }

    void insert(const char *str, const unsigned length) {
        idx_t curIdx = 0;

        for (unsigned i = 0; i < length; ++i) {
            idx_t child = buf[curIdx].getChild(str[i]);

            if (child == BAD_IDX) {
                buf.push_back(Node(curIdx, str[i]));
                child = buf.size() - 1;

                buf[curIdx].children[str[i]] = child;
            }

            curIdx = child;
        }

        buf[curIdx].isTerm++;
        buf[curIdx].type = Node::T_BAD;
    }

    idx_t getLink(idx_t idx) {
        if (buf[idx].link == BAD_IDX) {
            if (idx == 0 || buf[idx].parent == 0) {
                buf[idx].link = 0;
            } else {
                buf[idx].link = getNext(getLink(buf[idx].parent), buf[idx].data);
            }
        }

        return buf[idx].link;
    }

    idx_t getNext(idx_t idx, char chr) {
        idx_t next = buf[idx].getChild(chr);

        if (next == BAD_IDX) {
            if (idx == 0) {
                next = 0;
            } else {
                next = getNext(getLink(idx), chr);
            }

            buf[idx].children[chr] = next;
        }

        return next;
    }

    bool validateNode(idx_t idx) {
        if (idx == 0)
            return true;

        if (buf[idx].type == Node::T_BAD)
            return false;

        return validateNode(getLink(idx));
    }

    bool testCyclability(idx_t idx) {
        switch (buf[idx].dfsState) {
        case 1:
            return true;

        case 2:
            assert(buf[idx].type != Node::T_UNKNOWN);
            return buf[idx].type == Node::T_GOOD;

        default:
            break;
        }

        //assert(buf[idx].type == Node::T_UNKNOWN);
        if (buf[idx].type != Node::T_UNKNOWN) {
            buf[idx].dfsState = 2;
            return buf[idx].type == Node::T_GOOD;
        }

        buf[idx].dfsState = 1;

        if (!validateNode(idx)) {
            buf[idx].dfsState = 2;
            buf[idx].type = Node::T_BAD;

            return false;
        }

        for (char c = '0'; c <= '1'; ++c) {
            if (testCyclability(getNext(idx, c))) {
                buf[idx].dfsState = 2;
                buf[idx].type = Node::T_GOOD;

                return true;
            }
        }

        buf[idx].dfsState = 2;
        buf[idx].type = Node::T_BAD;

        return false;
    }

};


int main() {
    constexpr unsigned STR_SIZE_LIMIT = 1000001;
    constexpr char STR_FMT[] = "%1000000s";

    Trie trie;

    unsigned n = 0;
    int res = scanf("%u", &n);
    assert(res == 1);

    std::vector<char> buf(STR_SIZE_LIMIT, '\0');
    for (unsigned i = 0; i < n; ++i) {
        res = scanf(STR_FMT, buf.data());
        assert(res == 1);

        trie.insert(buf.data());
    }

    printf("%s\n", trie.testCyclability(0) ? "TAK" : "NIE");

    return 0;
}


