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

  void Submit();

  void Poll();

  ~Context();

 private:
  bool _initialized = false;
  io_uring _ring;
};

struct ReadResult {
  std::vector<char> buffer;
  int error_code;
};

struct ReadImpl : ReadResult {
  std::unique_ptr<iovec[]> iovecs; /* Referred by readv/writev */
};

struct CompletionAwaiter {
  yaclib_std::coroutine_handle<> handle;
  int res = 0;
};

struct ReadAwaiter : CompletionAwaiter {
  constexpr bool await_ready() const noexcept {
    return true;
  }

  void await_suspend(yaclib_std::coroutine_handle<> handle) {
    this->handle = handle;
  }

  ReadResult await_resume() {
    read_impl.error_code = res;
    return std::move(read_impl);
  }

  ReadImpl read_impl;
  io_uring_sqe* sqe;
};

ReadAwaiter Read(Context& ctx, int fd, std::uintmax_t file_size, std::size_t offset, std::size_t length);

ReadAwaiter Read(Context& ctx, const std::filesystem::path& file_path, std::size_t offset, std::size_t length);

}  // namespace ione
