#pragma once

#include <cstdint>

#include "runtime/public/objc3_runtime_result.h"

namespace objc3c::runtime {

enum class DispatchFamily {
  Invalid = 0,
  Instance = 1,
  Class = 2,
};

enum class RuntimeMethodReturnKind {
  Unsupported = 0,
  I32Like = 1,
  Bool = 2,
  Void = 3,
};

RuntimeMethodReturnKind ClassifyRuntimeReturnType(
    const char *return_type_name);
objc3_runtime_dispatch_status_code RuntimeMethodShapeStatus(
    const char *return_type_name, std::uint64_t parameter_count);
bool IsSupportedRuntimeMethodShape(
    const char *return_type_name, std::uint64_t parameter_count);

}  // namespace objc3c::runtime
