#pragma once

#include <ione/context.hpp>

namespace ione {
namespace detail {

class [[nodiscard]] MadviseAwaiter : private CompletionAwaiter {
 public:
  using Result = int;

  MadviseAwaiter(Context& ctx, void* addr, uint64_t length, int advise) {
    SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_madvise(sqe, addr, length, advise);
    });
  }

  Result await_resume() {
    return this->res;
  }
};

}  // namespace detail

YACLIB_INLINE auto Madvise(Context& ctx, void* addr, uint64_t length, int advise) {
  return detail::MadviseAwaiter{ctx, addr, length, advise};
}

}  // namespace ione