#pragma once

#include <cstddef>

// Strict dispatch entrypoint ABI constants own the fixed-slot canonical
// runtime dispatch symbol and argument slot limits.
inline constexpr std::size_t kObjc3RuntimeDispatchDefaultArgs = 4;
inline constexpr std::size_t kObjc3RuntimeDispatchMaxArgs = 16;
inline constexpr const char *kObjc3RuntimeDispatchSymbol =
    "objc3_runtime_dispatch_i32";
