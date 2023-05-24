#pragma once

#include <ione/context.hpp>

namespace ione {
namespace detail {

class [[nodiscard]] NopAwaiter : private CompletionAwaiter {
 public:
  using Result = int;

  NopAwaiter(Context& ctx) {
    SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_nop(sqe);
    });
  }

  Result await_resume() {
    return this->res;
  }
};

}  // namespace detail

YACLIB_INLINE auto Nop(Context& ctx) {
  // TODO: maybe return void?
  return detail::NopAwaiter{ctx};
}

}  // namespace ione