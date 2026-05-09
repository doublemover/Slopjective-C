#pragma once

namespace objc3c::runtime {

struct RealizedClassNode;
struct RealizedPropertyAccessor;
struct RuntimeState;

void ResetRuntimePropertyReflectionQueryStateUnlocked(RuntimeState &state);
void BeginRuntimePropertyReflectionQueryUnlocked(RuntimeState &state,
                                                 const char *class_name,
                                                 const char *property_name);
void RecordRuntimePropertyReflectionCacheUseUnlocked(RuntimeState &state,
                                                     bool used_cache);
void RecordRuntimePropertyReflectionHitUnlocked(
    RuntimeState &state,
    const RealizedClassNode &resolved_node,
    const RealizedPropertyAccessor &accessor,
    bool inherited);

}  // namespace objc3c::runtime
