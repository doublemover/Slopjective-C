#pragma once

#include <cstddef>
#include <cstdint>

namespace objc3c::pipeline {

inline constexpr std::size_t kRuntimeDispatchDefaultArgs = 4;
inline constexpr std::size_t kRuntimeDispatchMaxArgs = 16;
inline constexpr const char *kRuntimeDispatchDefaultSymbol =
    "objc3_runtime_dispatch_i32";
inline constexpr std::uint8_t kFrontendDefaultLanguageVersion = 3u;

enum class LanguageProfile : std::uint8_t {
  Canonical = 0,
};

}  // namespace objc3c::pipeline
