#pragma once

#include "runtime/dispatch/dispatch_family.h"
#include "runtime/dispatch/dispatch_target_resolution.h"
#include "runtime/dispatch/method_cache.h"
#include "runtime/public/objc3_runtime_selector.h"

#include <cstdint>

namespace objc3c::runtime {

struct MethodCacheEntry;
struct RuntimeState;

RuntimeDispatchTarget ResolveMethodCacheHitUnlocked(
    RuntimeState &state, const MethodCacheKey &cache_key,
    const MethodCacheEntry &entry, std::uint64_t receiver_base_identity,
    std::uint64_t expected_normalized_receiver_identity,
    std::uint64_t expected_selector_stable_id);

RuntimeDispatchTarget ResolveMethodCacheMissUnlocked(
    RuntimeState &state, std::uint64_t base_identity,
    std::uint64_t normalized_receiver_identity, DispatchFamily family,
    const objc3_runtime_selector_handle &selector_handle,
    std::uint64_t receiver_base_identity, const MethodCacheKey &cache_key);

}  // namespace objc3c::runtime
