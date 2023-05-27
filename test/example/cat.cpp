#include <yaclib/algo/wait_group.hpp>
#include <yaclib/coro/await.hpp>
#include <yaclib/coro/current_executor.hpp>
#include <yaclib/coro/on.hpp>
#include <yaclib/coro/task.hpp>
#include <yaclib/runtime/fair_thread_pool.hpp>

#include <ione/async/read.hpp>

#include <algorithm>
#include <thread>

#include <sys/ioctl.h>

namespace {

uint64_t GetFileSize(int fd) {
  struct stat st;

  if (fstat(fd, &st) < 0) {
    perror("fstat");
    return -1;
  }

  if (S_ISBLK(st.st_mode)) {
    unsigned long long bytes;
    if (ioctl(fd, BLKGETSIZE64, &bytes) != 0) {
      perror("ioctl");
      return -1;
    }
    return bytes;
  } else if (S_ISREG(st.st_mode))
    return st.st_size;

  return -1;
}

void OutputToConsole(const char* buf, size_t len) {
  while (len--) {
    fputc(*buf++, stdout);
  }
}

}  // namespace

yaclib::Task<> ReadFile(ione::Context& ctx, const char* file_path) {
  int file_fd = open(file_path, O_RDONLY);
  if (file_fd < 0) {
    throw std::runtime_error{"File open error"};
  }

  auto file_size = GetFileSize(file_fd);
  if (file_size == -1) {
    throw std::runtime_error{"Incorrect file size"};
  }
  constexpr size_t kSize = 100;
  char buffer[kSize];
  auto& current = co_await yaclib::CurrentExecutor();
  size_t offset = 0;
  while (file_size != 0) {
    auto res = co_await Read(ctx, file_fd, buffer, std::min(file_size, kSize), offset);
    if (res < 0) {
      throw std::runtime_error{"Error during reading"};
    }
    // fprintf(stderr, "file_path %s, res: %d\n", file_path, res);
    OutputToConsole(buffer, res);
    co_await On(current);
    file_size -= res;
    offset += res;
  }
  co_return{};
}

int main(int file_num, char* argv[]) {
  if (file_num < 2) {
    std::fprintf(stderr, "Usage: %s [file name] <[file name] ...>\n", argv[0]);
    return 1;
  }

  ione::Context ctx;
  ctx.Init(10, 0);

  yaclib::FairThreadPool tp1{1};
  auto coro = [&]() -> yaclib::Task<> {
    for (int i = 1; i < file_num; ++i) {
      co_await ReadFile(ctx, argv[i]);
    }
    co_return{};
  };
  auto f = coro().ToFuture(tp1);

  std::atomic_bool run{true};
  std::thread poller{[&] {
    while (run.load()) {
      ctx.Run();
    }
  }};

  Wait(f);

  run.store(false);
  poller.join();

  tp1.HardStop();
  tp1.Wait();

  return 0;
}
