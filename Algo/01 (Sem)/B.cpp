#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <vector>


struct Node {
    Node *children[2];
    unsigned value;

    static_assert(sizeof(children) == sizeof(void *) * 2);

    Node() :
        Node(nullptr, nullptr) {}

    Node(Node *left, Node *right) :
        children{left, right},
        value{-1u} {}

    Node(unsigned value) :
        children{nullptr, nullptr},
        value{value} {}

    ~Node() {
        delete children[0];
        delete children[1];
    }

    void insert(unsigned num, unsigned depth) {
        Node *cur = this;

        while (depth) {
            depth--;
            unsigned bit = (num >> depth) & 1;

            if (!cur->children[bit])
                cur->children[bit] = new Node();

            cur = cur->children[bit];
            assert(cur);
        }

        cur->value = num;
    }

    bool remove(unsigned num, unsigned depth) {
        if (!depth) {
            assert(num == value);
            return true;
        }

        depth--;
        unsigned bit = (num >> depth) & 1;

        if (!children[bit])
            return false;

        if (children[bit]->remove(num, depth)) {
            delete children[bit];
            children[bit] = nullptr;

            return !children[1 - bit];
        }

        return false;
    }

    unsigned findBest(unsigned num, unsigned depth) {
        Node *cur = this;

        while (depth) {
            depth--;
            unsigned bit = (num >> depth) & 1;

            Node *next = cur->children[1 - bit];

            if (!next)
                next = cur->children[bit];

            cur = next;
            assert(cur);
        }

        return cur->value;
    }
};


struct Tree {
    Node *root;
    unsigned depth;

    Tree() :
        Tree(0) {}

    Tree(unsigned depth) :
        root{nullptr},
        depth{depth} {

        assert(depth);
        root = new Node();
    }

    ~Tree() {
        delete root;
    }

    void insert(unsigned num) {
        assert(root);
        root->insert(num, depth);
    }

    void remove(unsigned num) {
        assert(root);
        root->remove(num, depth);
    }

    unsigned findBest(unsigned num) {
        assert(root);
        return root->findBest(num, depth);
    }
};


int main() {
    Tree tree{32};

    unsigned n = 0;
    int res = scanf("%u", &n);
    assert(res == 1);

    for (unsigned i = 0; i < n; ++i) {
        unsigned cmd = 0, arg = 0;

        res = scanf("%u %u", &cmd, &arg);
        assert(res == 2);

        switch (cmd) {
        case 1:
            tree.insert(arg);
            break;

        case 2:
            tree.remove(arg);
            break;

        case 3:
            printf("%u\n", tree.findBest(arg));
            break;

        default:
            assert(false);
            return 1;
        }
    }

    return 0;
}
