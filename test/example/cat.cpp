#include <yaclib/algo/wait_group.hpp>
#include <yaclib/async/future.hpp>
#include <yaclib/coro/future.hpp>

#include <ione/async/read.hpp>

#include <thread>

#include <sys/ioctl.h>

uint64_t get_file_size(int fd) {
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

void output_to_console(char* buf, int len) {
  while (len--) {
    fputc(*buf++, stdout);
  }
}

yaclib::Future<> ReadFile(ione::Context& ctx, const char* file_path) {
  int file_fd = open(file_path, O_RDONLY);
  if (file_fd < 0) {
    throw std::runtime_error{"File open error"};
  }

  auto file_size = get_file_size(file_fd);
  if (file_size == -1) {
    throw std::runtime_error{"Incorrect file size"};
  }
  auto read_result = co_await ione::Read{ctx, file_fd, 0, file_size};
  if (read_result.error_code < 0) {
    throw std::runtime_error{"Govno in error_code"};
  }
  output_to_console(read_result.buffer.data(), read_result.buffer.size());
  co_return{};
}

int main(int file_num, char* argv[]) {
  if (file_num < 2) {
    fprintf(stderr, "Usage: %s [file name] <[file name] ...>\n", argv[0]);
    return 1;
  }

  ione::Context ctx;
  ctx.Init(1, 0);

  yaclib::WaitGroup wg{1};
  std::thread reader{[&] {
    for (int i = 1; i < file_num; ++i) {
      wg.Consume(ReadFile(ctx, argv[i]));
    }
    ctx.Submit();
  }};

  std::thread poller{[&]() mutable {
    for (int i = 1; i < file_num; ++i) {
      ctx.Poll();
    }
  }};

  reader.join();
  poller.join();
  wg.Done();
  wg.Wait();

  return 0;
}
