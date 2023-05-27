#pragma once

#include <ione/context.hpp>

#include <span>

namespace ione {
namespace detail {

template <typename Func>
class [[nodiscard]] SendAwaiter : public ScopeAwaiter<Func> {
 public:
  using Result = int;

  SendAwaiter(Context& ctx, int fd, const void* data, size_t size, int flags, Func func)
    : ScopeAwaiter<Func>{std::move(func)} {
    this->SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_send(sqe, fd, data, size, flags);
    });
  }

  Result await_resume() {
    this->_func();
    return this->res;
  }
};

}  // namespace detail

template <typename Func>
YACLIB_INLINE auto Send(Context& ctx, int fd, const void* data, size_t size, int flags, Func&& func) {
  return detail::SendAwaiter{ctx, fd, data, size, flags, std::forward<Func>(func)};
}

YACLIB_INLINE auto Send(Context& ctx, int fd, const void* data, size_t size, int flags) {
  return Send(ctx, fd, data, size, flags, [] {
  });
}

template <typename T>
auto Send(Context& ctx, int fd, std::vector<T> buffer, int flags) {
  static_assert(std::is_standard_layout_v<T>);

  auto data = buffer.data();
  auto size = buffer.size() * sizeof(T);
  return Send(ctx, fd, data, size, flags, [buf = std::move(buffer)]() mutable {
    buf = {};
  });
}

}  // namespace ione