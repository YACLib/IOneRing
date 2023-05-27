#pragma once

#include <ione/context.hpp>

namespace ione {
namespace detail {

class [[nodiscard]] Fallocate : public CompletionAwaiter {
 public:
  using Result = int;

  Fallocate(Context& ctx, int fd, int mode, uint64_t offset, uint64_t length) {
    SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_fallocate(sqe, fd, mode, offset, length);
    });
  }

  Result await_resume() {
    return this->res;
  }
};

}  // namespace detail

YACLIB_INLINE auto Fallocate(Context& ctx, int fd, int mode, uint64_t offset, uint64_t length) {
  return detail::Fallocate{ctx, fd, mode, offset, length};
}

}  // namespace ione
