// https://www.babichev.org/cgi-bin/new-client?action=139&prob_id=20

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <set>
#include <vector>


std::vector<unsigned> zFunc(const char *str, unsigned length) {
    std::vector<unsigned> z(length);
    for (unsigned i = 1, l = 0, r = 0; i < length; ++i) {
        if (i <= r)  z[i] = std::min(r - i + 1, z[i - l]);
        for (; i + z[i] < length && str[z[i]] == str[i + z[i]]; ++z[i]);
        if (i + z[i] - 1 > r)  l = i, r = i + z[i] - 1;
    }
    return z;
}

int main() {
    char str[5001] = "";

    int res = scanf("%5000s", str);
    assert(res == 1);

    unsigned length = strlen(str);

    std::vector<unsigned> zOrig = zFunc(str, length);

    for (unsigned i = 0; i < length; ++i) {
        unsigned orig = zOrig[i];

        std::vector<unsigned> z = zFunc(str + i, length - i);

        printf("% 2u: ", i);
        for (unsigned item : z) {
            printf("% 2u ", item);
        }
        printf("\n");
    }

    return 0;
}

/*typedef long long myhash_t;

constexpr myhash_t HASH_BASE = 29;
constexpr myhash_t HASH_MOD = 1000000007;


// BasePowers should have >= length entries allocated
void buildBasePowers(myhash_t *basePowers, unsigned length) {
    basePowers[0] = 1;

    for (unsigned i = 1; i < length; ++i) {
        basePowers[i] = basePowers[i - 1] * HASH_BASE % HASH_MOD;
    }
}


// Length should be <= strlen(str), and prefTable should have >= length + 1 entries allocated
// BasePowers should have been built
void buildPrefHashes(myhash_t *prefTable, const myhash_t *basePowers, const char *str, unsigned length) {
    prefTable[0] = 0;

    for (unsigned i = 0; i < length; ++i) {
        prefTable[i + 1] = (prefTable[i] + (str[i] - 'a' + 1) * basePowers[i]) % HASH_MOD;
    }
}


inline myhash_t getSubHash(const myhash_t *prefTable, const myhash_t *basePowers,
                           unsigned length, unsigned from, unsigned to) {
    return (prefTable[to] - prefTable[from]) * basePowers[length - from] % HASH_MOD;
}


int main() {
    char str[5001] = "";

    int res = scanf("%5000s", str);
    assert(res == 1);

    unsigned length = strlen(str);
    myhash_t *basePowers = (myhash_t *)calloc(length, sizeof(myhash_t));
    myhash_t *prefTable = (myhash_t *)calloc(length + 1, sizeof(myhash_t));

    buildBasePowers(basePowers, length);
    buildPrefHashes(prefTable, basePowers, str, length);

    std::set<myhash_t> seenHashes{0};  // 0 is the hash of an empty string

    for (unsigned from = 0; from < length; ++from) {
        for (unsigned to = from + 1; to < length; ++to) {
            seenHashes.insert(getSubHash(prefTable, basePowers, length, from, to));
        }
    }

    printf("%zu", seenHashes.size());

    return 0;
}*/
