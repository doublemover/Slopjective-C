#include "runtime/state/runtime_class_graph_state_clear.h"

#include "runtime/reflection/property_reflection_query_state.h"
#include "runtime/state/runtime_state_records.h"

namespace objc3c::runtime {

void ClearRealizedClassGraphUnlocked(RuntimeState &state) {
  state.realized_class_name_by_base_identity.clear();
  state.ambiguous_realized_base_identities.clear();
  state.realized_class_node_indices_by_name.clear();
  state.realized_class_nodes.clear();
  state.property_lookup_cache.clear();
  state.property_lookup_cache_hit_count = 0;
  state.property_lookup_cache_miss_count = 0;
  state.realized_root_class_count = 0;
  state.realized_metaclass_edge_count = 0;
  state.receiver_class_binding_count = 0;
  state.realized_attached_category_count = 0;
  state.realized_protocol_conformance_edge_count = 0;
  state.last_realized_class_name.clear();
  state.last_realized_class_owner_identity.clear();
  state.last_realized_metaclass_owner_identity.clear();
  state.last_attached_category_owner_identity.clear();
  state.last_attached_category_name.clear();
  state.last_malformed_class_graph_metadata_surface.clear();
  state.last_malformed_class_graph_target_kind.clear();
  state.last_malformed_class_graph_visibility_state.clear();
  state.last_malformed_class_graph_availability_state.clear();
  state.last_queried_class_name.clear();
  state.last_resolved_class_query_name.clear();
  state.last_resolved_class_query_owner_identity.clear();
  state.last_class_query_found = false;
  state.last_protocol_conformance_class_name.clear();
  state.last_protocol_conformance_protocol_name.clear();
  state.last_protocol_conformance_owner_identity.clear();
  state.last_protocol_conformance_attachment_owner_identity.clear();
  state.last_protocol_conformance_matched_class_name.clear();
  state.last_protocol_conformance_matched_class_owner_identity.clear();
  state.last_protocol_conformance_failure_reason.clear();
  state.last_protocol_conformance_matched_protocol_depth = 0;
  state.last_protocol_conformance_matched_from_category = false;
  state.last_protocol_conformance_matched_from_superclass = false;
  state.last_protocol_conformance_matched_via_inherited_protocol = false;
  state.last_protocol_query_class_found = false;
  state.last_protocol_query_protocol_found = false;
  state.last_protocol_query_conforms = false;
  state.last_protocol_query_malformed_metadata = false;
  ResetRuntimePropertyReflectionQueryStateUnlocked(state);
}

}  // namespace objc3c::runtime
