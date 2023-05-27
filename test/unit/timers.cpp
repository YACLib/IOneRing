#include <util/time.hpp>
#include <yaclib/async/wait.hpp>
#include <yaclib/coro/task.hpp>
#include <yaclib/runtime/fair_thread_pool.hpp>

#include <ione/async/timeout.hpp>

#include <chrono>

#include <gtest/gtest.h>

namespace {

using namespace std::chrono_literals;

template <class Rep, class Period>
yaclib::Task<> Sleep(ione::Context& ctx, const std::chrono::duration<Rep, Period>& sleep_duration) {
  co_await ione::SleepFor(ctx, sleep_duration);
  std::cout << std::chrono::duration_cast<std::chrono::seconds>(sleep_duration).count() << " seconds passed"
            << std::endl;
  co_return{};
}

TEST(Timers, SleepNonBlocking) {
  ione::Context ctx;
  ctx.Init(3, 0);
  yaclib::FairThreadPool tp(1);
  test::util::StopWatch watch;
  auto f1 = Sleep(ctx, 1s).ToFuture(tp);
  auto f2 = Sleep(ctx, 2s).ToFuture(tp);
  auto f3 = Sleep(ctx, 3s).ToFuture(tp);

  std::atomic_bool run{true};
  std::thread poller{[&] {
    while (run.load()) {
      ctx.Run();
    }
  }};

  yaclib::Wait(f1, f2, f3);
  EXPECT_EQ(std::chrono::duration_cast<std::chrono::seconds>(watch.Elapsed()).count(), 3);
  EXPECT_NO_THROW(std::ignore = std::move(f1).Get().Ok());
  EXPECT_NO_THROW(std::ignore = std::move(f2).Get().Ok());
  EXPECT_NO_THROW(std::ignore = std::move(f3).Get().Ok());
  run.store(false);
  tp.HardStop();
  tp.Wait();
  poller.join();
}

}  // namespace
