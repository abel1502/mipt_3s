#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <vector>
#include <algorithm>
#include <map>
#include <type_traits>


#if 0
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
        std::vector<unsigned> dictIdxs;
        unsigned len;


        Node(idx_t new_parent = 0, char new_data = 0, unsigned new_len = 0) :
            parent{new_parent}, children{}, isTerm{0}, data{new_data},
            link{BAD_IDX}, dictIdxs{}, len{new_len} {}

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

    inline void insert(const char *str, unsigned dictIdx) {
        insert(str, strlen(str), dictIdx);
    }

    void insert(const char *str, const unsigned length, unsigned dictIdx) {
        idx_t curIdx = 0;

        for (unsigned i = 0; i < length; ++i) {
            idx_t child = buf[curIdx].getChild(str[i]);

            if (child == BAD_IDX) {
                buf.push_back(Node(curIdx, str[i], i + 1));
                child = buf.size() - 1;

                buf[curIdx].children[str[i]] = child;
            }

            curIdx = child;
        }

        buf[curIdx].isTerm++;
        buf[curIdx].dictIdxs.push_back(dictIdx);

        assert(buf[curIdx].len == length);
    }

    std::vector<char> rebuidStr(idx_t idx) {
        std::vector<char> result;

        while (idx) {
            result.push_back(buf[idx].data);

            idx = buf[idx].parent;
        }

        std::reverse(result.begin(), result.end());
        result.push_back('\0');

        return result;
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

};


int main() {
    constexpr unsigned STR_SIZE_LIMIT = 1000001;
    constexpr char STR_FMT[] = "%1000000s";

    Trie trie;

    std::vector<char> str(STR_SIZE_LIMIT, '\0');
    int res = scanf(STR_FMT, str.data());
    assert(res == 1);

    unsigned n = 0;
    res = scanf("%u", &n);
    assert(res == 1);

    {
        std::vector<char> buf(STR_SIZE_LIMIT, '\0');
        for (unsigned i = 0; i < n; ++i) {
            res = scanf(STR_FMT, buf.data());
            assert(res == 1);

            trie.insert(buf.data(), i);
        }
    }

    std::vector<std::vector<unsigned>> ans(n);
    unsigned length = strlen(str.data());
    Trie::idx_t curIdx = 0;

    for (unsigned i = 0; i < length; ++i) {
        curIdx = trie.getNext(curIdx, str[i]);

        DBG("loop");

        for (Trie::idx_t matchIdx = curIdx; matchIdx; matchIdx = trie.getLink(matchIdx)) {
            DBG("loop %u", matchIdx);

            if (!trie.buf[matchIdx].isTerm)
                continue;

            unsigned matchLen = trie.buf[matchIdx].len;

            for (unsigned dictIdx : trie.buf[matchIdx].dictIdxs) {
                ans[dictIdx].push_back(i - matchLen + 1);
            }
        }
    }

    for (auto i : ans) {
        printf("%zu ", i.size());

        for (unsigned item : i) {
            printf("%u ", item + 1);
        }

        printf("\n");
    }

    return 0;
}

