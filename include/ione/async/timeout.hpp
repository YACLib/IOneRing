#pragma once

#include <ione/context.hpp>

#include <chrono>
#include <span>

namespace ione {
namespace detail {

class [[nodiscard]] TimeoutAwaiter : public CompletionAwaiter {
 public:
  using Result = int;

  TimeoutAwaiter(Context& ctx, int64_t seconds, int64_t nanoseconds, unsigned count, unsigned flags)
    : _time{seconds, nanoseconds} {
    this->SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_timeout(sqe, &_time, count, flags | IORING_TIMEOUT_ETIME_SUCCESS);
    });
  }

  Result await_resume() {
    return this->res;
  }

 private:
  __kernel_timespec _time;
};

}  // namespace detail

YACLIB_INLINE auto Timer(Context& ctx, std::chrono::seconds sec, std::chrono::nanoseconds nanosec, unsigned count,
                         unsigned flags) {
  return detail::TimeoutAwaiter{ctx, sec.count(), nanosec.count(), count, flags};
}

YACLIB_INLINE auto SleepFor(Context& ctx, std::chrono::seconds sec, std::chrono::nanoseconds nanosec) {
  return Timer(ctx, sec, nanosec, 1, 0);
}

}  // namespace ione
