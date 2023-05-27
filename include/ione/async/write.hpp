#pragma once

#include <ione/context.hpp>

#include <span>

namespace ione {
namespace detail {

template <typename Func>
class [[nodiscard]] WriteAwaiter : public ScopeAwaiter<Func> {
 public:
  using Result = int;

  WriteAwaiter(Context& ctx, int fd, const void* data, size_t size, uint64_t offset, Func func)
    : ScopeAwaiter<Func>{std::move(func)} {
    this->SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_write(sqe, fd, data, size, offset);
    });
  }

  Result await_resume() {
    this->_func();
    return this->res;
  }
};

}  // namespace detail

template <typename Func>
YACLIB_INLINE auto Write(Context& ctx, int fd, const void* data, size_t size, uint64_t offset, Func&& func) {
  return detail::WriteAwaiter{ctx, fd, data, size, offset, std::forward<Func>(func)};
}

YACLIB_INLINE auto Write(Context& ctx, int fd, const void* data, size_t size, uint64_t offset) {
  return Write(ctx, fd, data, size, offset, [] {
  });
}

template <typename T>
auto Write(Context& ctx, int fd, std::vector<T> buffer, uint64_t offset) {
  static_assert(std::is_standard_layout_v<T>);

  auto data = buffer.data();
  auto size = buffer.size() * sizeof(T);
  return Write(ctx, fd, data, size, offset, [buf = std::move(buffer)]() mutable {
    buf = {};
  });
}

// TODO: WriteV

}  // namespace ione
