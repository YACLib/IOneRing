#include <yaclib/async/contract.hpp>

#include <ione/context.hpp>

#include <cstdlib>

namespace ione {

namespace fs = std::filesystem;

void Context::Destroy() {
  assert(_initialized);
  io_uring_queue_exit(&_ring);
  _initialized = false;
}

Context::~Context() {
  if (_initialized) {
    Destroy();
  }
}

void Context::Init(unsigned entries, unsigned flags, unsigned sq_thread_cpu, unsigned sq_thread_idle) {
  assert(!_initialized);
  assert(entries != 0);
  io_uring_params params{.flags = flags, .sq_thread_cpu = sq_thread_cpu, .sq_thread_idle = sq_thread_idle};
  io_uring_queue_init_params(entries, &_ring, &params);
  _initialized = true;
}

void Context::Init(unsigned entries, unsigned flags) {
  assert(!_initialized);
  assert(entries != 0);
  auto ret = io_uring_queue_init(entries, &_ring, flags);
  if (ret < 0) {
    throw std::runtime_error("queue_init error");
  }
  _initialized = true;
}

void Context::Run() {
  io_uring_submit_and_wait(&_ring, 0);
  Poll();
}

int Context::Submit() {
  return io_uring_submit(&_ring);
}

unsigned Context::Poll() {
  io_uring_cqe* cqe;
  unsigned head;
  unsigned counter = 0;
  // TODO(kononovk): think about io_uring_wait_cqe syscall
  io_uring_for_each_cqe(&_ring, head, cqe) {
    auto* awaiter = static_cast<detail::CompletionAwaiter*>(io_uring_cqe_get_data(cqe));
    awaiter->res = cqe->res;
    awaiter->handle.resume();
    ++counter;
  }
  io_uring_cq_advance(&_ring, counter);
  return counter;
}

}  // namespace ione
