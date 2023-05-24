#pragma once

#include <ione/context.hpp>

namespace ione {
namespace detail {

template <typename Func>
class [[nodiscard]] StatxAwaiter : private ScopeAwaiter<Func> {
 public:
  struct Result {
    statx statx{};
    int error_code;
  };

  StatxAwaiter(Context& ctx, int dirfd, const char* path, int flags, unsigned mask, Func func)
    : ScopeAwaiter<Func>{std::move(func)} {
    this->SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_statx(sqe, dirfd, path, flags, mask, &_statx);
    });
  }

  Result await_resume() {
    this->_func();
    return {_statx, this->res};
  }

 private:
  statx _statx{};
};

}  // namespace detail

YACLIB_INLINE auto Statx(Context& ctx, int dirfd, const char* path, int flags, unsigned mask) {
  return detail::StatxAwaiter{ctx, dirfd, path, flags, mask, yaclib::Unit{}};
}

template <typename Func>
YACLIB_INLINE auto Statx(Context& ctx, int dirfd, const char* path, int flags, unsigned mask, Func&& func) {
  return detail::StatxAwaiter{ctx, dirfd, path, flags, mask, std::forward<Func>(func)};
}

YACLIB_INLINE auto Statx(Context& ctx, std::filesystem::path path, int flags, unsigned mask) {
  auto absolute_path = absolute(path);
  return detail::StatxAwaiter{ctx, 0, absolute_path.c_str(), flags, mask, [p = std::move(absolute_path)]() mutable {
                                p = {};
                              }};
}

}  // namespace ione