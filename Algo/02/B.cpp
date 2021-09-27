#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <vector>
#include <algorithm>


void manacher(const std::vector<unsigned> &arr, std::vector<unsigned> &result) {
    result.resize(arr.size());

    unsigned left = 0,
             right = 0,
             curBest = 0;

    for (unsigned i = 0; i < arr.size(); ++i) {
        curBest = 0;

        if (i < right)
            curBest = std::min(right - i, result[right + left - i]);

        while (i + curBest - 1 < arr.size() && i >= curBest &&
               arr[i + curBest - 1] == arr[i - curBest])
               ++curBest;

        result[i] = curBest;

        if (i + curBest > right) {
            left  = i - curBest + 1;
            right = i + curBest - 1;
        }
    }
}

int main() {
    unsigned n = 0,
             m = 0;
    int res = scanf("%u %u", &n, &m);
    assert(res == 2);

    std::vector<unsigned> arr(n);
    for (unsigned i = 0; i < n; ++i) {
        res = scanf("%u", &arr[i]);
        assert(res == 1);
    }

    std::vector<unsigned> manacherArr{};
    manacher(arr, manacherArr);

    std::vector<unsigned> ans;
    for (unsigned i = 1; i < n; ++i) {
        if (manacherArr[i] == i + 1) {
            ans.push_back(i);
        }
    }

    // I wanted to try some perverted c++, sorry)
    for (auto i = ans.rbegin(); i != ans.rend(); ++i) {
        printf("%u ", n - *i);
    }
    printf("%u\n", n);

    return 0;
}
