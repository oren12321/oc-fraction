#include <type_traits>
#include <limits>
#include <cmath>
#include <utility>

template <typename T>
concept Integer = std::is_integral_v<T> && !std::is_same_v<T, bool>;

template <typename T>
concept Decimal = std::is_floating_point_v<T>;

/*
Algorithm:
Initialize:
Z1 = X
D0 = 0, D1 = 1
Repeat until convergance:
Zi+1 = 1 / (Zi - INT(Zi))
Di+1 = Di x INT(Z+1) + Di-1
Ni+1 = ROUND(X x Di+1)
*/
template <Integer I, Decimal F>
std::pair<I, I> decimal_to_fraction(F decimal)
{
    if (static_cast<I>(decimal) == decimal) {
        return std::make_pair(static_cast<I>(decimal), I{1});
    }

    I sign = decimal > 0 ? I{1} : I{-1};
    F unsigned_decimal = std::abs(decimal);
    F accuracy = std::numeric_limits<F>::epsilon();

    F z_i{unsigned_decimal};
    I d_i{1};
    I d_i_minus_1{0};

    F z_i_plus_1{static_cast<F>(1) / (z_i - static_cast<I>(z_i))};
    I d_i_plus_1{d_i * static_cast<I>(z_i_plus_1) + d_i_minus_1};
    I n_i_plus_1{static_cast<I>(std::round(unsigned_decimal * d_i_plus_1))};

    while (
        static_cast<I>(z_i) != z_i &&
        std::abs(unsigned_decimal - static_cast<F>(n_i_plus_1) / d_i_plus_1) > accuracy) {
        z_i = z_i_plus_1;
        d_i_minus_1 = d_i;
        d_i = d_i_plus_1;

        z_i_plus_1 = static_cast<F>(1) / (z_i - static_cast<I>(z_i));
        d_i_plus_1 = d_i * static_cast<I>(z_i_plus_1) + d_i_minus_1;
        n_i_plus_1 = static_cast<I>(std::round(unsigned_decimal * d_i_plus_1));
    }
    return std::make_pair(sign * n_i_plus_1, d_i_plus_1);
}

