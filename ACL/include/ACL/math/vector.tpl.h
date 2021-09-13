#if !defined(NAME_) || !defined(N_)
#error "DON'T use this file in your own projects!!! Use vector.h instead"
#endif

#if !defined(EXTRA_)
#define EXTRA_
#endif

template <typename T>
class NAME_ {
    static_assert(1 <= N_ && N_ <= 4);
    static_assert(std::is_signed<T>::value);

public:
    static constexpr unsigned DIM = N_;
    static constexpr NAME_<T> ZERO{};

    T values[DIM];

    constexpr NAME_() :
        values{} {}

    constexpr NAME_(const NAME_ &other) {
        for (unsigned i = 0; i < DIM; ++i) {
            values[i] = other.values[i];
        }
    }

    constexpr NAME_ &operator=(const NAME_ &other) {
        for (unsigned i = 0; i < DIM; ++i) {
            values[i] = other.values[i];
        }

        return *this;
    }

    constexpr NAME_ &operator+=(const NAME_ &other) {
        for (unsigned i = 0; i < DIM; ++i) {
            values[i] += other.values[i];
        }

        return *this;
    }

    constexpr NAME_ &operator-=(const NAME_ &other) {
        for (unsigned i = 0; i < DIM; ++i) {
            values[i] -= other.values[i];
        }

        return *this;
    }

    constexpr NAME_ &operator*=(double other) {
        for (unsigned i = 0; i < DIM; ++i) {
            values[i] *= other;
        }

        return *this;
    }

    constexpr NAME_ &operator/=(double other) {
        for (unsigned i = 0; i < DIM; ++i) {
            values[i] /= other;
        }

        return *this;
    }

    /// Scalar multiplication
    constexpr T operator*(const NAME_ &other) const {
        T result{};

        for (unsigned i = 0; i < DIM; ++i) {
            result += values[i] * other.values[i];
        }

        return result;
    }

    NAME_ &normalize() {
        double length_ = length();

        if (length_ == 0.d)
            throw new vector_error("Zero division");

        return *this *= 1 / length_;
    }

    NAME_ normalized() const {
        NAME_ result{*this};

        return result.normalize();
    }

    constexpr double length() const {
        if constexpr (DIM == 1)
            return (double)values[0];
        if constexpr (DIM == 2)
            return std::hypot((double)values[0], (double)values[1]);
        if constexpr (DIM == 3)
            return std::hypot((double)values[0], (double)values[1], (double)values[2]);
        return std::sqrt((double)magnitude());
    }

    constexpr T magnitude() const {
        T result{};

        for (unsigned i = 0; i < DIM; ++i)
            result += values[i] * values[i];

        return result;
    }

    constexpr bool isZero() const {
        for (unsigned i = 0; i < DIM; ++i)
            if (values[i])
                return false;

        return true;
    }

    constexpr friend NAME_ operator+(NAME_ self, const NAME_ &other) {
        return self += other;
    }

    constexpr friend NAME_ operator-(NAME_ self, const NAME_ &other) {
        return self -= other;
    }

    constexpr friend NAME_ operator*(NAME_ self, T other) {
        return self *= other;
    }

    constexpr friend NAME_ operator/(NAME_ self, T other) {
        return self /= other;
    }

    constexpr friend NAME_ operator-(NAME_ self) {
        return self *= -1.d;
    }

    template <typename OTHER_T>
    constexpr explicit operator NAME_<OTHER_T>() const {
        NAME_<OTHER_T> result{};

        for (unsigned i = 0; i < DIM; ++i)
            result.values[i] = (OTHER_T)values[i];

        return result;
    }

    EXTRA_

};

#define CONCAT_INNER_(A, B)  A##B
#define CONCAT_(A, B)  CONCAT_INNER_(A, B)

using CONCAT_(NAME_, i) = NAME_<int>;
using CONCAT_(NAME_, f) = NAME_<float>;
using CONCAT_(NAME_, d) = NAME_<double>;

#undef CONCAT_INNER_
#undef CONCAT_

#undef NAME_
#undef N_
#undef EXTRA_
