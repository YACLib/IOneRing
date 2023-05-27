#pragma once

#include <ione/context.hpp>

namespace ione {
namespace detail {

template <typename Func>
class [[nodiscard]] OpenAt2Awaiter : public ScopeAwaiter<Func> {
 public:
  using Result = int;

  OpenAt2Awaiter(Context& ctx, int dirfd, const char* path, open_how how, Func func)
    : ScopeAwaiter<Func>{std::move(func)}, _how{how} {
    this->SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_openat2(sqe, dirfd, path, &_how);
    });
  }

  Result await_resume() {
    this->_func();
    return {this->res};
  }

 private:
  open_how _how;
};

}  // namespace detail

template <typename Func>
YACLIB_INLINE auto OpenAt2(Context& ctx, int dirfd, const char* path, open_how how, Func&& func) {
  return detail::OpenAt2Awaiter{ctx, dirfd, path, how, std::forward<Func>(func)};
}

YACLIB_INLINE auto OpenAt2(Context& ctx, int dirfd, const char* path, open_how how) {
  return OpenAt2(ctx, dirfd, path, how, [] {
  });
}

}  // namespace ione
