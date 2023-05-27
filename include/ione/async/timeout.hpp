#pragma once

#include <ione/context.hpp>

#include <chrono>
#include <iostream>
#include <span>

namespace ione {
namespace detail {

class [[nodiscard]] TimeoutAwaiter : public CompletionAwaiter {
 public:
  using Result = int;

  TimeoutAwaiter(Context& ctx, __kernel_timespec time, unsigned flags) : _time{time} {
    this->SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_timeout(sqe, &_time, 1, flags);
    });
  }

  Result await_resume() {
    return this->res;
  }

 private:
  __kernel_timespec _time;
};

template <class Rep, class Period>
__kernel_timespec DurationToTimespec(const std::chrono::duration<Rep, Period>& duration) {
  auto sec_duration = std::chrono::duration_cast<std::chrono::seconds>(duration);
  auto sec = sec_duration.count();
  static_assert(std::is_integral_v<decltype(sec)>);
  auto nsec_duration = duration - sec_duration;
  auto nsec = nsec_duration.count();
  return {sec, nsec};
}

}  // namespace detail

template <class Rep, class Period>
YACLIB_INLINE auto SleepFor(Context& ctx, const std::chrono::duration<Rep, Period>& sleep_duration) {
  return detail::TimeoutAwaiter{ctx, detail::DurationToTimespec(sleep_duration), 0};
}

}  // namespace ione
