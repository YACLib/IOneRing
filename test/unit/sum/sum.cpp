#include <ione/sum/sum.hpp>

#include <experimental/coroutine>
#include <iostream>

#include <gtest/gtest.h>

namespace {

struct suspend_always {                // NOLINT
  bool await_ready() const noexcept {  // NOLINT
    return false;
  }
  void await_suspend(std::experimental::coroutine_handle<>) const noexcept {  // NOLINT
  }
  void await_resume() const noexcept {  // NOLINT
  }
};

template <typename T>
struct Generator {
  struct promise_type {
    T current_value;
    using coro_handle = std::experimental::coroutine_handle<promise_type>;
    auto get_return_object() {
      return coro_handle::from_promise(*this);
    }
    auto initial_suspend() {
      return suspend_always();
    }
    auto final_suspend() noexcept {
      return suspend_always();
    }
    void return_void() {
    }
    void unhandled_exception() {
      std::terminate();
    }
    auto yield_value(T value) {
      current_value = value;
      return suspend_always();
    }
  };
  using coro_handle = std::experimental::coroutine_handle<promise_type>;  // NOLINT

  bool move_next() {  // NOLINT
    return _handle && (_handle.resume(), !_handle.done());
  }
  T current_value() const {  // NOLINT
    return _handle.promise().current_value;
  }

  Generator(coro_handle h) : _handle(h) {
  }
  Generator(const Generator&) = delete;
  Generator(Generator&& rhs) noexcept : _handle(rhs._handle) {
    rhs._handle = nullptr;
  }
  Generator& operator=(const Generator&) = delete;
  Generator& operator=(Generator&& other) = delete;

  ~Generator() {
    if (_handle) {
      _handle.destroy();
    }
  }

 private:
  coro_handle _handle;
};

TEST(Simple, JustWorks) {
  EXPECT_EQ(ione::Sum(2, 3), 5);
}

Generator<int> NaturalNums() {
  int num{};
  for (;;) {
    co_yield num;
    num++;
  }
}

TEST(Coroutine, JustWorks) {
  auto nums = NaturalNums();
  nums.move_next();
  auto a = nums.current_value();
  EXPECT_EQ(a, 0);
  nums.move_next();
  a = nums.current_value();
  EXPECT_EQ(a, 1);
}

}  // namespace
