#include <gtest/gtest.h>

#include <sample/sample.h>

TEST(sample, add) {
    EXPECT_EQ(2.0, sample::add(1.0, 1.0));
    EXPECT_EQ(5, sample::add(2, 3));
}
