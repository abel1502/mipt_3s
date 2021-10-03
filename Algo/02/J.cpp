#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>


void zFunc(std::string str, std::vector<unsigned> &result) {
    unsigned length = str.length();
    result.resize(length);

    unsigned l = 0, r = 0;

    for (unsigned i = 1; i < length; ++i) {
        result[i] = (unsigned)std::max<int>(0, std::min<int>(result[i - l], r - i));

        while (result[i] + i < length &&
               str[result[i]] == str[result[i] + i])
            result[i]++;

        if (result[i] + i > r) {
            l = i;
            r = result[i] + i;
        }
    }
}


int main() {
    // As advised by Max, strings are significantly more convenient here
    std::string str{}, pattern{};

    std::cin >> str >> pattern;

    unsigned k = 0;
    int res = scanf("%u", &k);
    assert(res == 1);

    std::vector<unsigned> zFwd{},
                          zRev{};

    zFunc(pattern + "#" + str, zFwd);
    std::reverse(str.begin(), str.end());
    std::reverse(pattern.begin(), pattern.end());
    zFunc(pattern + "#" + str, zRev);

    std::vector<unsigned> matches{};

    for (unsigned i = 0; i + pattern.length() < str.length() + 1; ++i) {
        unsigned common = zFwd[pattern.length() + 1 + i] +
                          zRev[str.length() + 1 - i];

        if (common + k >= pattern.length()) {
            matches.push_back(i);
        }
    }

    printf("%zu\n", matches.size());
    for (unsigned i : matches) {
        printf("%u ", i + 1);
    }
    printf("\n");

    return 0;
}

