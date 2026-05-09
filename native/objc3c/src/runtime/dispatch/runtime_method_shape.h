#pragma once

#include <cstdint>

#include "runtime/public/objc3_runtime_result.h"

namespace objc3c::runtime {

objc3_runtime_dispatch_status_code RuntimeMethodShapeStatus(
    const char *return_type_name, std::uint64_t parameter_count);
bool IsSupportedRuntimeMethodShape(
    const char *return_type_name, std::uint64_t parameter_count);

}  // namespace objc3c::runtime
