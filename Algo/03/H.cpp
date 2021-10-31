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


class SuffixTree {
public:
    using idx_t = unsigned;
    using mask_t = unsigned;
    static constexpr idx_t BAD_IDX = -1u;

    struct Pos;
    struct Node;

    struct Node {
        unsigned strL = 0, strR = 0;
        idx_t parent = BAD_IDX;
        idx_t link = BAD_IDX;
        mask_t mask = 0;
        std::map<char, idx_t> children{};

        Node(unsigned strL_, unsigned strR_, idx_t parent_ = BAD_IDX, mask_t mask_ = 0) noexcept :
            strL{strL_}, strR{strR_}, parent{parent_}, mask{mask_} {}

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

            // TODO: Verify
            targetPos.advance(tree, strL + (parent == 0), strR);

            idx_t ourIdx = tree.idxByRef(*this);
            idx_t newLink = targetPos.split(tree);  // This line might invalidate `this`
            return (tree.nodes[ourIdx].link = newLink);
        }

        void dump(SuffixTree &tree, unsigned indentation = 0) {
            indent(indentation); printf("\"%.*s\" <%u> (link = %d, parent = %d, mask = %x) %s",
                                        getLength(), tree.str.data() + strL,
                                        tree.idxByRef(*this),
                                        (int)getLink(tree),
                                        (int)parent,
                                        mask,
                                        isLeaf() ? "{}" : "{\n");

            for (auto child : children) {
                tree.nodes[child.second].dump(tree, indentation + 1);
            }

            indent(indentation); printf("%s\n", isLeaf() ? "" : "}");
        }

        mask_t dfsSetMasks(SuffixTree &tree) {
            if (isLeaf()) {
                if (getLength() == 0) {
                    assert(isRoot());
                    return 0;
                }

                assert(getLength() > 0);
                assert((unsigned)(tree.str[strR - 1] - '0') < 10);
                return (mask = 1 << (tree.str[strR - 1] - '0'));
            }

            mask = 0;

            for (auto child : children) {
                mask |= tree.nodes[child.second].dfsSetMasks(tree);
            }

            return mask;
        }

        unsigned dfsGetMaxCommon(const SuffixTree &tree, mask_t allMask, unsigned &end) const {
            if ((mask & allMask) != allMask) {
                end = strR;
                return 0;
            }

            unsigned bestLen = 0;
            end = strR;

            for (auto child : children) {
                unsigned curEnd = 0, curLen = 0;
                curLen = tree.nodes[child.second].dfsGetMaxCommon(tree, allMask, curEnd);

                if (curLen > bestLen) {
                    bestLen = curLen;
                    end = curEnd;
                }
            }

            return bestLen + getLength();
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

        unsigned advance(const SuffixTree &tree, unsigned strL, unsigned strR) {
            assert(strL <= strR);

            Pos backup = *this;

            unsigned length = strR - strL;
            while (strL < strR) {
                char curChr = tree.str[strL];

                if (pos == getCurLength(tree)) {
                    node = getCurNode(tree).getChild(curChr);
                    pos = 0;

                    if (node == BAD_IDX) {
                        *this = backup;
                        return 0;
                    }

                    continue;
                }

                if (tree.str[getCurNode(tree).strL + pos] != curChr) {
                    *this = backup;
                    return 0;
                }

                if (strR - strL < getRemainingLength(tree)) {
                    pos += strR - strL;
                    return length;
                }

                strL += getRemainingLength(tree);
                pos = getCurLength(tree);
            }

            return length;
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


    SuffixTree() :
        str{""}, nodes{Node(0, 0)} {}

    SuffixTree &operator+=(const char *str_) {
        insert(str_);

        return *this;
    }

    void insert(const char *str_) {
        unsigned start = str.size();
        str += str_;
        str += '0' + numStrs;

        build(start);
        numStrs++;
    }

    mask_t setMasks() {
        assert(nodes.size() > 0);
        return nodes[0].dfsSetMasks(*this);
    }

    std::string_view getMaxCommon() const {
        assert(nodes.size() > 0);
        unsigned strR = 0, length = 0;
        length = nodes[0].dfsGetMaxCommon(*this, maskAll(), strR);
        return std::string_view(str.data() + strR - length, length);
    }

    void dump() {
        printf("SuffTree(\"%s\") {\n", str.data());
        nodes[0].dump(*this, 1);
        printf("}\n");
    }

    const std::string &getStr() const noexcept { return str; }

    const Node &operator[](idx_t idx) const {
        assert(idx < nodes.size());
        return nodes[idx];
    }

protected:
    unsigned numStrs = 0;
    std::string str;
    std::vector<Node> nodes;


    inline idx_t idxByRef(const Node &node) const {
        assert((size_t)(&node - nodes.data()) < nodes.size());

        return &node - nodes.data();
    }

    inline idx_t lastIdx() const noexcept {
        assert(nodes.size());
        return nodes.size() - 1;
    }

    inline mask_t maskAll() const noexcept {
        return (1 << numStrs) - 1;
    }

    idx_t addNode(idx_t parent, unsigned strL, unsigned strR) {
        char chr = str[strL];

        // assert(!nodes[parent].hasChild(chr));

        nodes.emplace_back(strL, strR, parent);
        nodes[parent].addChild(chr, lastIdx());

        return lastIdx();
    }

    void build(unsigned start) {
        Pos curPos(0, 0);

        for (unsigned idx = start; idx < str.size(); ++idx) {
            step(idx, curPos);
        }
    }

    void step(unsigned idx, Pos &pos) {
        while (true) {
            if (pos.advance(*this, idx, idx + 1))
                return;

            idx_t newNode = pos.split(*this);
            addNode(newNode, idx, str.size());

            pos = nodes[nodes[newNode].getLink(*this)].getPos(*this);

            if (!newNode)
                return;
        }
    }

};



int main() {
    unsigned n = 0;
    std::cin >> n;  // That's just easier

    std::string str{};
    SuffixTree st{};

    for (unsigned i = 0; i < n; ++i) {
        std::cin >> str;
        st.insert(str.data());
    }

    // st.insert("aboba");
    // st.insert("biba");
    st.setMasks();
    // st.dump();
    std::cout << st.getMaxCommon() << "\n";

    return 0;
}

