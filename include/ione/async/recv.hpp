#pragma once

#include <ione/context.hpp>

#include <span>

namespace ione {
namespace detail {

class [[nodiscard]] RecvAwaiter : public CompletionAwaiter {
 public:
  using Result = int;

  RecvAwaiter(Context& ctx, int fd, void* data, size_t size, int flags) {
    this->SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_recv(sqe, fd, data, size, flags);
    });
  }

  Result await_resume() {
    return this->res;
  }
};

class [[nodiscard]] RecvToBufferAwaiter : public CompletionAwaiter {
 public:
  struct Result {
    std::vector<char> buffer;
    int error_code;
  };

  RecvToBufferAwaiter(Context& ctx, int fd, size_t size, int flags) : _buffer(size) {
    this->SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_recv(sqe, fd, _buffer.data(), _buffer.size(), flags);
    });
  }

  Result await_resume() {
    return {std::move(_buffer), res};
  }

 private:
  std::vector<char> _buffer;
};

}  // namespace detail

template <typename Func>
YACLIB_INLINE auto Recv(Context& ctx, int fd, void* data, size_t size, int flags) {
  return detail::RecvAwaiter{ctx, fd, data, size, flags};
}

YACLIB_INLINE auto Recv(Context& ctx, int fd, size_t size, int flags) {
  return detail::RecvToBufferAwaiter{ctx, fd, size, flags};
}

}  // namespace ione