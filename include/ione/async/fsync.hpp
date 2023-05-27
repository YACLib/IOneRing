#pragma once

#include <ione/context.hpp>

namespace ione {
namespace detail {

class [[nodiscard]] FsyncAwaiter : public CompletionAwaiter {
 public:
  using Result = int;

  FsyncAwaiter(Context& ctx, int fd, unsigned flags) {
    SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_fsync(sqe, fd, flags);
    });
  }

  Result await_resume() {
    return this->res;
  }
};

}  // namespace detail

YACLIB_INLINE auto Fsync(Context& ctx, int fd, unsigned flags) {
  return detail::FsyncAwaiter{ctx, fd, flags};
}

}  // namespace ione
