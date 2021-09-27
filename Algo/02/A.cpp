/*
    I guess I'll stop copying the task texts, by the way.
    If I still should, tell me.
*/


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
    constexpr unsigned STR_SIZE_LIMIT = 50001;
    constexpr char STR_FMT[] = "%50000s";

    std::vector<char> s(STR_SIZE_LIMIT, '\0'),
                      t(STR_SIZE_LIMIT + 1, '\0');

    int res = 0;

    res = scanf(STR_FMT, s.data());
    assert(res == 1);

    res = scanf(STR_FMT, t.data());
    assert(res == 1);

    unsigned tLen = strlen(t.data());
    assert(tLen < STR_SIZE_LIMIT + 1);
    t[tLen] = '#';
    t.insert(t.begin() + tLen + 1, s.begin(), s.end());

    std::vector<unsigned> zArr{};
    zFunc(t.data(), zArr);

    for (unsigned i = 0; i < zArr.size(); ++i)
        if (zArr[i] == tLen)
            printf("%u ", i - tLen - 1);
    printf("\n");

    return 0;
}
