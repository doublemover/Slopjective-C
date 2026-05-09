#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdint>

namespace objc3c::runtime {

enum class DispatchFamily;
struct EmittedMethodListRef;
struct RealizedClassNode;
struct RuntimeState;
struct SlowPathResolution;

bool TryResolveMethodFromMethodListRefUnlocked(
    RuntimeState &state,
    const EmittedMethodListRef *method_list_ref,
    const char *resolved_class_name,
    DispatchFamily family,
    std::uint64_t normalized_receiver_identity,
    std::uint64_t selector_stable_id,
    const char *selector_spelling,
    SlowPathResolution &resolution,
    bool &ambiguous);
bool ProbeProtocolSelectorDeclarationsFromAggregateUnlocked(
    RuntimeState &state,
    const objc3_runtime_pointer_aggregate *protocol_refs,
    DispatchFamily family,
    std::uint64_t selector_stable_id,
    const char *selector_spelling,
    std::uint64_t &protocol_probe_count,
    bool &ambiguous);
bool TryResolveMethodFromAttachedCategoriesUnlocked(
    RuntimeState &state,
    const RealizedClassNode &node,
    DispatchFamily family,
    std::uint64_t normalized_receiver_identity,
    std::uint64_t selector_stable_id,
    const char *selector_spelling,
    SlowPathResolution &resolution,
    bool &ambiguous,
    std::uint64_t &category_probe_count,
    std::uint64_t &protocol_probe_count);

}  // namespace objc3c::runtime
