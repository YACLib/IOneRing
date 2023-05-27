#include <yaclib/coro/task.hpp>

#include <ione/async/read.hpp>
#include <ione/async/write.hpp>

#include <cstdio>
#include <thread>

#include <sys/ioctl.h>
#include <unistd.h>

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

}  // namespace

yaclib::Task<> CopyFile(ione::Context& ctx, int src_fd, int dst_fd, uint64_t len) {
  constexpr size_t kSize = 1024;
  char buffer[kSize];
  size_t offset = 0;
  size_t write_offset = 0;
  while (len != 0) {
    auto res = co_await Read(ctx, src_fd, buffer, std::min(len, kSize), offset);
    if (res < 0) {
      throw std::runtime_error{"Error during reading"};
    }
    len -= res;
    offset += res;

    while (res != 0) {
      auto written_len = co_await Write(ctx, dst_fd, buffer, res, write_offset);
      res -= written_len;
      write_offset += written_len;
    }
  }
  co_return{};
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::fprintf(stderr, "Usage: %s [file name] <[file name] ...>\n", argv[0]);
    return 1;
  }
  auto infd = open(argv[1], O_RDONLY);
  if (infd < 0) {
    perror("open infile");
    return 1;
  }
  auto outfd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (outfd < 0) {
    perror("open outfile");
    return 1;
  }
  ione::Context ctx;
  static constexpr int kEntriesNum = 2;
  static constexpr int kFlags = 0;
  ctx.Init(kEntriesNum, kFlags);

  auto insize = GetFileSize(infd);
  if (insize == -1) {
    throw std::runtime_error{"Incorrect file size"};
  }

  auto cp_future = CopyFile(ctx, infd, outfd, insize).ToFuture();

  std::atomic_bool run{true};
  std::thread poller{[&] {
    while (run.load()) {
      ctx.Run();
    }
  }};

  try {
    std::ignore = std::move(cp_future).Get().Ok();
  } catch (const std::exception& err) {
    perror(err.what());
    return 1;
  }

  run.store(false);
  poller.join();

  close(infd);
  close(outfd);
  return 0;
}
