#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <set>
#include <type_traits>
#include <iostream>


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
        std::set<idx_t> curDNAs;  // the DNAs that have stopped here


        Node(idx_t new_parent = 0) :
            parent{new_parent}, children{}, curDNAs{} {}

        inline bool hasChild(char chr) {
            return children.find(chr) != children.end();
        }

        inline idx_t getChild(char chr) {
            auto it = children.find(chr);

            return it != children.end() ? it->second : BAD_IDX;
        }

        inline bool isTerm() const {
            return !children.size();
        }
    };

    struct DNA {
        idx_t idx;
        std::string value;
        unsigned pos;
        idx_t curNode;

        DNA(idx_t idx_, const std::string &value_) :
            idx{idx_}, value{value_}, pos{0}, curNode{0} {}

        DNA(idx_t idx_, std::string &&value_) :
            idx{idx_}, value{std::move(value_)}, pos{0}, curNode{0} {}

        inline char curSym() const {
            return value[pos];
        }
    };


    std::vector<Node> buf;
    std::vector<DNA> DNAs;


    Trie() :
        buf(1), DNAs{} {}

    bool tickDNA(idx_t dnaIdx) {
        DNA &dna = DNAs[dnaIdx];
        assert(dnaIdx == dna.idx);

        idx_t curNode = dna.curNode;

        buf[curNode].curDNAs.erase(dnaIdx);

        for (; dna.pos < dna.value.length();
             curNode = buf[curNode].getChild(dna.curSym()), dna.pos++) {

            if (buf[curNode].isTerm() && curNode)
                curNode = 0;

            if (!buf[curNode].hasChild(dna.curSym()))
                break;
        }

        dna.curNode = curNode;

        if (dna.pos == dna.value.length()) {
            return buf[curNode].isTerm();
        }

        buf[curNode].curDNAs.insert(dnaIdx);

        return false;
    }

    std::vector<idx_t> addDNA(std::string &&str) {
        idx_t idx = DNAs.size();
        DNAs.emplace_back(idx, std::move(str));
        buf[0].curDNAs.insert(idx);

        std::vector<idx_t> ans{};

        if (tickDNA(idx))
            ans.push_back(idx);

        return ans;
    }

    inline std::vector<idx_t> addDNA(const std::string &str) {
        return addDNA(str);
    }

    std::vector<idx_t> addGene(const std::string &str) {
        std::vector<idx_t> ans{};

        idx_t curNode = 0;

        for (char chr : str) {
            idx_t child = buf[curNode].getChild(chr);

            if (child == BAD_IDX) {
                buf.push_back(Node(curNode));
                child = buf.size() - 1;

                buf[curNode].children[chr] = child;

                for (idx_t dna : buf[curNode].curDNAs) {
                    if (DNAs[dna].curSym() == chr)
                        ans.push_back(dna);
                }
            }

            curNode = child;
        }

        DBG("Insert done");

        for (unsigned i = 0; i < ans.size();) {
            DBG("%u out of %zu", i, ans.size());

            if (tickDNA(ans[i])) {
                DBG("Matched");
                ++i;
            } else {
                DBG("Not matched");
                std::swap(ans[i], ans[ans.size() - 1]);
                ans.pop_back();
            }
        }

        DBG("Ticking done");

        return ans;
    }
};


inline void cycleStr(std::string &str, unsigned k) {
    k = k % str.length();

    if (k == 0)
        return;

    str = str.substr(k, str.length() - k) + str.substr(0, k);
}


int main() {
    Trie trie;

    unsigned n = 0;
    int res = scanf("%u", &n);
    assert(res == 1);

    unsigned k = 0;

    for (unsigned i = 0; i < n; ++i) {
        char cmd = 0;
        std::string str;

        // Could have used scanf for op, but why bother, since I'm forced to use cin anyway?
        std::cin >> cmd >> str;

        cycleStr(str, k);

        assert(cmd == '+' || cmd == '?');

        std::vector<Trie::idx_t> ans = (cmd == '+') ? trie.addGene(str) : trie.addDNA(std::move(str));

        k = ans.size();

        printf("%u ", k);
        for (unsigned idx : ans) {
            printf("%u ", idx + 1);
        }
        printf("\n");
    }

    return 0;
}

