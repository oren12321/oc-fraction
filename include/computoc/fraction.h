#ifndef COMPUTOC_TYPES_FRACTION_H
#define COMPUTOC_TYPES_FRACTION_H

#include <type_traits>
#include <limits>
#include <cmath>
#include <stdexcept>

#include <computoc/errors.h>

namespace computoc::types {
    namespace details {
        template <typename T>
        concept Integer = std::is_integral_v<T> && !std::is_same_v<T, bool> && std::is_signed_v<T>;

#ifndef DECIMAL_CONCEPT_PATCH
#define DECIMAL_CONCEPT_PATCH
        template <typename T>
        concept Decimal = std::is_floating_point_v<T>;
#endif // DECIMAL_CONCEPT_PATCH

        template <Integer I = int, Decimal F = float>
        class Fraction {
        public:
            Fraction(I n = I{ 0 }, I d = I{ 1 })
                : n_(n), d_(d)
            {
                COMPUTOC_THROW_IF_FALSE(d_ != I{ 0 }, std::invalid_argument, "denominator is zero");

                if (n_ == I{ 0 }) {
                    return;
                }

                I sign = (n_ < I{ 0 } || d_ < I{ 0 }) ? I{ -1 } : I{ 1 };
                n_ = std::abs(n_) * sign;
                d_ = std::abs(d_);

                I g = gcd(n_, d_);
                n_ /= g;
                d_ /= g;
            }

            Fraction(F d)
            {
                *this = decimal_to_fraction(d);
            }

            I n() const noexcept
            {
                return n_;
            }

            I d() const noexcept
            {
                return d_;
            }

            template <Integer I_o, Decimal F_o>
            friend Fraction<I_o, F_o> operator-(const Fraction<I_o, F_o>& other) noexcept;

            template <Integer I_o, Decimal F_o>
            friend Fraction<I_o, F_o> operator+(const Fraction<I_o, F_o>& other) noexcept;

            template <Integer I_o, Decimal F_o>
            friend Fraction<I_o, F_o> reciprocal(const Fraction<I_o, F_o>& other);

            template <Integer I_o, Decimal F_o>
            friend bool operator==(const Fraction<I_o, F_o>& lhs, const Fraction<I_o, F_o>& rhs) noexcept;

            operator F() const noexcept
            {
                return static_cast<F>(n_) / static_cast<F>(d_);
            }

            template <Integer I_o, Decimal F_o>
            friend Fraction<I_o, F_o> operator+(const Fraction<I_o, F_o>& lhs, const Fraction<I_o, F_o>& rhs) noexcept;

            Fraction<I, F>& operator+=(const Fraction<I, F> other) noexcept
            {
                n_ = n_ * other.d_ + other.n_ * d_;
                d_ = d_ * other.d_;
                I g{ gcd(n_, d_) };
                n_ /= g;
                d_ /= g;
                return *this;
            }

            template <Integer I_o, Decimal F_o>
            friend Fraction<I_o, F_o> operator-(const Fraction<I_o, F_o>& lhs, const Fraction<I_o, F_o>& rhs) noexcept;

            Fraction<I, F>& operator-=(const Fraction<I, F> other) noexcept
            {
                return operator+=(-other);
            }

            template <Integer I_o, Decimal F_o>
            friend Fraction<I_o, F_o> operator*(const Fraction<I_o, F_o>& lhs, const Fraction<I_o, F_o>& rhs) noexcept;

            Fraction<I, F>& operator*=(const Fraction<I, F> other) noexcept
            {
                n_ *= other.n_;
                d_ *= other.d_;
                I g{ gcd(n_, d_) };
                n_ /= g;
                d_ /= g;
                return *this;
            }

            template <Integer I_o, Decimal F_o>
            friend Fraction<I_o, F_o> operator/(const Fraction<I_o, F_o>& lhs, const Fraction<I_o, F_o>& rhs) noexcept;

            Fraction<I, F>& operator/=(const Fraction<I, F> other) noexcept
            {
                return operator*=(reciprocal(other));
            }

        private:
            static I gcd(I a, I b) noexcept
            {
                I t;
                if (a > b) {
                    t = b;
                    b = a;
                    a = t;
                }
                while (b != I{ 0 }) {
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
            static Fraction<I, F> decimal_to_fraction(F decimal, F accuracy = F{ 1e-19 }) noexcept
            {
                F sign = decimal >= F{ 0 } ? F{ 1 } : F{ -1 };

                F decimal_abs = std::abs(decimal);

                F decimal_int_part{ std::floor(decimal_abs) };

                if (decimal_abs == decimal_int_part) {
                    return { static_cast<I>(sign * decimal_abs), I{1} };
                }

                F z_i{ decimal_abs };
                F d_i{ 1 };
                F d_i_minus_1{ 0 };

                F n_i{ 0 }; // Used for overflow check

                F z_i_plus_1{ F{1} / (z_i - std::floor(z_i)) };
                F d_i_plus_1{ d_i * std::floor(z_i_plus_1) + d_i_minus_1 };
                F n_i_plus_1{ std::round(decimal_abs * d_i_plus_1) };

                F z_i_int_part{ std::floor(z_i) };

                while (
                    z_i_int_part != z_i &&
                    std::abs(decimal_abs - n_i_plus_1 / d_i_plus_1) > accuracy) {
                    z_i = z_i_plus_1;
                    d_i_minus_1 = d_i;
                    d_i = d_i_plus_1;

                    n_i = n_i_plus_1;

                    z_i_plus_1 = F{ 1 } / (z_i - std::floor(z_i));
                    d_i_plus_1 = d_i * std::floor(z_i_plus_1) + d_i_minus_1;
                    n_i_plus_1 = std::round(decimal_abs * d_i_plus_1);

                    z_i_int_part = std::floor(z_i);

                    F max_int{ static_cast<F>(std::numeric_limits<I>::max()) };
                    if (n_i_plus_1 > max_int || d_i_plus_1 > max_int) {
                        return { static_cast<I>(sign * n_i), static_cast<I>(d_i) };
                    }
                }
                return { static_cast<I>(sign * n_i_plus_1), static_cast<I>(d_i_plus_1) };
            }

            I n_{ 0 };
            I d_{ 0 };
        };

        template<Integer I, Decimal F>
        inline Fraction<I, F> operator-(const Fraction<I, F>& other) noexcept
        {
            return { -other.n_, other.d_ };
        }

        template<Integer I, Decimal F>
        inline Fraction<I, F> operator+(const Fraction<I, F>& other) noexcept
        {
            return other;
        }

        template<Integer I, Decimal F>
        inline Fraction<I, F> reciprocal(const Fraction<I, F>& other)
        {
            COMPUTOC_THROW_IF_FALSE(other.n_ != I{ 0 }, std::overflow_error, "division by zero");

            I sign = (other.n_ * other.d_) / std::abs(other.n_ * other.d_);
            return { other.d_ * sign, std::abs(other.n_) };
        }

        template<Integer I, Decimal F>
        inline bool operator==(const Fraction<I, F>& lhs, const Fraction<I, F>& rhs) noexcept
        {
            return lhs.n_ == rhs.n_ && lhs.d_ == rhs.d_;
        }

        template<Integer I, Decimal F>
        inline Fraction<I, F> operator+(const Fraction<I, F>& lhs, const Fraction<I, F>& rhs) noexcept
        {
            Fraction<I, F> sum{ lhs.n_ * rhs.d_ + rhs.n_ * lhs.d_, lhs.d_ * rhs.d_ };
            I g{ Fraction<I, F>::gcd(sum.n_, sum.d_) };
            sum.n_ /= g;
            sum.d_ /= g;
            return sum;
        }

        template<Integer I, Decimal F>
        inline Fraction<I, F> operator-(const Fraction<I, F>& lhs, const Fraction<I, F>& rhs) noexcept
        {
            return operator+(lhs, -rhs);
        }

        template<Integer I, Decimal F>
        inline Fraction<I, F> operator*(const Fraction<I, F>& lhs, const Fraction<I, F>& rhs) noexcept
        {
            Fraction<I, F> multiplication{ lhs.n_ * rhs.n_, lhs.d_ * rhs.d_ };
            I g{ Fraction<I, F>::gcd(multiplication.n_, multiplication.d_) };
            multiplication.n_ /= g;
            multiplication.d_ /= g;
            return multiplication;
        }

        template<Integer I, Decimal F>
        inline Fraction<I, F> operator/(const Fraction<I, F>& lhs, const Fraction<I, F>& rhs) noexcept
        {
            return operator*(lhs, reciprocal(rhs));
        }
    }

    using details::Fraction;
}

namespace computoc {
    using types::details::reciprocal;
}

#endif // COMPUTOC_TYPES_FRACTION_H
