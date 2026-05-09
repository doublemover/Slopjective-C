#pragma once

#include "runtime/dispatch/dispatch_target_resolution.h"
#include "runtime/public/objc3_runtime_result.h"
#include "runtime/public/objc3_runtime_selector.h"

#include <cstdint>

namespace objc3c::runtime {

struct MethodCacheEntry;
struct RuntimeState;
struct SlowPathResolution;

void ResetRuntimeDispatchStateUnlocked(
    RuntimeState &state, const char *selector,
    const objc3_runtime_selector_handle *selector_handle,
    objc3_runtime_dispatch_status_code initial_status);
void PublishMethodCacheEntryStateUnlocked(
    RuntimeState &state, const MethodCacheEntry &entry,
    std::uint64_t receiver_base_identity);
void PublishSlowPathResolutionStateUnlocked(
    RuntimeState &state, const SlowPathResolution &resolution,
    std::uint64_t receiver_base_identity);
RuntimeDispatchTarget PublishStrictDispatchErrorTargetUnlocked(
    RuntimeState &state, objc3_runtime_dispatch_status_code status_code,
    const char *dispatch_path);

}  // namespace objc3c::runtime
