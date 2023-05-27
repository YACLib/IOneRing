#include <ione/context.hpp>

#include <gtest/gtest.h>

namespace {

TEST(Context, EmptySubmit) {
  static constexpr int kEntries = 1;
  static constexpr int kFlags = 0;

  ione::Context ctx;
  ctx.Init(kEntries, kFlags);
  EXPECT_EQ(ctx.Submit(), 0);
}

}  // namespace
