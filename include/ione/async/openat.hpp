#pragma once

#include <ione/context.hpp>

namespace ione {
namespace detail {

template <typename Func>
class [[nodiscard]] OpenAtAwaiter : private ScopeAwaiter<Func> {
 public:
  using Result = int;

  OpenAtAwaiter(Context& ctx, int dirfd, const char* path, int flags, mode_t mode, Func func)
    : ScopeAwaiter<Func>{std::move(func)} {
    this->SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_openat(sqe, dirfd, path, flags, mode);
    });
  }

  Result await_resume() {
    this->_func();
    return {this->res};
  }
};

}  // namespace detail

template <typename Func>
YACLIB_INLINE auto OpenAt(Context& ctx, int dirfd, const char* path, int flags, mode_t mode, Func&& func) {
  return detail::OpenAtAwaiter{ctx, dirfd, path, flags, mode, std::forward<Func>(func)};
}

YACLIB_INLINE auto OpenAt(Context& ctx, int dirfd, const char* path, int flags, mode_t mode) {
  return OpenAt(ctx, dirfd, path, flags, mode, []{});
}

}  // namespace ione