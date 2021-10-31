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


#if 1
#define DBG(FMT, ...) \
    printf("[%s#%d: " FMT "]\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define DBG(FMT, ...)
#endif


class SuffixTree;

class SuffixArray {
public:
    struct Suffix {
        unsigned start;
        unsigned lcp;

        void dump(const SuffixArray &array) const noexcept {
            printf("    \"%-*s\" [%2u] (lcp = %2u)\n", (int)array.str.size(), array.str.data() + start, start, lcp);
        }
    };

    friend class Suffix;


    SuffixArray(const char *str_ = "");

    SuffixArray(const SuffixTree &suffTree);

    template <typename ... As>
    SuffixArray &operator=(As &&... args) {
        this->~SuffixArray();
        SuffixArray *newAddr = new (this) SuffixArray(std::forward<As>(args)...);
        assert(newAddr == this);

        return *this;
    }

    // Warning: Do not attempt to use directly!
    void addSuffix(unsigned start, unsigned lcp) {
        assert(suffixes.size() < str.size());
        suffixes.push_back(Suffix{start, lcp});
    }

    void dump() const noexcept {
        printf("SuffArr(\"%s\") {\n", str.data());

        for (unsigned i = 0; i < suffixes.size(); ++i) {
            suffixes[i].dump(*this);
        }

        printf("}\n");
    }

    const std::string_view &getStr() const noexcept { return str; }

    const std::vector<Suffix> &getSuffixes() const noexcept { return suffixes; }

protected:
    std::string_view str;
    std::vector<Suffix> suffixes;

};


class SuffixTree {
public:
    using idx_t = unsigned;
    static constexpr idx_t BAD_IDX = -1u;

    struct Pos;
    struct Node;

    struct Node {
        unsigned strL = 0, strR = 0;
        idx_t parent = BAD_IDX;
        idx_t link = BAD_IDX;
        mutable unsigned totalLength = 0;
        std::map<char, idx_t> children{};

        Node(unsigned strL_, unsigned strR_, idx_t parent_ = BAD_IDX) noexcept :
            strL{strL_}, strR{strR_}, parent{parent_} {}

        inline bool hasChild(char chr) const noexcept {
            return children.find(chr) != children.end();
        }

        inline idx_t getChild(char chr) const noexcept {
            auto it = children.find(chr);

            return it != children.end() ? it->second : BAD_IDX;
        }

        inline void addChild(char chr, idx_t child) noexcept {
            children[chr] = child;
        }

        inline unsigned getLength() const noexcept {
            return strR - strL;
        }

        inline bool isRoot() const noexcept {
            return parent == BAD_IDX;
        }

        inline bool isLeaf() const noexcept {
            return children.size() == 0;
        }

        Pos getPos(const SuffixTree &tree) {
            return Pos(tree.idxByRef(*this), getLength());
        }

        idx_t getLink(SuffixTree &tree) {
            if (link != BAD_IDX)
                return link;

            if (isRoot())
                return (link = 0);

            idx_t target = tree.nodes[parent].getLink(tree);
            Pos targetPos = tree.nodes[target].getPos(tree);

            targetPos.advance(tree, strL + (parent == 0), strR);

            idx_t ourIdx = tree.idxByRef(*this);
            idx_t newLink = targetPos.split(tree);  // This line might invalidate `this`
            return (tree.nodes[ourIdx].link = newLink);
        }

        void dump(SuffixTree &tree, unsigned indentation = 0) {
            indent(indentation); printf("\"%.*s\" <%u> (link = %d, parent = %d) %s",
                                        getLength(), tree.str.data() + strL,
                                        tree.idxByRef(*this),
                                        (int)/*getLink(tree)*/link,
                                        (int)parent,
                                        isLeaf() ? "{}" : "{\n");

            for (auto child : children) {
                tree.nodes[child.second].dump(tree, indentation + 1);
            }

            indent(indentation); printf("%s\n", isLeaf() ? "" : "}");
        }

        void dfsPopulateSuffArray(const SuffixTree &tree, SuffixArray &suffArr, unsigned &lcp, unsigned suffLen = 0) const noexcept {
            if (isLeaf()) {
                suffArr.addSuffix(strL - suffLen, lcp);
                lcp = suffLen;
                return;
            }

            suffLen += getLength();

            for (auto child : children) {
                tree.nodes[child.second].dfsPopulateSuffArray(tree, suffArr, lcp, suffLen);
                if (lcp > suffLen)
                    lcp = suffLen;
            }
        }

        unsigned getTotalLength(const SuffixTree &tree) const noexcept {
            if (!totalLength) {
                totalLength = getLength() + (isRoot() ? 0 : tree.nodes[parent].getTotalLength(tree));
            }

            return totalLength;
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

    struct Pos {
        idx_t node = BAD_IDX;
        unsigned pos = 0;

        Pos(idx_t node_, unsigned pos_) noexcept :
            node{node_}, pos{pos_} {}

        inline       Node &getCurNode(      SuffixTree &tree) const { assert(node != BAD_IDX); return tree.nodes[node]; }
        inline const Node &getCurNode(const SuffixTree &tree) const { assert(node != BAD_IDX); return tree.nodes[node]; }

        inline unsigned getCurLength(const SuffixTree &tree) const {
            return getCurNode(tree).getLength();
        }

        inline unsigned getRemainingLength(const SuffixTree &tree) const {
            return getCurLength(tree) - pos;
        }

        bool advance(const SuffixTree &tree, unsigned strL, unsigned strR) {
            assert(strL <= strR);

            Pos backup = *this;

            while (strL < strR) {
                char curChr = tree.str[strL];

                if (pos == getCurLength(tree)) {
                    node = getCurNode(tree).getChild(curChr);
                    pos = 0;

                    if (node == BAD_IDX) {
                        *this = backup;
                        return false;
                    }

                    continue;
                }

                if (tree.str[getCurNode(tree).strL + pos] != curChr) {
                    *this = backup;
                    return false;
                }

                if (strR - strL < getRemainingLength(tree)) {
                    pos += strR - strL;
                    return true;
                }

                strL += getRemainingLength(tree);
                pos = getCurLength(tree);
            }

            return true;
        }

        idx_t split(SuffixTree &tree) {
            if (pos == getCurLength(tree)) {
                return node;
            }

            if (pos == 0) {
                return getCurNode(tree).parent;
            }

            Node backup = getCurNode(tree);
            idx_t newNode = tree.addNode(backup.parent, backup.strL, backup.strL + pos);
            tree.nodes[newNode].addChild(tree.str[backup.strL + pos], node);
            getCurNode(tree).parent = newNode;
            getCurNode(tree).strL += pos;

            return newNode;
        }

        unsigned getTotalLength(const SuffixTree &tree) const noexcept {
            return getCurNode(tree).getTotalLength(tree) - getRemainingLength(tree);
        }
    };

    friend struct Pos;


    SuffixTree(const char *str_ = "") :
        str{str_}, nodes{Node(0, 0)} {

        build();
    }

    SuffixTree &operator=(const char *str_) {
        str = str_;
        nodes.clear();
        nodes.push_back(Node(0, 0));

        build();

        return *this;
    }

    void dump() {
        printf("SuffTree(\"%s\") {\n", str.data());
        nodes[0].dump(*this, 1);
        printf("}\n");
    }

    void populateSuffArray(SuffixArray &suffArr) const noexcept {
        unsigned lcp = 0;
        assert(nodes.size() > 0);
        nodes[0].dfsPopulateSuffArray(*this, suffArr, lcp);
    }

    const std::string_view &getStr() const noexcept { return str; }

    const Node &operator[](idx_t idx) const {
        assert(idx < nodes.size());
        return nodes[idx];
    }

protected:
    std::string_view str;
    std::vector<Node> nodes;


    inline idx_t idxByRef(const Node &node) const {
        assert((size_t)(&node - nodes.data()) < nodes.size());

        return &node - nodes.data();
    }

    inline idx_t lastIdx() const noexcept {
        assert(nodes.size());
        return nodes.size() - 1;
    }

    idx_t addNode(idx_t parent, unsigned strL, unsigned strR) {
        char chr = str[strL];

        // assert(!nodes[parent].hasChild(chr));

        nodes.emplace_back(strL, strR, parent);
        nodes[parent].addChild(chr, lastIdx());

        return lastIdx();
    }

    void build() {
        Pos curPos(0, 0);

        unsigned long long differentSubstrs = 0;

        for (unsigned idx = 0; idx < str.size(); ++idx) {
            step(idx, curPos);

            differentSubstrs += idx + 1 - curPos.getTotalLength(*this);
            printf("%llu\n", differentSubstrs);
        }
    }

    void step(unsigned idx, Pos &pos) {
        while (true) {
            if (pos.advance(*this, idx, idx + 1)) {
                return;
            }

            idx_t newNode = pos.split(*this);
            addNode(newNode, idx, str.size());

            pos = nodes[nodes[newNode].getLink(*this)].getPos(*this);

            if (!newNode) {
                return;
            }
        }
    }

};


SuffixArray::SuffixArray(const char *str_) :
    SuffixArray(SuffixTree(str_)) {}

SuffixArray::SuffixArray(const SuffixTree &suffTree) :
    str{suffTree.getStr()}, suffixes{} {

    suffTree.populateSuffArray(*this);
}



int main() {
    std::string str{};
    std::cin >> str;
    // str += "$";

    SuffixTree st(str.data());
    // st.dump();

    return 0;
}

