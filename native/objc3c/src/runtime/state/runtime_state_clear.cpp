#include "runtime/state/runtime_state_clear.h"

namespace objc3c::runtime {

void ClearMethodCacheStateUnlocked(RuntimeState &state) {
  state.method_cache.clear();
  state.method_cache_hit_count = 0;
  state.method_cache_miss_count = 0;
  state.slow_path_lookup_count = 0;
  state.live_dispatch_count = 0;
  state.strict_dispatch_error_count = 0;
  state.fast_path_seed_count = 0;
  state.fast_path_hit_count = 0;
  state.last_dispatch_selector.clear();
  state.last_dispatch_selector_stable_id = 0;
  state.last_dispatch_normalized_receiver_identity = 0;
  state.last_category_probe_count = 0;
  state.last_protocol_probe_count = 0;
  state.last_dispatch_used_cache = false;
  state.last_dispatch_used_fast_path = false;
  state.last_dispatch_resolved_live_method = false;
  state.last_dispatch_strict_error = false;
  state.last_dispatch_effective_direct_dispatch = false;
  state.last_dispatch_used_builtin = false;
  state.last_dispatch_status_code =
      OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR;
  state.last_fast_path_reason.clear();
  state.last_dispatch_path.clear();
  state.last_dispatch_implementation_kind.clear();
  state.last_dispatch_return_kind.clear();
  state.last_dispatch_diagnostic_code.clear();
  state.last_dispatch_diagnostic_message.clear();
  state.last_dispatch_result_contract.clear();
  state.last_dispatch_property_name.clear();
  state.last_resolved_class_name.clear();
  state.last_resolved_owner_identity.clear();
  state.last_dispatch_parameter_count = 0;
  state.last_dispatch_property_base_identity = 0;
  state.last_dispatch_property_slot_index = 0;
}

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
  state.last_queried_class_name.clear();
  state.last_resolved_class_query_name.clear();
  state.last_resolved_class_query_owner_identity.clear();
  state.last_class_query_found = false;
  state.last_protocol_conformance_class_name.clear();
  state.last_protocol_conformance_protocol_name.clear();
  state.last_protocol_conformance_owner_identity.clear();
  state.last_protocol_conformance_attachment_owner_identity.clear();
  state.last_protocol_query_class_found = false;
  state.last_protocol_query_protocol_found = false;
  state.last_protocol_query_conforms = false;
  state.last_queried_property_class_name.clear();
  state.last_queried_property_name.clear();
  state.last_reflected_property_class_name.clear();
  state.last_reflected_property_owner_identity.clear();
  state.last_property_query_found = false;
  state.last_property_query_inherited = false;
  state.last_property_query_used_cache = false;
}

void ClearRuntimeInstanceStateUnlocked(RuntimeState &state) {
  state.runtime_instances_by_receiver.clear();
  state.runtime_blocks_by_handle.clear();
  state.weak_slot_refs_by_target_receiver.clear();
  state.next_runtime_instance_receiver = 0x100000;
  state.next_runtime_block_handle = 0x200000;
  state.live_runtime_instance_count = 0;
  state.last_allocated_runtime_instance_receiver = 0;
  state.last_allocated_runtime_instance_base_identity = 0;
  state.last_allocated_runtime_instance_size_bytes = 0;
  state.last_allocated_runtime_instance_class_name.clear();
}

}  // namespace objc3c::runtime
