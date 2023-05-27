#include <yaclib/async/wait.hpp>
#include <yaclib/coro/task.hpp>
#include <yaclib/runtime/fair_thread_pool.hpp>

#include <ione/async/timeout.hpp>

#include <chrono>

#include <gtest/gtest.h>

namespace {

using namespace std::chrono_literals;

yaclib::Task<> Sleep(ione::Context& ctx, std::chrono::seconds sec) {
  co_await ione::SleepFor(ctx, sec, 0ns);
  std::cout << sec.count() << " seconds passed" << std::endl;
  co_return{};
}

TEST(Simple, JustWorks) {
  ione::Context ctx;
  ctx.Init(2, 0);
  yaclib::FairThreadPool tp(2);
  auto f1 = Sleep(ctx, 2s).ToFuture(tp);
  auto f2 = Sleep(ctx, 1s).ToFuture(tp);
  auto f3 = Sleep(ctx, 1s).ToFuture(tp);

  std::atomic_bool run{true};
  std::thread poller{[&] {
    while (run.load()) {
      ctx.Run();
    }
  }};

  yaclib::Wait(f1, f2, f3);
  run.store(false);
  tp.HardStop();
  tp.Wait();
  poller.join();
}

}  // namespace
