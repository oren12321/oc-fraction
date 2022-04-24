#include <gtest/gtest.h>

#include <math/core/fraction.h>

TEST(Fraction_test, can_be_initialized_n_and_d_or_an_integer)
{
    using namespace math::core::types;

    Fraction<int> f1{};
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

    Fraction<int> f1 = 0.0;
    EXPECT_EQ(f1.n(), 0);
    EXPECT_EQ(f1.d(), 1);

    Fraction<int> f2 = 0.263157894737;
    EXPECT_EQ(f2.n(), 5);
    EXPECT_EQ(f2.d(), 19);

    Fraction<int> f3 = 0.606557377049;
    EXPECT_EQ(f3.n(), 37);
    EXPECT_EQ(f3.d(), 61);
}
