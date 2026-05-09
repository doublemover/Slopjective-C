#include "runtime/reflection/property_reflection_query_state.h"

#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_state_records.h"

namespace objc3c::runtime {

void ResetRuntimePropertyReflectionQueryStateUnlocked(RuntimeState &state) {
  state.last_queried_property_class_name.clear();
  state.last_queried_property_name.clear();
  state.last_reflected_property_class_name.clear();
  state.last_reflected_property_owner_identity.clear();
  state.last_property_query_found = false;
  state.last_property_query_inherited = false;
  state.last_property_query_used_cache = false;
}

void BeginRuntimePropertyReflectionQueryUnlocked(RuntimeState &state,
                                                 const char *class_name,
                                                 const char *property_name) {
  ResetRuntimePropertyReflectionQueryStateUnlocked(state);
  state.last_queried_property_class_name =
      class_name != nullptr ? class_name : "";
  state.last_queried_property_name =
      property_name != nullptr ? property_name : "";
}

void RecordRuntimePropertyReflectionCacheUseUnlocked(RuntimeState &state,
                                                     bool used_cache) {
  state.last_property_query_used_cache = used_cache;
}

void RecordRuntimePropertyReflectionHitUnlocked(
    RuntimeState &state,
    const RealizedClassNode &resolved_node,
    const RealizedPropertyAccessor &accessor,
    bool inherited) {
  state.last_property_query_found = true;
  state.last_property_query_inherited = inherited;
  state.last_reflected_property_class_name = resolved_node.class_name;
  state.last_reflected_property_owner_identity =
      accessor.property_descriptor != nullptr &&
              accessor.property_descriptor->declaration_owner_identity != nullptr
          ? accessor.property_descriptor->declaration_owner_identity
          : "";
}

}  // namespace objc3c::runtime
