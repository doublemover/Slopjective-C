#pragma once

#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/dispatch/typed_dispatch_result.h"

#include <cstdint>

namespace objc3c::runtime {

RuntimeTypedDispatchResult InvokeIntCompatibleRuntimeMethodSignature(
    const void *implementation, RuntimeMethodReturnKind return_kind,
    std::uint64_t parameter_count, int a0, int a1, int a2, int a3);
RuntimeTypedDispatchResult InvokeBoolRuntimeMethodSignature(
    const void *implementation, RuntimeMethodReturnKind return_kind,
    std::uint64_t parameter_count, int a0, int a1, int a2, int a3);
RuntimeTypedDispatchResult InvokeVoidRuntimeMethodSignature(
    const void *implementation, RuntimeMethodReturnKind return_kind,
    std::uint64_t parameter_count, int a0, int a1, int a2, int a3);

}  // namespace objc3c::runtime
