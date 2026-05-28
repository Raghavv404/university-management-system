#include <gtest/gtest.h>

#include "../../src/utils/IdGenerator.h"

TEST(IdGeneratorTest, IdsAreMonotonicallyIncreasing) {
    IdGenerator gen;
    EXPECT_EQ(gen.generateId(), 1u);
    EXPECT_EQ(gen.generateId(), 2u);
    EXPECT_EQ(gen.generateId(), 3u);
}

TEST(IdGeneratorTest, SeparateInstancesAreIndependent) {
    IdGenerator a, b;
    EXPECT_EQ(a.generateId(), 1u);
    EXPECT_EQ(a.generateId(), 2u);
    EXPECT_EQ(b.generateId(), 1u);    // b starts fresh, doesn't share state
    EXPECT_EQ(b.generateId(), 2u);
    EXPECT_EQ(a.generateId(), 3u);
}
