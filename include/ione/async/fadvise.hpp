#pragma once

#include <ione/context.hpp>

#include <sys/mman.h>

namespace ione {
namespace detail {

class [[nodiscard]] FadviseAwaiter : public CompletionAwaiter {
 public:
  using Result = int;

  FadviseAwaiter(Context& ctx, int fd, uint64_t offset, uint64_t length, int advise) {
    SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_fadvise(sqe, fd, offset, length, advise);
    });
  }

  Result await_resume() {
    return this->res;
  }
};

}  // namespace detail

YACLIB_INLINE auto Fadvise(Context& ctx, int fd, uint64_t offset, uint64_t length, int advise) {
  return detail::FadviseAwaiter{ctx, fd, offset, length, advise};
}

}  // namespace ione