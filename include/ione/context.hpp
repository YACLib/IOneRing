#pragma once

#include <yaclib/async/future.hpp>

#include <cassert>
#include <filesystem>
#include <vector>

#include <liburing.h>

namespace ione {

class Context {
 public:
  Context() = default;

  void Init(unsigned entries, unsigned flags);

  void Init(unsigned entries, io_uring_params& params);

  io_uring& GetRing() {
    assert(_initialized);
    return _ring;
  }

  void Destroy();

  int Submit();

  unsigned Poll();

  ~Context();

 private:
  bool _initialized = false;
  io_uring _ring;
};

namespace detail {

struct CompletionAwaiter {
  union {
    io_uring_sqe* sqe;
    yaclib_std::coroutine_handle<> handle;
  };
  int res = 0;

  void SetFlags(unsigned flags) {
    assert(sqe != nullptr);
    sqe->flags = flags;
  }

  void AddFlags(unsigned flags) {
    assert(sqe != nullptr);
    sqe->flags |= flags;
  }

  void Discard() && {
    AddFlags(IOSQE_CQE_SKIP_SUCCESS);
    handle = {};
  }

  constexpr bool await_ready() const noexcept {
    return false;
  }

  void await_suspend(yaclib_std::coroutine_handle<> handle) {
    this->handle = handle;
  }

 protected:
  CompletionAwaiter() : sqe{} {
  }

  ~CompletionAwaiter() {
    handle.~coroutine_handle<>();
  }

  template <typename Func>
  void SetUp(Context& ctx, Func&& func) {
    auto* ring = &ctx.GetRing();
    sqe = io_uring_get_sqe(ring);
    if (sqe == nullptr) {
      ctx.Submit();
      sqe = io_uring_get_sqe(ring);
      assert(sqe != nullptr);
    }
    func(sqe);
    io_uring_sqe_set_data(sqe, this);
  }
};

template <typename Func>
struct ScopeAwaiter : CompletionAwaiter {
 protected:
  ScopeAwaiter(Func func) : _func{std::move(func)} {
  }

  // TODO(kononovk): make func union T and call ~T in await_resume
  YACLIB_NO_UNIQUE_ADDRESS Func _func;
};

}  // namespace detail

}  // namespace ione
