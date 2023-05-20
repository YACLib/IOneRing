#include <yaclib/async/contract.hpp>

#include <ione/context/context.hpp>

#include <cstdlib>

namespace ione {

namespace fs = std::filesystem;

void Context::Destroy() {
  assert(!_initialized);
  io_uring_queue_exit(&_ring);
  _initialized = false;
}

Context::~Context() {
  if (_initialized) {
    Destroy();
  }
}

void Context::Init(unsigned entries, io_uring_params& params) {
  assert(!_initialized);
  assert(entries != 0);
  io_uring_queue_init_params(entries, &_ring, &params);
  _initialized = true;
}

void Context::Init(unsigned entries, unsigned flags) {
  assert(!_initialized);
  assert(entries != 0);
  io_uring_queue_init(entries, &_ring, flags);
  _initialized = true;
}

void Context::Submit() {
  io_uring_submit(&_ring);
}

void Context::Poll() {
  io_uring_cqe* cqe = nullptr;
  int ret = io_uring_wait_cqe(&_ring, &cqe);
  if (ret < 0) {
    assert(false);
  }
  while (cqe != nullptr) {
    auto* awaiter = static_cast<CompletionAwaiter*>(io_uring_cqe_get_data(cqe));
    awaiter->res = cqe->res;
    io_uring_cq_advance(&_ring, 1);
    awaiter->handle.resume();
    io_uring_peek_cqe(&_ring, &cqe);
  }
}

ReadAwaiter Read(Context& ctx, int fd, std::uintmax_t file_size, std::size_t offset, std::size_t length) {
  ReadAwaiter awaiter;

  static constexpr std::size_t kBlockSize = 1024;
  std::size_t current_block = 0;

  const auto blocks = (length + kBlockSize - 1) / kBlockSize;
  awaiter.read_impl.buffer.resize(length);
  awaiter.read_impl.iovecs = std::make_unique<iovec[]>(blocks);

  std::size_t offset_in_buffer = 0;
  while (length) {
    auto bytes_to_read = length;
    if (bytes_to_read > kBlockSize) {
      bytes_to_read = kBlockSize;
    }
    void* buf = awaiter.read_impl.buffer.data() + offset_in_buffer;
    awaiter.read_impl.iovecs[current_block].iov_base = buf;
    awaiter.read_impl.iovecs[current_block].iov_len = bytes_to_read;

    current_block++;
    offset_in_buffer += bytes_to_read;
    length -= bytes_to_read;
  }

  // Get an SQE
  auto* ring = &ctx.GetRing();
  awaiter.sqe = io_uring_get_sqe(ring);
  if (awaiter.sqe == nullptr) {
    ctx.Submit();
    awaiter.sqe = io_uring_get_sqe(ring);
    assert(awaiter.sqe != nullptr);
  }

  /* Set up a readv2 operation */
  io_uring_prep_readv2(awaiter.sqe, fd, awaiter.read_impl.iovecs.get(), blocks, offset, 0);
  io_uring_sqe_set_data(awaiter.sqe, &awaiter);
  return awaiter;
}

ReadAwaiter Read(Context& ctx, const std::filesystem::path& file_path, std::size_t offset, std::size_t length) {
  assert(fs::is_regular_file(file_path));

  auto fd = open(file_path.c_str(), O_RDONLY);  // NOLINT
  if (fd < 0) {
    assert(false);  // TODO
  }

  std::error_code ec;
  auto file_size = fs::file_size(file_path, ec);
  if (file_size == static_cast<std::uintmax_t>(-1)) {
    (void)ec;
    assert(false);
  }

  return Read(ctx, fd, file_size, offset, length);
}

}  // namespace ione
