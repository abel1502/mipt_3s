#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <vector>


void zFunc(const char *str, std::vector<unsigned> &result) {
    unsigned length = strlen(str);
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
    constexpr unsigned STR_SIZE_LIMIT = 1000001;
    constexpr char STR_FMT[] = "%1000000s";

    std::vector<char> s(STR_SIZE_LIMIT, '\0');

    int res = scanf(STR_FMT, s.data());
    assert(res == 1);

    std::vector<unsigned> zArr{};
    zFunc(s.data(), zArr);

    for (unsigned i : zArr)
        printf("%u ", i);
    printf("\n");

    return 0;
}

