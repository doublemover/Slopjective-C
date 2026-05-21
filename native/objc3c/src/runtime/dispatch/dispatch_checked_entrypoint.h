#pragma once

#include "runtime/dispatch/dispatch_snapshot_contracts.h"
#include "runtime/public/objc3_runtime_dispatch_result.h"

namespace objc3c::runtime {

objc3_runtime_dispatch_i32_result ExecuteRuntimeDispatchI32Checked(
    int receiver, const char *selector, int a0, int a1, int a2, int a3);
objc3_runtime_dispatch_typed_result ExecuteRuntimeDispatchTypedChecked(
    int receiver, const char *selector, int a0, int a1, int a2, int a3);
objc3_runtime_dispatch_i32_result ExecuteRuntimeDispatchI32FromClassChecked(
    int receiver,
    const char *lookup_start_class_name,
    const char *selector,
    int a0,
    int a1,
    int a2,
    int a3);
objc3_runtime_dispatch_typed_result
ExecuteRuntimeDispatchTypedFromClassChecked(
    int receiver,
    const char *lookup_start_class_name,
    const char *selector,
    int a0,
    int a1,
    int a2,
    int a3);
objc3_runtime_dispatch_i32_result ExecuteRuntimeCacheAwareDispatchI32Checked(
    int receiver,
    const objc3_runtime_cache_aware_dispatch_descriptor *descriptor,
    int a0,
    int a1,
    int a2,
    int a3);

}  // namespace objc3c::runtime
