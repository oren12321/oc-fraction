#include <gtest/gtest.h>

#include <numbers>
#include <cmath>

#include <math/core/fraction.h>

TEST(Fraction_test, can_be_initialized_n_and_d_or_an_integer)
{
    using namespace math::core::types;

    Fraction f1{};
    EXPECT_EQ(f1.n(), 0);
    EXPECT_EQ(f1.d(), 1);

    Fraction f2{ 2, 4 };
    EXPECT_EQ(f2.n(), 1);
    EXPECT_EQ(f2.d(), 2);

    Fraction f3 = -2;
    EXPECT_EQ(f3.n(), -2);
    EXPECT_EQ(f3.d(), 1);

    Fraction f4{ 10, 45 };
    EXPECT_EQ(f4.n(), 2);
    EXPECT_EQ(f4.d(), 9);

    Fraction f5{ 1, -2 };
    EXPECT_EQ(f5.n(), -1);
    EXPECT_EQ(f5.d(), 2);
}

TEST(Fraction_test, can_be_initalized_with_a_decimal)
{
    using namespace math::core::types;

    Fraction f1 = 0.0;
    EXPECT_EQ(f1.n(), 0);
    EXPECT_EQ(f1.d(), 1);

    Fraction f2 = -0.263157894737;
    EXPECT_EQ(f2.n(), -5);
    EXPECT_EQ(f2.d(), 19);

    Fraction f3 = 0.606557377049;
    EXPECT_EQ(f3.n(), 37);
    EXPECT_EQ(f3.d(), 61);
}

TEST(Fraction_test, can_be_compared_with_other_fraction_integer_or_decimal)
{
    using namespace math::core::types;

    Fraction f1{ 1, 2 };
    Fraction f2 = 0.5;
    EXPECT_EQ(f1, f2);
    EXPECT_NE(f1, 1);
    EXPECT_EQ(f1, 0.5);
    EXPECT_EQ(0.5, f1);
}

TEST(Fraction_test, can_be_negated)
{
    using namespace math::core::types;

    Fraction f{ -Fraction{1, 2} };
    EXPECT_EQ(f.n(), -1);
    EXPECT_EQ(f.d(), 2);
}

TEST(Fraction_test, have_reciprocal)
{
    using namespace math::core::types;

    Fraction f1 = 0;
    EXPECT_THROW(f1.reciprocal(), std::overflow_error);

    Fraction f2{ Fraction{-1, 2}.reciprocal() };
    EXPECT_EQ(f2.n(), -2);
    EXPECT_EQ(f2.d(), 1);
}

TEST(Fraction_test, can_have_improved_accuracy)
{
    using namespace math::core::types;
    using namespace std::numbers;

    Fraction<int, float> f1 = static_cast<float>(pi);
    Fraction<long, double> f2 = pi;

    double error1 = std::abs(pi - static_cast<double>(static_cast<float>(f1)));
    double error2 = std::abs(pi - static_cast<double>(f2));

    EXPECT_GE(error1, error2);
}

TEST(Fraction_test, can_be_added_to_a_fraction_integer_or_decimal)
{
    using namespace math::core::types;

    Fraction f1{ 1, 2 };
    EXPECT_EQ((f1 + Fraction{ 1, 2 }), (Fraction{ 1 }));
    EXPECT_EQ(0.5 + f1, 1);
    EXPECT_EQ(f1 + 1, 1.5);

    f1 += 1;
    EXPECT_EQ(f1, 1.5);
}

TEST(Fraction_test, can_be_subracted_from_a_fraction_integer_or_decimal)
{
    using namespace math::core::types;

    Fraction f1{ 1, 2 };
    EXPECT_EQ((f1 - Fraction{ 1, 2 }), (Fraction{}));
    EXPECT_EQ(-0.5 + f1, 0);
    EXPECT_EQ(f1 - 1, -0.5);

    f1 -= 1;
    EXPECT_EQ(f1, -0.5);
}

TEST(Fraction_test, can_be_multiply_by_a_fraction_integer_or_decimal)
{
    using namespace math::core::types;

    Fraction f1{ 1, 2 };
    EXPECT_EQ((f1 * Fraction{ 1, 2 }), (Fraction{1, 4}));
    EXPECT_EQ(0.5 * f1, 0.25);
    EXPECT_EQ(f1 * 1, 0.5);

    f1 *= 2;
    EXPECT_EQ(f1, 1);
}

TEST(Fraction_test, can_be_divided_by_a_fraction_integer_or_decimal)
{
    using namespace math::core::types;

    Fraction f1{ 1, 2 };
    EXPECT_EQ((f1 / Fraction{ 1, 2 }), (Fraction{ 1, 1 }));
    EXPECT_EQ(0.5 / f1, 1);
    EXPECT_EQ(f1 / 2, 0.25);

    f1 /= 2;
    EXPECT_EQ(f1, 0.25);
}

TEST(Fraction_test, can_be_powered_by_a_fraction_integer_or_decimal)
{
    using namespace math::core::types;

    Fraction f1{ 1, 4 };
    EXPECT_EQ(std::pow(f1, 0), 1);
    EXPECT_EQ(std::pow(f1, 0.5f), 0.5);
    EXPECT_EQ(std::pow(0.0625f, f1), 0.5);

    f1 = std::pow(f1, 2);
    EXPECT_EQ(f1, 0.0625);
}

TEST(Fraction_test, complex_expressions_can_be_computed)
{
    using namespace math::core::types;

    Fraction f1 = (Fraction{ 1,3 } - Fraction{ 1, 4 }) / (Fraction{ 1, 8 } + Fraction{ 1, 2 });
    Fraction f2{ 2, 15 };

    EXPECT_EQ(f2, f1);
}
