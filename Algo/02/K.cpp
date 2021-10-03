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
    constexpr unsigned STR_SIZE_LIMIT = 8001;
    constexpr char STR_FMT[] = "%8000s";

    std::vector<char> text(STR_SIZE_LIMIT, '\0');

    int res = scanf(STR_FMT, text.data());
    assert(res == 1);

    unsigned length = strlen(text.data());

    if (!length) {
        printf("0\n");
        return 0;
    }

    std::vector<unsigned> zArr{};

    unsigned long long ans = 1;
    for (unsigned sufLen = 2; sufLen <= length; ++sufLen) {
        zFunc(text.data() + length - sufLen, zArr);

        unsigned long long max = *std::max_element(zArr.begin(), zArr.end());
        unsigned long long tmp = sufLen - max;

        ans += max * tmp;
        ans += tmp * (tmp + 1) / 2;
    }

    printf("%llu\n", ans);

    return 0;
}
