#ifndef OC_FRACTION_H
#define OC_FRACTION_H

#include <limits>
#include <stdexcept>
#include <concepts>
#include <cmath>
#include <numeric>
#include <limits>

namespace oc {
    namespace details {
        template <std::integral I = int, std::floating_point F = float>
        class fraction final {
        public:
            constexpr fraction(I n = I{ 0 }, I d = I{ 1 })
                : n_(n), d_(d)
            {
                if (d_ == I{ 0 }) {
                    throw std::overflow_error{ "division by zero" };
                }

                if (n_ == I{ 0 }) {
                    d_ = I{ 1 };
                    return;
                }

                I sign = (n_ < I{ 0 } || d_ < I{ 0 }) ? I{ -1 } : I{ 1 };
                n_ = std::abs(n_) * sign;
                d_ = std::abs(d_);

                I g = std::gcd(n_, d_);
                n_ /= g;
                d_ /= g;
            }

            constexpr fraction(F d) noexcept
            {
                *this = decimal_to_fraction<I, F>(d);
            }

            constexpr fraction(const fraction<I, F>& other) = default;
            constexpr fraction<I, F>& operator=(const fraction<I, F>& other) = default;

            constexpr fraction(fraction<I, F>&& other) = default;
            constexpr fraction<I, F>& operator=(fraction<I, F>&& other) = default;

            ~fraction() = default;

            template <std::integral I_o, std::floating_point F_o>
            constexpr fraction(const fraction<I_o, F_o>& other) noexcept
                : n_(other.n()), d_(other.d())
            {
            }
            template <std::integral I_o, std::floating_point F_o>
            constexpr fraction<I_o, F_o> operator=(const fraction<I_o, F_o>& other) noexcept
            {
                n_ = other.n();
                d_ = other.d();

                return *this;
            }

            template <std::integral I_o, std::floating_point F_o>
            constexpr fraction(fraction<I_o, F_o>&& other) noexcept
                : n_(other.n()), d_(other.d())
            {
            }
            template <std::integral I_o, std::floating_point F_o>
            constexpr fraction<I_o, F_o> operator=(fraction<I_o, F_o>&& other) noexcept
            {
                n_ = other.n();
                d_ = other.d();

                return *this;
            }

            [[nodiscard]] constexpr I n() const noexcept
            {
                return n_;
            }

            [[nodiscard]] constexpr I d() const noexcept
            {
                return d_;
            }

            [[nodiscard]] constexpr operator F() const noexcept
            {
                return static_cast<F>(n_) / static_cast<F>(d_);
            }

            constexpr fraction<I, F>& operator+=(const fraction<I, F>& other) noexcept
            {
                n_ = n_ * other.d_ + other.n_ * d_;
                d_ = d_ * other.d_;
                I g{ std::gcd(n_, d_) };
                n_ /= g;
                d_ /= g;
                return *this;
            }

            constexpr fraction<I, F>& operator-=(const fraction<I, F>& other) noexcept
            {
                return operator+=(-other);
            }

            constexpr fraction<I, F>& operator*=(const fraction<I, F>& other) noexcept
            {
                n_ *= other.n_;
                d_ *= other.d_;
                I g{ std::gcd(n_, d_) };
                n_ /= g;
                d_ /= g;
                return *this;
            }

            constexpr fraction<I, F>& operator/=(const fraction<I, F>& other)
            {
                return operator*=(reciprocal(other));
            }

            template <std::integral I_o, std::floating_point F_o>
            constexpr fraction<I, F>& operator+=(const fraction<I_o, F_o>& other) noexcept
            {
                n_ = n_ * other.d() + other.n() * d_;
                d_ = d_ * other.d();
                I g{ std::gcd(n_, d_) };
                n_ /= g;
                d_ /= g;
                return *this;
            }

            template <std::integral I_o, std::floating_point F_o>
            constexpr fraction<I, F>& operator-=(const fraction<I_o, F_o>& other) noexcept
            {
                return operator+=(-other);
            }

            template <std::integral I_o, std::floating_point F_o>
            constexpr fraction<I, F>& operator*=(const fraction<I_o, F_o>& other) noexcept
            {
                n_ *= other.n();
                d_ *= other.d();
                I g{ std::gcd(n_, d_) };
                n_ /= g;
                d_ /= g;
                return *this;
            }

            template <std::integral I_o, std::floating_point F_o>
            constexpr fraction<I, F>& operator/=(const fraction<I_o, F_o>& other)
            {
                return operator*=(reciprocal(other));
            }

        private:
            I n_{ 0 };
            I d_{ 1 };
        };

        /*
        * Given a decimal number, this function returns a fraction at specified accuracy with
        * respect to the decimal number.
        * 
        * The algorithm of this function is an iterative one, in such way that it tries to
        * estimate the fraction until the difference between the floating number devision
        * of the fraction numerator/denominator and the decimal number is suitable to the
        * requested accuracy.
        * 
        * Algorithm:
        * ----------
        * 
        * The main iterative variables are:
        * - Z - used for two purposes: partial denominator estimation, and to check if
        *   denominator is converged.
        * - D - the estimated denominator
        * - N - the estimated numerator
        * 
        * Considering the input decimal number is X and i is the iteration index, the
        * iteration formulas are as follows:
        *   initial values:
        *       Z0 = X
        *       D0 = 0, D1 = 1
        *       N0 = 0
        * 
        *   one iteration:
        *       Partial estimation of the denominator:
        *       Zi+1 = 1 / (Zi - floor(Zi))
        * 
        *       Denominator estimation:
        *       Di+1 = Di * floor(Zi+1) + Di-1
        *       (equivelant to Di+1 = Di / d + Di-1 s.t. d is the required estimation)
        * 
        *       Numerator estimation:
        *       Ni+1 = round(X * Di+1)
        *       (equivelant to N = (n/d)*D s.t. n and d are the required estimations)
        * 
        * These iterations should be done until one of the following conditions fulfil:
        * - The estimated denominator is converged:
        *       Zi = floor(Zi)
        * - The estimated fraction is suitable for the specified accuracy:
        *       abs(abs(X) - Ni+1 / Di+1) <= accuracy
        */
        template <std::integral I = int, std::floating_point F = float>
        [[nodiscard]] inline constexpr fraction<I, F> decimal_to_fraction(F decimal, F accuracy = F{1e-19}) noexcept
        {
            F sign = decimal >= F{0} ? F{1} : F{-1};

            // If the decimal number is equal to its integer part,
            // conversion is not required.
            if (std::abs(decimal) == std::floor(std::abs(decimal))) {
                return {static_cast<I>(sign * std::abs(decimal)), I{1}};
            }

            // Initialization
            F z_i{std::abs(decimal)};
            F d_i{1};
            F d_i_minus_1{0};

            F n_i{0}; // Used for overflow check

            // Estimation
            F z_i_plus_1{F{1} / (z_i - std::floor(z_i))};
            F d_i_plus_1{d_i * std::floor(z_i_plus_1) + d_i_minus_1};
            F n_i_plus_1{std::round(std::abs(decimal) * d_i_plus_1)};

            while (std::floor(z_i) != z_i && std::abs(std::abs(decimal) - n_i_plus_1 / d_i_plus_1) > accuracy) {
                // Save previous estimations
                z_i = z_i_plus_1;
                d_i_minus_1 = d_i;
                d_i = d_i_plus_1;

                n_i = n_i_plus_1;

                // Estimation
                z_i_plus_1 = F{1} / (z_i - std::floor(z_i));
                d_i_plus_1 = d_i * std::floor(z_i_plus_1) + d_i_minus_1;
                n_i_plus_1 = std::round(std::abs(decimal) * d_i_plus_1);

                // Perform overflow check. If either on of the current estimations
                // is overflow, return the previous estimations.
                F max_int{static_cast<F>(std::numeric_limits<I>::max())};
                if (n_i_plus_1 > max_int || d_i_plus_1 > max_int) {
                    return {static_cast<I>(sign * n_i), static_cast<I>(d_i)};
                }
            }
            return {static_cast<I>(sign * n_i_plus_1), static_cast<I>(d_i_plus_1)};
        }

        template<std::integral I, std::floating_point F>
        [[nodiscard]] inline constexpr fraction<I, F> operator-(const fraction<I, F>& other) noexcept
        {
            return { -other.n(), other.d() };
        }

        template<std::integral I, std::floating_point F>
        [[nodiscard]] inline constexpr fraction<I, F> operator+(const fraction<I, F>& other) noexcept
        {
            return other;
        }

        template<std::integral I, std::floating_point F>
        [[nodiscard]] inline constexpr fraction<I, F> reciprocal(const fraction<I, F>& other)
        {
            if (other.n() == I{ 0 }) {
                throw std::overflow_error{ "division by zero" };
            }

            I sign = (other.n() * other.d()) / std::abs(other.n() * other.d());
            return { other.d() * sign, std::abs(other.n()) };
        }

        template<std::integral I1, std::floating_point F1, std::integral I2, std::floating_point F2>
        [[nodiscard]] inline constexpr bool operator==(const fraction<I1, F1>& lhs, const fraction<I2, F2>& rhs) noexcept
        {
            return lhs.n() == rhs.n() && lhs.d() == rhs.d();
        }

        template<std::integral I1, std::floating_point F1, std::integral I2, std::floating_point F2>
        [[nodiscard]] inline constexpr fraction<decltype(I1{} + I2{}), decltype(F1{} + F2{}) > operator+(const fraction<I1, F1>& lhs, const fraction<I2, F2>& rhs) noexcept
        {
            return { lhs.n() * rhs.d() + rhs.n() * lhs.d(), lhs.d() * rhs.d() };
        }

        template<std::integral I1, std::floating_point F1, std::integral I2, std::floating_point F2>
        [[nodiscard]] inline constexpr fraction<decltype(I1{} - I2{}), decltype(F1{} - F2{}) > operator-(const fraction<I1, F1>& lhs, const fraction<I2, F2>& rhs) noexcept
        {
            return operator+(lhs, -rhs);
        }

        template<std::integral I1, std::floating_point F1, std::integral I2, std::floating_point F2>
        [[nodiscard]] inline constexpr fraction<decltype(I1{} * I2{}), decltype(F1{} * F2{}) > operator*(const fraction<I1, F1>& lhs, const fraction<I2, F2>& rhs) noexcept
        {
            return { lhs.n() * rhs.n(), lhs.d() * rhs.d() };
        }

        template<std::integral I1, std::floating_point F1, std::integral I2, std::floating_point F2>
        [[nodiscard]] inline constexpr fraction<decltype(I1{} / I2{}), decltype(F1{} / F2{}) > operator/(const fraction<I1, F1>& lhs, const fraction<I2, F2>& rhs) noexcept
        {
            return operator*(lhs, reciprocal(rhs));
        }

        template <std::integral I = int, std::floating_point F = float>
        std::ostream& operator<<(std::ostream& os, const details::fraction<I, F>& f)
        {
            if (f.d() == 1 || f.n() == 0) {
                os << f.n();
                return os;
            }

            os << f.n() << '/' << f.d();
            return os;
        }
    }

    using details::fraction;
    using details::reciprocal;
}

#endif // OC_FRACTION_H
