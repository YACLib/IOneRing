#pragma once

#include <ione/context.hpp>

namespace ione {
namespace detail {

class [[nodiscard]] SpliceAwaiter : private CompletionAwaiter {
 public:
  using Result = int;

  SpliceAwaiter(Context& ctx, int fd_in, int64_t off_in, int fd_out, int64_t off_out, size_t len, unsigned flags) {
    SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_splice(sqe, fd_in, off_in, fd_out, off_out, len, flags);
    });
  }

  Result await_resume() {
    return res;
  }
};

}  // namespace detail

YACLIB_INLINE auto Splice(Context& ctx, int fd_in, uint64_t off_in, int fd_out, uint64_t off_out, size_t len,
                          unsigned flags) {
  return detail::SpliceAwaiter{ctx, fd_in, static_cast<int64_t>(off_in), fd_out, static_cast<int64_t>(off_out),
                               len, flags};
}

YACLIB_INLINE auto Splice(Context& ctx, int fd_in, uint64_t off_in, int fd_out, size_t len, unsigned flags) {
  return detail::SpliceAwaiter{ctx, fd_in, static_cast<int64_t>(off_in), fd_out, -1, len, flags};
}

YACLIB_INLINE auto Splice(Context& ctx, int fd_in, int fd_out, uint64_t off_out, size_t len, unsigned flags) {
  return detail::SpliceAwaiter{ctx, fd_in, -1, fd_out, static_cast<int64_t>(off_out), len, flags};
}

YACLIB_INLINE auto Splice(Context& ctx, int fd_in, int fd_out, size_t len, unsigned flags) {
  return detail::SpliceAwaiter{ctx, fd_in, -1, fd_out, -1, len, flags};
}

}  // namespace ione