#include <ione/config.hpp>

#include <iostream>
#include <random>
#include <thread>

#include <gtest/gtest.h>

namespace test {
namespace {

#define IONE_STRINGIZE(X) IONE_DO_STRINGIZE(X)
#define IONE_DO_STRINGIZE(X) #X

#if defined(__clang_version__)
#  define IONE_COMPILER "clang : " __clang_version__
#elif defined(_MSC_FULL_VER) && defined(_MSC_VER)
#  define IONE_COMPILER "ms visual c++ : " IONE_STRINGIZE(_MSC_FULL_VER) " or " IONE_STRINGIZE(_MSC_VER)
#elif defined(_MSC_FULL_VER)
#  define IONE_COMPILER "ms visual c++ : " IONE_STRINGIZE(_MSC_FULL_VER)
#elif defined(_MSC_VER)
#  define IONE_COMPILER "ms visual c++ : " IONE_STRINGIZE(_MSC_VER)
#elif defined(__VERSION__)
#  define IONE_COMPILER "gnu c++ : " __VERSION__
#else
#  define IONE_COMPILER "unknown"
#endif

#if defined(__GLIBCXX__)
#  define IONE_STDLIB "gnu libstdc++ : " IONE_STRINGIZE(__GLIBCXX__)
#elif defined(__GLIBCPP__)
#  define IONE_STDLIB "gnu libstdc++ : " IONE_STRINGIZE(__GLIBCPP__)
#elif defined(_LIBCPP_VERSION)
#  define IONE_STDLIB "libc++ : " IONE_STRINGIZE(_LIBCPP_VERSION)
#else
#  define IONE_STDLIB "unknown"
#endif

#if defined(_WIN64)
#  define IONE_PLATFORM "windows 64-bit"
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#  define IONE_PLATFORM "windows 32-bit"
#elif defined(__APPLE__)
#  define IONE_PLATFORM "apple"
#elif defined(__ANDROID__)
#  define IONE_PLATFORM "android"
#elif defined(__linux__)
#  define IONE_PLATFORM "linux"
#elif defined(__unix__)
#  define IONE_PLATFORM "unix"
#elif defined(_POSIX_VERSION)
#  define IONE_PLATFORM "posix"
#else
#  define IONE_PLATFORM "unknown"
#endif

void PrintEnvInfo() {
  std::cerr << "compiler: " << IONE_COMPILER << "\nstdlib: " << IONE_STDLIB "\nplatform: " << IONE_PLATFORM
            << std::endl;
}

}  // namespace
}  // namespace test

int main(int argc, char** argv) {
  test::PrintEnvInfo();
  testing::InitGoogleTest(&argc, argv);
  int result = 0;
  result = RUN_ALL_TESTS();
  return result;
}
