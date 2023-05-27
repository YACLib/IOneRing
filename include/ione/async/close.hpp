#pragma once

#include <ione/context.hpp>

namespace ione {
namespace detail {

class [[nodiscard]] CloseAwaiter : public CompletionAwaiter {
 public:
  using Result = int;

  CloseAwaiter(Context& ctx, int fd) {
    SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_close(sqe, fd);
    });
  }

  Result await_resume() {
    return this->res;
  }
};

}  // namespace detail

YACLIB_INLINE auto Close(Context& ctx, int fd) {
  return detail::CloseAwaiter{ctx, fd};
}

}  // namespace ione