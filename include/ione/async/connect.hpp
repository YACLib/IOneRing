#pragma once

#include <ione/context.hpp>

namespace ione {
namespace detail {

class [[nodiscard]] ConnectAwaiter : private CompletionAwaiter {
 public:
  using Result = int;

  ConnectAwaiter(Context& ctx, int fd, sockaddr addr, socklen_t addrlen) : _addr{addr} {
    SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_connect(sqe, fd, &_addr, addrlen);
    });
  }

  Result await_resume() {
    return res;
  }

 private:
  sockaddr _addr;
};

}  // namespace detail

YACLIB_INLINE auto Connect(Context& ctx, int fd, sockaddr addr, socklen_t addrlen) {
  return detail::ConnectAwaiter{ctx, fd, addr, addrlen};
}

}  // namespace ione