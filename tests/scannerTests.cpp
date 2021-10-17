#include <gtest/gtest.h>

extern "C" {
#include "../src/scanner.h"
}

// Demonstrate some basic assertions.
TEST(test2, BasicAssertions) {
    EXPECT_EQ(1, 1);
}
