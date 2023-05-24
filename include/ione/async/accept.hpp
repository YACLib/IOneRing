#pragma once

#include <ione/context.hpp>

namespace ione {
namespace detail {

class [[nodiscard]] AcceptAwaiter : private CompletionAwaiter {
 public:
  struct Result {
    sockaddr addr;
    socklen_t addrlen;
    int error_code;
  };

  AcceptAwaiter(Context& ctx, int fd, sockaddr addr, socklen_t addrlen, int flags) : _addr{addr}, _addrlen{addrlen} {
    SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_accept(sqe, fd, &_addr, &_addrlen, flags);
    });
  }

  Result await_resume() {
    return {_addr, _addrlen, res};
  }

 private:
  sockaddr _addr;
  socklen_t _addrlen;
};

class [[nodiscard]] DefaultAcceptAwaiter : private CompletionAwaiter {
 public:
  using Result = int;

  DefaultAcceptAwaiter(Context& ctx, int fd, int flags) {
    SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_accept(sqe, fd, nullptr, nullptr, flags);
    });
  }

  Result await_resume() {
    return res;
  }
};

}  // namespace detail

YACLIB_INLINE auto Accept(Context& ctx, int fd, sockaddr addr, socklen_t addrlen, int flags) {
  return detail::AcceptAwaiter{ctx, fd, addr, addrlen, flags};
}

YACLIB_INLINE auto Accept(Context& ctx, int fd, int flags) {
  return detail::DefaultAcceptAwaiter{ctx, fd, flags};
}

}  // namespace ione