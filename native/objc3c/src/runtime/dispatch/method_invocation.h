#pragma once

#include <cstdint>

#include "runtime/dispatch/typed_dispatch_result.h"

namespace objc3c::runtime {

RuntimeTypedDispatchResult InvokeRuntimeMethodImplementation(
    const void *implementation, RuntimeMethodReturnKind return_kind,
    std::uint64_t parameter_count, int a0, int a1, int a2, int a3,
    int *throws_error_out);

}  // namespace objc3c::runtime
