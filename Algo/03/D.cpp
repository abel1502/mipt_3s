#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <vector>
#include <string_view>
#include <string>
#include <iostream>
#include <algorithm>
#include <map>
#include <new>
#include <type_traits>
#include <utility>


#if 0
#define DBG(FMT, ...) \
    printf("[%s#%d: " FMT "]\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define DBG(FMT, ...)
#endif


class SuffixAutomata {
public:
    using idx_t = unsigned;
    static constexpr idx_t BAD_IDX = -1u;

    struct Node;

    struct Node {
        char chr;
        // Hold the END position in both cases!
        unsigned long long firstOcc = 0, lastOcc = 0;
        unsigned long long longestLen = 0;
        idx_t link = BAD_IDX;
        bool terminal = false;
        unsigned long long score = -1ull;
        std::map<char, idx_t> children{};

        Node(char chr_) noexcept :
            chr{chr_} {}

        inline bool hasChild(char chr_) const noexcept {
            return children.find(chr_) != children.end();
        }

        inline idx_t getChild(char chr_) const noexcept {
            auto it = children.find(chr_);

            return it != children.end() ? it->second : BAD_IDX;
        }

        inline void addChild(char chr_, idx_t child) noexcept {
            children[chr_] = child;
        }

        inline bool isRoot() const noexcept {
            return chr == '\0';
        }

        inline bool isTerm() const noexcept {
            return terminal;
        }

        void dfsCalcScores(SuffixAutomata &automata) {
            if (score != -1ull)
                return;

            unsigned long long maxContainingLen = lastOcc + longestLen - firstOcc;

            score = maxContainingLen;

            if (lastOcc != firstOcc) {
                score += longestLen * longestLen;
            }
            // Otherwise we're only found once in the string, so we
            // aren't a proper suprefix and shouldn't be accounted for

            for (auto child : children) {
                automata[child.second].dfsCalcScores(automata);
            }
        }

        void dump(const SuffixAutomata &automata, unsigned indentation = 0) const {
            indent(indentation);
            printf("'%c' <%u> (link = %d, len = %llu)\n",
                   chr,
                   automata.refToIdx(*this),
                   (int)link,
                   longestLen);

            for (auto child : children) {
                indent(indentation);
                printf(" |-> '%c' %u\n", child.first, child.second);
            }

            printf("\n");
        }

    private:
        static void indent(unsigned indentation) {
            static constexpr const char INDENT[] = "    ";

            for (unsigned i = 0; i < indentation; ++i) {
                printf("%s", INDENT);
            }
        }
    };

    friend struct Node;


    SuffixAutomata(const char *str_ = "") :
        str{str_}, nodes{Node('\0')} {

        build();
    }

    SuffixAutomata &operator=(const char *str_) {
        str = str_;
        nodes.clear();
        nodes.push_back(Node('\0'));

        build();

        return *this;
    }

    void dump() const {
        printf("SuffAutomata(\"%s\") {\n", str.data());
        for (const Node &node : nodes) {
            node.dump(*this, 1);
        }
        printf("}\n");
    }

    const std::string_view &getStr() const noexcept { return str; }

    const Node &operator[](idx_t idx) const {
        assert(idx < nodes.size());
        return nodes[idx];
    }

    unsigned long long findBestScore() {
        assert(nodes.size() > 0);
        nodes[0].dfsCalcScores(*this);

        unsigned long long bestScore = 0;
        for (const Node &node : nodes) {
            if (bestScore < node.score) {
                bestScore = node.score;
            }
        }

        return bestScore;
    }

protected:
    std::string_view str;
    std::vector<Node> nodes;
    idx_t last = 0;


    Node &operator[](idx_t idx) {
        assert(idx < nodes.size());
        return nodes[idx];
    }

    inline idx_t refToIdx(const Node &node) const {
        assert((size_t)(&node - nodes.data()) < nodes.size());

        return &node - nodes.data();
    }

    inline idx_t lastIdx() const noexcept {
        assert(nodes.size());
        return nodes.size() - 1;
    }

    idx_t addNode(idx_t prev, char chr) {
        // assert(!nodes[prev].hasChild(chr));

        nodes.emplace_back(chr);
        Node & newNode = (*this)[lastIdx()];
        Node &prevNode = (*this)[prev     ];

        // Shouldn't be done here, in fact
        // prevNode.addChild(chr, lastIdx());

        newNode.longestLen = prevNode.longestLen + 1;
        newNode.firstOcc   = prevNode.firstOcc + 1;
        newNode. lastOcc   = prevNode. lastOcc + 1;

        return lastIdx();
    }

    idx_t cloneNode(idx_t original) {
        nodes.emplace_back((*this)[original]);

        return lastIdx();
    }

    void build() {
        for (unsigned idx = 0; idx < str.size(); ++idx) {
            step(str[idx]);
        }

        markTerminals();
    }

    void step(char chr) {
        idx_t curIdx = addNode(last, chr);

        idx_t prevIdx = last;
        for (; prevIdx != BAD_IDX && !(*this)[prevIdx].hasChild(chr);
               prevIdx = (*this)[prevIdx].link) {

            (*this)[prevIdx].addChild(chr, curIdx);
        }

        if (prevIdx == BAD_IDX) {
            (*this)[curIdx].link = 0;
            last = curIdx;
            return;
        }

        idx_t candidateIdx = (*this)[prevIdx].getChild(chr);
        assert(candidateIdx != BAD_IDX);

        if ((*this)[candidateIdx].longestLen == (*this)[prevIdx].longestLen + 1) {
            (*this)[curIdx].link = candidateIdx;
            (*this)[candidateIdx].lastOcc = (*this)[curIdx].lastOcc;
            last = curIdx;
            return;
        }

        idx_t cloneIdx = cloneNode(candidateIdx);

        (*this)[cloneIdx].lastOcc = (*this)[curIdx].lastOcc;
        (*this)[cloneIdx].longestLen = (*this)[prevIdx].longestLen + 1;

        for (; prevIdx != BAD_IDX && (*this)[prevIdx].getChild(chr) == candidateIdx;
               prevIdx = (*this)[prevIdx].link) {

            (*this)[prevIdx].addChild(chr, cloneIdx);
        }

        (*this)[curIdx]      .link = cloneIdx;
        (*this)[candidateIdx].link = cloneIdx;
        last = curIdx;
        return;
    }

    void markTerminals() {
        idx_t cur = last;

        for (; cur != BAD_IDX;
               cur = (*this)[cur].link) {

            (*this)[cur].terminal = true;
        }
    }

};


int main() {
    std::string str{};
    std::cin >> str;
    // str = "ababaab";

    SuffixAutomata sa(str.data());

    // sa.dump();

    std::cout << sa.findBestScore() << "\n";

    return 0;
}

