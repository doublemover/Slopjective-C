#pragma once
#include <string>
#include <vector>

#if defined(__aarch64__)
#define OBJC3_TEST_NATIVE_LINUX_TARGET "aarch64-unknown-linux-gnu"
#else
#define OBJC3_TEST_NATIVE_LINUX_TARGET "x86_64-unknown-linux-gnu"
#endif

namespace objc3::clang_foundation::test {
inline std::vector<std::string> NativeToolArguments(std::vector<std::string> args) {
  args.push_back("--target=" OBJC3_TEST_NATIVE_LINUX_TARGET);
#if defined(__aarch64__)
  // Raw Tooling ASTs do not read the installed SDK's driver configuration.
  args.push_back("--rtlib=compiler-rt");
#endif
  return args;
}
}
