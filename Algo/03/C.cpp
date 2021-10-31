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
#include <stack>


#if 0
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

    size_t size() const noexcept { return suffixes.size(); }

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
                                        (int)getLink(tree),
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

            /*node = newNode;
            pos = 0;*/

            return newNode;
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

        for (unsigned idx = 0; idx < str.size(); ++idx) {
            step(idx, curPos);
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
    /*SuffixTree st("ABRACADABRA$");
    SuffixArray sa(st);

    st.dump();
    sa.dump();*/

    unsigned n = 0;
    int res = scanf("%u %*u", &n);
    assert(res == 1);

    std::string str{};

    for (unsigned i = 0; i < n; ++i) {
        char cur = 0;
        res = scanf("%hhu", &cur);
        assert(res == 1);
        str.push_back(cur + '0');
    }
    str.push_back('$');

    SuffixArray sa(str.data());

    // sa.dump();

    struct RefrInfo {
        unsigned long long cnt = 0;
        unsigned start = 0;
        unsigned size = 0;

        constexpr unsigned long long score() const noexcept {
            return cnt * size;
        }

        constexpr unsigned end() const noexcept {
            return start + size;
        }
    };

    RefrInfo best{1, 0, n};

    std::stack<RefrInfo> stk{};

    for (unsigned i = sa.size() - 1; i != -1u; --i) {
        SuffixArray::Suffix suff = sa.getSuffixes()[i];
        unsigned long long cnt = 1;

        while (!stk.empty() && suff.lcp <= stk.top().size) {
            DBG("Stk top: %llu %u %u", stk.top().cnt, stk.top().start, stk.top().size);

            cnt += stk.top().cnt;

            DBG("Was %llu against %llu", best.score(), cnt * stk.top().size);
            if (cnt * stk.top().size > best.score()) {
                best = stk.top();
                best.cnt = cnt;
                DBG("Is %llu", best.score());
            }

            stk.pop();
        }

        stk.push(RefrInfo{cnt, suff.start, suff.lcp});
    }

    printf("%llu\n%u\n", best.score(), best.size);

    for (unsigned i = best.start; i < best.end(); ++i) {
        printf("%hhu ", str[i] - '0');
    }
    printf("\n");

    return 0;
}

