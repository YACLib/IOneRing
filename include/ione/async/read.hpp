#pragma once

#include <ione/context.hpp>

#include <span>

namespace ione {
namespace detail {

class [[nodiscard]] ReadAwaiter : public CompletionAwaiter {
 public:
  using Result = int;

  ReadAwaiter(Context& ctx, int fd, void* data, size_t size, uint64_t offset) {
    this->SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_read(sqe, fd, data, size, offset);
    });
  }

  Result await_resume() {
    return this->res;
  }
};

class [[nodiscard]] ReadToBufferAwaiter : public CompletionAwaiter {
 public:
  struct Result {
    std::vector<char> buffer;
    int error_code;
  };

  ReadToBufferAwaiter(Context& ctx, int fd, size_t size, uint64_t offset) : _buffer(size) {
    this->SetUp(ctx, [&](io_uring_sqe* sqe) {
      io_uring_prep_read(sqe, fd, _buffer.data(), _buffer.size(), offset);
    });
  }

  Result await_resume() {
    return {std::move(_buffer), res};
  }

 private:
  std::vector<char> _buffer;
};

}  // namespace detail

YACLIB_INLINE auto Read(Context& ctx, int fd, void* data, size_t size, uint64_t offset) {
  return detail::ReadAwaiter{ctx, fd, data, size, offset};
}

YACLIB_INLINE auto Read(Context& ctx, int fd, size_t size, uint64_t offset) {
  return detail::ReadToBufferAwaiter{ctx, fd, size, offset};
}

// TODO: ReadV

}  // namespace ione