#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <vector>


using value_t = int;


struct Point {
    value_t coord = 0;
    unsigned idx = -1u;

    static Point read(unsigned idx) {
        Point result{};

        scanf("%u", &result.coord);
        result.idx = idx;

        return result;
    }

    constexpr bool operator<(const Point &other) const noexcept {
        return  coord <  other.coord ||
               (coord == other.coord && idx < other.idx);
    }
};


int main() {
    unsigned n = 0;
    scanf("%u", &n);
    std::vector<Point> points(n);

    for (unsigned i = 0; i < n; ++i)
        points[i] = Point::read(i);

    std::sort(points.begin(), points.end());

    value_t bestDist = -1u >> 1;
    unsigned bestIdx = 0;
    for (unsigned i = 1; i < n; ++i) {
        value_t dist = points[i].coord - points[i - 1].coord;
        if (dist < bestDist) {
            bestDist = dist;
            bestIdx = i;
        }
    }

    printf("%d\n", bestDist);
    unsigned idxA = points[bestIdx    ].idx,
             idxB = points[bestIdx - 1].idx;

    printf("%u %u\n", std::min(idxA, idxB) + 1, std::max(idxA, idxB) + 1);
}

