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


        Node(idx_t new_parent = 0, char new_data = 0) :
            parent{new_parent}, children{}, isTerm{0}, data{new_data} {}

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
                DBG("link %u (%c)-> %u", curIdx, str[i], child);
            }

            curIdx = child;
        }

        buf[curIdx].isTerm++;
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

    unsigned output(idx_t idx, const std::vector<unsigned> &dotCount, unsigned curDotsIdx = 0) {
        for (unsigned rep = 0; rep < buf[idx].isTerm; ++rep) {
            for (unsigned i = 0; i < dotCount[curDotsIdx]; ++i) {
                putchar('.');
            }
            curDotsIdx++;

            printf("%s", rebuidStr(idx).data());
        }

        for (auto child : buf[idx].children) {
            curDotsIdx = output(child.second, dotCount, curDotsIdx);
        }

        return curDotsIdx;
    }
};


int main() {
    constexpr unsigned STR_SIZE_LIMIT = 1000001;
    constexpr char STR_FMT[] = "%1000000s";

    Trie trie;

    std::vector<char> str(STR_SIZE_LIMIT, '\0');
    int res = scanf(STR_FMT, str.data());
    assert(res == 1);

    unsigned length = strlen(str.data());

    char *curStr = str.data();
    unsigned curLen = 0;

    std::vector<unsigned> dotCount{};
    dotCount.push_back(0);

    for (unsigned i = 0; i < length; ++i) {
        if (str[i] == '.') {
            if (i > 0 && dotCount.back() == 0) {
                trie.insert(curStr, curLen);
                curLen = 0;
            }

            curStr = str.data() + i + 1;

            dotCount.back()++;

            continue;
        }

        if (i == 0 || dotCount.back())
            dotCount.push_back(0);

        curLen++;
    }

    if (!dotCount.back()) {
        trie.insert(curStr, curLen);
    }

    dotCount.push_back(0);

    for (auto i : dotCount) {
        DBG("dotCount %u", i);
    }

    unsigned idx = trie.output(0, dotCount);
    DBG("last dotCount %u", dotCount[idx]);
    for (unsigned i = 0; i < dotCount[idx]; ++i) {
        putchar('.');
    }
    puts("");

    return 0;
}
