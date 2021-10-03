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
        idx_t parent;
        std::map<char, idx_t> children;
        unsigned isTerm;  /// the number of words ending here, since duplicates seem to be a thing
        char data;
        idx_t link;
        unsigned numGood;
        unsigned cachedNumGood;


        Node(idx_t new_parent = 0, char new_data = 0) :
            parent{new_parent}, children{}, isTerm{0}, data{new_data},
            link{BAD_IDX}, numGood{0}, cachedNumGood{0} {}

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

    bool hasTermSuff(idx_t idx) {
        return idx && (buf[idx].isTerm || hasTermSuff(getLink(idx)));
    }

    void iterate(unsigned modulo) {
        cacheNumGoods();

        for (idx_t idx = 0; idx < buf.size(); ++idx) {
            if (hasTermSuff(idx))
                continue;

            for (char transition = 'a'; transition <= 'z'; ++transition) {
                idx_t nextIdx = getNext(idx, transition);

                buf[nextIdx].numGood = (buf[nextIdx].numGood + buf[idx].cachedNumGood) % modulo;
            }
        }
    }

    unsigned countAll(unsigned modulo) {
        unsigned sum = 0;

        for (idx_t idx = 0; idx < buf.size(); ++idx) {
            if (hasTermSuff(idx))
                continue;

            sum = (sum + buf[idx].numGood) % modulo;
        }

        return sum;
    }

protected:
    // Could have been done by a 2-element array, but copying cost isn't that high here anyway.
    void cacheNumGoods() {
        for (Node &node : buf) {
            node.cachedNumGood = node.numGood;
            node.numGood = 0;
        }
    }

};


int main() {
    constexpr unsigned STR_SIZE_LIMIT = 11;
    constexpr char STR_FMT[] = "%10s";
    constexpr unsigned MODULO = 10000;

    Trie trie;

    unsigned n = 0, k = 0;
    int res = scanf("%u %u", &n, &k);
    assert(res == 2);

    std::vector<char> buf(STR_SIZE_LIMIT, '\0');
    for (unsigned i = 0; i < k; ++i) {
        res = scanf(STR_FMT, buf.data());
        assert(res == 1);

        trie.insert(buf.data());
    }

    trie.buf[0].numGood = 1;  // Root is good

    unsigned total = 1;
    for (unsigned i = 0; i < n; ++i) {
        total = (total * 26) % MODULO;
        trie.iterate(MODULO);
    }

    printf("%u\n", (total + MODULO - trie.countAll(MODULO)) % MODULO);

    return 0;
}



