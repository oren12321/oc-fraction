#include <gtest/gtest.h>

#include <numbers>
#include <cmath>
#include <sstream>

#include <oc/fraction.h>

TEST(fraction_test, can_be_initialized_n_and_d_or_an_integer)
{
    using namespace oc;

    fraction f1{};
    EXPECT_EQ(f1.n(), 0);
    EXPECT_EQ(f1.d(), 1);

    fraction f2{ 2, 4 };
    EXPECT_EQ(f2.n(), 1);
    EXPECT_EQ(f2.d(), 2);

    fraction f3 = -2;
    EXPECT_EQ(f3.n(), -2);
    EXPECT_EQ(f3.d(), 1);

    fraction f4{ 10, 45 };
    EXPECT_EQ(f4.n(), 2);
    EXPECT_EQ(f4.d(), 9);

    fraction f5{ 1, -2 };
    EXPECT_EQ(f5.n(), -1);
    EXPECT_EQ(f5.d(), 2);

    EXPECT_THROW((fraction{ 0, 0 }), std::overflow_error);
}

TEST(fraction_test, can_be_initalized_with_a_decimal)
{
    using namespace oc;

    fraction f1 = 0.0;
    EXPECT_EQ(f1.n(), 0);
    EXPECT_EQ(f1.d(), 1);

    fraction f2 = -0.263157894737;
    EXPECT_EQ(f2.n(), -5);
    EXPECT_EQ(f2.d(), 19);

    fraction f3 = 0.606557377049;
    EXPECT_EQ(f3.n(), 37);
    EXPECT_EQ(f3.d(), 61);
}

TEST(fraction_test, can_be_compared_with_other_fraction_integer_or_decimal)
{
    using namespace oc;

    fraction f1{ 1, 2 };
    fraction f2 = 0.5;
    EXPECT_EQ(f1, f2);
    EXPECT_NE(f1, 1);
    EXPECT_EQ(f1, 0.5);
    EXPECT_EQ(0.5, f1);

    EXPECT_EQ(f1, (fraction<long, double>{1, 2}));
    EXPECT_EQ(f2, (fraction<long, float>{1, 2}));
    EXPECT_EQ(f2, 0.5f);
}

TEST(fraction_test, can_be_negated)
{
    using namespace oc;

    fraction f{ -fraction{1, 2} };
    EXPECT_EQ(f.n(), -1);
    EXPECT_EQ(f.d(), 2);
}

TEST(fraction_test, have_reciprocal)
{
    using namespace oc;

    fraction f1 = 0;
    EXPECT_THROW((void)reciprocal(f1), std::overflow_error);

    fraction f2{ reciprocal(fraction{-1, 2}) };
    EXPECT_EQ(f2.n(), -2);
    EXPECT_EQ(f2.d(), 1);
}

TEST(fraction_test, can_have_improved_accuracy)
{
    using namespace oc;
    using namespace std::numbers;

    fraction<int, float> f1 = static_cast<float>(pi);
    fraction<long, double> f2 = pi;

    double error1 = std::abs(pi - static_cast<double>(static_cast<float>(f1)));
    double error2 = std::abs(pi - static_cast<double>(f2));

    EXPECT_GE(error1, error2);
}

TEST(fraction_test, can_be_added_to_a_fraction_integer_or_decimal)
{
    using namespace oc;

    fraction f1{ 1, 2 };
    EXPECT_EQ((f1 + fraction{ 1, 2 }), (fraction{ 1 }));
    EXPECT_EQ(0.5 + f1, 1);
    EXPECT_EQ(f1 + 1, 1.5);

    f1 += 1;
    EXPECT_EQ(f1, 1.5);

    EXPECT_EQ(f1, (f1 + fraction<long, float>{}));
    f1 += fraction<long, float>{1l};
    EXPECT_EQ(f1, 2.5);
}

TEST(fraction_test, can_be_subracted_from_a_fraction_integer_or_decimal)
{
    using namespace oc;

    fraction f1{ 1, 2 };
    EXPECT_EQ((f1 - fraction{ 1, 2 }), (fraction{}));
    EXPECT_EQ(-0.5 + f1, 0);
    EXPECT_EQ(f1 - 1, -0.5);

    f1 -= 1;
    EXPECT_EQ(f1, -0.5);

    EXPECT_EQ(f1, (f1 + fraction<long, float>{}));
    f1 -= fraction<long, float>{1l};
    EXPECT_EQ(f1, -1.5);
}

TEST(fraction_test, can_be_multiply_by_a_fraction_integer_or_decimal)
{
    using namespace oc;

    fraction f1{ 1, 2 };
    EXPECT_EQ((f1 * fraction{ 1, 2 }), (fraction{1, 4}));
    EXPECT_EQ(0.5 * f1, 0.25);
    EXPECT_EQ(f1 * 1, 0.5);

    f1 *= 2;
    EXPECT_EQ(f1, 1);

    EXPECT_EQ(f1, (f1 * fraction<long, float>{1l}));
    f1 *= fraction<long, float>{2l};
    EXPECT_EQ(f1, 2.0);
}

TEST(fraction_test, can_be_divided_by_a_fraction_integer_or_decimal)
{
    using namespace oc;

    fraction f1{ 1, 2 };
    EXPECT_EQ((f1 / fraction{ 1, 2 }), (fraction{ 1, 1 }));
    EXPECT_EQ(0.5 / f1, 1);
    EXPECT_EQ(f1 / 2, 0.25);

    f1 /= 2;
    EXPECT_EQ(f1, 0.25);

    EXPECT_EQ(f1, (f1 / fraction<long, float>{1l}));
    f1 /= fraction<long, float>{2l};
    EXPECT_EQ(f1, 0.125);
}

TEST(fraction_test, can_be_powered_by_a_fraction_integer_or_decimal)
{
    using namespace oc;

    fraction f1{ 1, 4 };
    EXPECT_EQ(std::pow(f1, 0), 1);
    EXPECT_EQ(std::pow(f1, 0.5f), 0.5);
    EXPECT_EQ(std::pow(0.0625f, f1), 0.5);

    f1 = std::pow(f1, 2);
    EXPECT_EQ(f1, 0.0625);
}

TEST(fraction_test, complex_expressions_can_be_computed)
{
    using namespace oc;

    fraction f1 = (fraction{ 1,3 } - fraction{ 1, 4 }) / (fraction{ 1, 8 } + fraction{ 1, 2 });
    fraction f2{ 2, 15 };

    EXPECT_EQ(f2, f1);
}

TEST(fraction_test, copy)
{
    using namespace oc;

    {
        fraction f1{ 1, 2 };

        fraction f2{ f1 };
        EXPECT_EQ(f1, f2);

        fraction f3{};
        f3 = f2;
        EXPECT_EQ(f2, f3);
    }

    {
        fraction f1{ 1, 2 };

        fraction<int, float> f2{ f1 };
        EXPECT_EQ(f1, f2);

        fraction<long, double> f3{};
        f3 = f2;
        EXPECT_EQ(f2, f3);
    }
}

TEST(fraction_test, move)
{
    using namespace oc;

    {
        fraction f1{ 1, 2 };
        fraction f1c{ f1 };

        fraction f2{ std::move(f1) };
        EXPECT_EQ(f1c, f2);

        fraction f3{};
        f3 = std::move(f2);
        EXPECT_EQ(f1c, f3);
    }

    {
        fraction f1{ 1, 2 };
        fraction f1c{ f1 };

        fraction<int, float> f2{ std::move(f1) };
        EXPECT_EQ(f1c, f2);

        fraction<long, double> f3{};
        f3 = std::move(f2);
        EXPECT_EQ(f1c, f3);
    }
}

TEST(fraction_test, print)
{
    using namespace oc;

    {
        fraction f = 5;
        std::stringstream ss;
        ss << f;
        EXPECT_EQ(ss.str(), "5");
    }

    {
        fraction f = 0;
        std::stringstream ss;
        ss << f;
        EXPECT_EQ(ss.str(), "0");
    }

    {
        fraction f(1, -5);
        std::stringstream ss;
        ss << f;
        EXPECT_EQ(ss.str(), "-1/5");
    }
}