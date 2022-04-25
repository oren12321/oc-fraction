#ifndef MATH_TYPES_FRACTION_H
#define MATH_TYPES_FRACTION_H

#include <type_traits>
#include <limits>
#include <cmath>
#include <stdexcept>

#include <math/core/utils.h>

namespace math::core::types {
    template <typename T>
    concept Integer = std::is_integral_v<T> && !std::is_same_v<T, bool> && std::is_signed_v<T>;

    template <typename T>
    concept Decimal = std::is_floating_point_v<T>;

    template <Integer I>
    class Fraction {
    public:
        Fraction(I n = I{ 0 }, I d = I{ 1 })
            : n_(n), d_(d)
        {
            CORE_EXPECT(d_ != I{ 0 }, std::invalid_argument, "denominator is zero");

            if (n_ == I{ 0 }) {
                return;
            }

            I sign = (n_ * d_) / std::abs(n_ * d_);
            n_ = std::abs(n_) * sign;
            d_ = std::abs(d_);

            I g = gcd(*this);
            n_ /= g;
            d_ /= g;
        }

        template <Decimal F>
        Fraction(F d)
        {
            *this = decimal_to_fraction(d);
        }

        I n()
        {
            return n_;
        }

        I d()
        {
            return d_;
        }

        Fraction<I> operator-()
        {
            return { -n_, d_ };
        }

        Fraction<I> reciprocal()
        {
            CORE_EXPECT(n_ != I{ 0 }, std::overflow_error, "division by zero");

            I sign = (n_ * d_) / std::abs(n_ * d_);
            return { n_ * sign, std::abs(d_) };
        }

    private:
        static I gcd(const Fraction<I>& f)
        {
            I t;
            I a = f.n_;
            I b = f.d_;
            if (a > b) {
                t = b;
                b = a;
                a = t;
            }
            while (b != 0) {
                t = a % b;
                a = b;
                b = t;
            }
            return std::abs(a);
        }

        // Initialize:
        //     Z1 = X
        //     D0 = 0, D1 = 1
        // Repeat until convergance:
        //     Zi+1 = 1 / (Zi - INT(Zi))
        //     Di+1 = Di x INT(Z+1) + Di-1
        //     Ni+1 = ROUND(X x Di+1)
        // Result:
        //     Ni+1/Di+1
        template <Decimal F>
        static Fraction<I> decimal_to_fraction(F decimal, F accuracy = F{ 1e-19 })
        {
            F sign = decimal >= F{0} ? F{1} : F{-1};

            F decimal_abs = std::abs(decimal);

            F decimal_int_part{std::floor(decimal_abs)};
            F decimal_fractional_part{decimal_abs - decimal_int_part};

            if (decimal_abs == decimal_int_part) {
                return { static_cast<I>(sign * decimal_abs), I{1} };
            }

            F z_i{decimal_abs};
            F d_i{1};
            F d_i_minus_1{0};

            F n_i{0}; // Used for overflow check

            F z_i_plus_1{F{1} / (z_i - std::floor(z_i))};
            F d_i_plus_1{d_i * std::floor(z_i_plus_1) + d_i_minus_1};
            F n_i_plus_1{std::round(decimal_abs * d_i_plus_1)};

            F z_i_int_part{std::floor(z_i)};

            while (
                z_i_int_part != z_i &&
                std::abs(decimal_abs - n_i_plus_1 / d_i_plus_1) > accuracy) {
                z_i = z_i_plus_1;
                d_i_minus_1 = d_i;
                d_i = d_i_plus_1;

                n_i = n_i_plus_1;

                z_i_plus_1 = F{1} / (z_i - std::floor(z_i));
                d_i_plus_1 = d_i * std::floor(z_i_plus_1) + d_i_minus_1;
                n_i_plus_1 = std::round(decimal_abs * d_i_plus_1);

                z_i_int_part = std::floor(z_i);

                F max_int{static_cast<F>(std::numeric_limits<I>::max())};
                if (n_i_plus_1 > max_int || d_i_plus_1 > max_int) {
                    return { static_cast<I>(sign * n_i), static_cast<I>(d_i) };
                }
            }
            return { static_cast<I>(sign * n_i_plus_1), static_cast<I>(d_i_plus_1) };
        }

        I n_{ 0 };
        I d_{ 0 };
    };
}

#endif
