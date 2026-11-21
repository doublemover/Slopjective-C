#pragma once
#include <string>
#include <string_view>
#include <vector>

#ifndef OBJC3_TEST_NATIVE_LINUX_TARGET
#if defined(__aarch64__)
#define OBJC3_TEST_NATIVE_LINUX_TARGET "aarch64-unknown-linux-gnu"
#else
#define OBJC3_TEST_NATIVE_LINUX_TARGET "x86_64-unknown-linux-gnu"
#endif
#endif

namespace objc3::clang_foundation::test {
inline constexpr bool NativeAArch64 =
    std::string_view(OBJC3_TEST_NATIVE_LINUX_TARGET).starts_with("aarch64-");
inline constexpr const char *NativeFeatureEnabled = NativeAArch64 ? "-march=armv8-a+crc" : "-mavx2";
inline constexpr const char *NativeFeatureDisabled = NativeAArch64 ? "-march=armv8-a+nocrc" : "-mno-avx2";

inline std::vector<std::string> NativeToolArguments(std::vector<std::string> args) {
  args.push_back("--target=" OBJC3_TEST_NATIVE_LINUX_TARGET);
  // Raw Tooling ASTs do not read the installed SDK's driver configuration.
  if (NativeAArch64) args.push_back("--rtlib=compiler-rt");
  return args;
}
}
