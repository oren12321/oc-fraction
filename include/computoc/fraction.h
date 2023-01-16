#ifndef COMPUTOC_TYPES_FRACTION_H
#define COMPUTOC_TYPES_FRACTION_H

#include <limits>
#include <cmath>
#include <stdexcept>

#include <erroc/errors.h>
#include <computoc/concepts.h>

namespace computoc {
    namespace details {
        template <Integer I = int, Decimal F = float>
        class Fraction {
        public:
            Fraction(I n = I{ 0 }, I d = I{ 1 })
                : n_(n), d_(d)
            {
                ERROC_EXPECT(d_ != I{ 0 }, std::overflow_error, "division by zero");

                if (n_ == I{ 0 }) {
                    d_ = I{ 1 };
                    return;
                }

                I sign = (n_ < I{ 0 } || d_ < I{ 0 }) ? I{ -1 } : I{ 1 };
                n_ = std::abs(n_) * sign;
                d_ = std::abs(d_);

                I g = gcd(n_, d_);
                n_ /= g;
                d_ /= g;
            }

            Fraction(F d) noexcept
            {
                *this = decimal_to_fraction(d);
            }

            Fraction(const Fraction<I, F>& other) = default;
            Fraction<I, F>& operator=(const Fraction<I, F>& other) = default;

            Fraction(Fraction<I, F>&& other) = default;
            Fraction<I, F>& operator=(Fraction<I, F>&& other) = default;

            virtual ~Fraction() = default;

            template <Integer I_o, Decimal F_o>
            Fraction(const Fraction<I_o, F_o>& other) noexcept
                : n_(other.n()), d_(other.d())
            {
            }
            template <Integer I_o, Decimal F_o>
            Fraction<I_o, F_o> operator=(const Fraction<I_o, F_o>& other) noexcept
            {
                n_ = other.n();
                d_ = other.d();

                return *this;
            }

            template <Integer I_o, Decimal F_o>
            Fraction(Fraction<I_o, F_o>&& other) noexcept
                : n_(other.n()), d_(other.d())
            {
            }
            template <Integer I_o, Decimal F_o>
            Fraction<I_o, F_o> operator=(Fraction<I_o, F_o>&& other) noexcept
            {
                n_ = other.n();
                d_ = other.d();

                return *this;
            }

            [[nodiscard]] I n() const noexcept
            {
                return n_;
            }

            [[nodiscard]] I d() const noexcept
            {
                return d_;
            }

            [[nodiscard]] operator F() const noexcept
            {
                return static_cast<F>(n_) / static_cast<F>(d_);
            }

            Fraction<I, F>& operator+=(const Fraction<I, F> other) noexcept
            {
                n_ = n_ * other.d_ + other.n_ * d_;
                d_ = d_ * other.d_;
                I g{ gcd(n_, d_) };
                n_ /= g;
                d_ /= g;
                return *this;
            }

            Fraction<I, F>& operator-=(const Fraction<I, F> other) noexcept
            {
                return operator+=(-other);
            }

            Fraction<I, F>& operator*=(const Fraction<I, F> other) noexcept
            {
                n_ *= other.n_;
                d_ *= other.d_;
                I g{ gcd(n_, d_) };
                n_ /= g;
                d_ /= g;
                return *this;
            }

            Fraction<I, F>& operator/=(const Fraction<I, F> other)
            {
                return operator*=(reciprocal(other));
            }

            template <Integer I_o, Decimal F_o>
            Fraction<I, F>& operator+=(const Fraction<I_o, F_o> other) noexcept
            {
                n_ = n_ * other.d() + other.n() * d_;
                d_ = d_ * other.d();
                I g{ gcd(n_, d_) };
                n_ /= g;
                d_ /= g;
                return *this;
            }

            template <Integer I_o, Decimal F_o>
            Fraction<I, F>& operator-=(const Fraction<I_o, F_o> other) noexcept
            {
                return operator+=(-other);
            }

            template <Integer I_o, Decimal F_o>
            Fraction<I, F>& operator*=(const Fraction<I_o, F_o> other) noexcept
            {
                n_ *= other.n();
                d_ *= other.d();
                I g{ gcd(n_, d_) };
                n_ /= g;
                d_ /= g;
                return *this;
            }

            template <Integer I_o, Decimal F_o>
            Fraction<I, F>& operator/=(const Fraction<I_o, F_o> other)
            {
                return operator*=(reciprocal(other));
            }

        private:
            [[nodiscard]] static I gcd(I a, I b) noexcept
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
            [[nodiscard]] static Fraction<I, F> decimal_to_fraction(F decimal, F accuracy = F{ 1e-19 }) noexcept
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
            I d_{ 1 };
        };

        template<Integer I, Decimal F>
        [[nodiscard]] inline Fraction<I, F> operator-(const Fraction<I, F>& other) noexcept
        {
            return { -other.n(), other.d() };
        }

        template<Integer I, Decimal F>
        [[nodiscard]] inline Fraction<I, F> operator+(const Fraction<I, F>& other) noexcept
        {
            return other;
        }

        template<Integer I, Decimal F>
        [[nodiscard]] inline Fraction<I, F> reciprocal(const Fraction<I, F>& other)
        {
            ERROC_EXPECT(other.n() != I{ 0 }, std::overflow_error, "division by zero");

            I sign = (other.n() * other.d()) / std::abs(other.n() * other.d());
            return { other.d() * sign, std::abs(other.n()) };
        }

        template<Integer I1, Decimal F1, Integer I2, Decimal F2>
        [[nodiscard]] inline bool operator==(const Fraction<I1, F1>& lhs, const Fraction<I2, F2>& rhs) noexcept
        {
            return lhs.n() == rhs.n() && lhs.d() == rhs.d();
        }

        template<Integer I1, Decimal F1, Integer I2, Decimal F2>
        [[nodiscard]] inline Fraction<decltype(I1{} + I2{}), decltype(F1{} + F2{}) > operator+(const Fraction<I1, F1>& lhs, const Fraction<I2, F2>& rhs) noexcept
        {
            return { lhs.n() * rhs.d() + rhs.n() * lhs.d(), lhs.d() * rhs.d() };
        }

        template<Integer I1, Decimal F1, Integer I2, Decimal F2>
        [[nodiscard]] inline Fraction<decltype(I1{} - I2{}), decltype(F1{} - F2{}) > operator-(const Fraction<I1, F1>& lhs, const Fraction<I2, F2>& rhs) noexcept
        {
            return operator+(lhs, -rhs);
        }

        template<Integer I1, Decimal F1, Integer I2, Decimal F2>
        [[nodiscard]] inline Fraction<decltype(I1{} * I2{}), decltype(F1{} * F2{}) > operator*(const Fraction<I1, F1>& lhs, const Fraction<I2, F2>& rhs) noexcept
        {
            return { lhs.n() * rhs.n(), lhs.d() * rhs.d() };
        }

        template<Integer I1, Decimal F1, Integer I2, Decimal F2>
        [[nodiscard]] inline Fraction<decltype(I1{} / I2{}), decltype(F1{} / F2{}) > operator/(const Fraction<I1, F1>& lhs, const Fraction<I2, F2>& rhs) noexcept
        {
            return operator*(lhs, reciprocal(rhs));
        }
    }

    using details::Fraction;
    using details::reciprocal;
}

#endif // COMPUTOC_TYPES_FRACTION_H
