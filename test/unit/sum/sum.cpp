#include <ione/sum/sum.hpp>

#include <gtest/gtest.h>

namespace {

TEST(Simple, JustWorks) {
  EXPECT_EQ(ione::Sum(2, 3), 5);
}

}  // namespace
