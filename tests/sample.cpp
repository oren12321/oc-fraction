#include <gtest/gtest.h>

#include <samplelib/sample.h>

TEST(samplelib, add) {
    EXPECT_EQ(2.0, add(1.0, 1.0));
    EXPECT_EQ(6, add(2, 3));
}
