#pragma once

#include "runtime/dispatch/method_cache.h"
#include "runtime/metadata/runtime_ownership_contracts.h"
#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/public/objc3_runtime_result.h"
#include "runtime/state/runtime_bootstrap_contracts.h"
#include "runtime/storage/runtime_instance_records.h"
#include "runtime/blocks/block_runtime_records.h"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace objc3c::runtime {

struct RuntimeState {
  std::mutex mutex;
  std::string owner_split_contract_id = kObjc3RuntimeOwnerSplitContractId;
  std::string metadata_model_owner = kObjc3RuntimeMetadataModelOwner;
  std::string registration_table_owner = kObjc3RuntimeRegistrationTableOwner;
  std::string manifest_descriptor_artifact_owner =
      kObjc3RuntimeManifestDescriptorArtifactOwner;
  std::string bootstrap_replay_owner = kObjc3RuntimeBootstrapReplayOwner;
  std::string dispatch_frame_state_owner =
      kObjc3RuntimeDispatchFrameStateOwner;
  std::string public_registration_api_owner =
      kObjc3RuntimePublicRegistrationApiOwner;
  std::string public_dispatch_diagnostics_owner =
      kObjc3RuntimePublicDispatchDiagnosticsOwner;
  std::string fail_closed_ownership_model =
      kObjc3RuntimeFailClosedOwnershipModel;
  bool runtime_owner_split_explicit = true;
  bool retired_route_path_allowed = false;
  std::uint64_t registered_image_count = 0;
  std::uint64_t registered_descriptor_total = 0;
  std::uint64_t next_expected_registration_order_ordinal = 1;
  std::uint64_t last_successful_registration_order_ordinal = 0;
  int last_registration_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  std::string last_registered_module_name;
  std::string last_registered_translation_unit_identity_key;
  std::string last_rejected_module_name;
  std::string last_rejected_translation_unit_identity_key;
  std::uint64_t last_rejected_registration_order_ordinal = 0;
  std::unordered_map<std::string, std::uint64_t>
      registration_order_by_identity_key;
  std::unordered_map<std::string, RegisteredImageMetadata>
      registered_image_metadata_by_identity_key;
  std::vector<std::string> retained_bootstrap_identity_order;
  std::unordered_map<std::string, RegisteredImageMetadata>
      retained_bootstrap_metadata_by_identity_key;
  std::unordered_map<std::string, std::size_t> selector_index_by_name;
  std::deque<SelectorSlot> selector_slots;
  std::unordered_map<std::uint64_t, KeyPathSlot> keypath_slots;
  std::unordered_map<PropertyLookupCacheKey, PropertyLookupCacheEntry,
                     PropertyLookupCacheKeyHash>
      property_lookup_cache;
  std::uint64_t metadata_backed_selector_count = 0;
  std::uint64_t dynamic_selector_count = 0;
  std::uint64_t metadata_provider_edge_count = 0;
  std::uint64_t image_backed_keypath_count = 0;
  std::uint64_t ambiguous_keypath_handle_count = 0;
  std::string last_materialized_selector;
  std::uint64_t last_materialized_stable_id = 0;
  std::uint64_t last_materialized_registration_order_ordinal = 0;
  std::uint64_t last_materialized_selector_pool_index = 0;
  bool last_materialized_from_metadata = false;
  std::uint64_t last_materialized_keypath_handle = 0;
  std::uint64_t last_materialized_keypath_registration_order_ordinal = 0;
  std::string last_materialized_keypath_profile;
  std::uint64_t last_queried_keypath_handle = 0;
  bool last_keypath_query_found = false;
  bool last_keypath_query_ambiguous = false;
  std::string last_resolved_keypath_profile;
  std::uint64_t malformed_class_metadata_rejection_count = 0;
  std::string last_malformed_class_graph_reason;
  std::unordered_map<MethodCacheKey, MethodCacheEntry, MethodCacheKeyHash>
      method_cache;
  std::uint64_t method_cache_hit_count = 0;
  std::uint64_t method_cache_miss_count = 0;
  std::uint64_t slow_path_lookup_count = 0;
  std::uint64_t stale_method_cache_entry_count = 0;
  std::uint64_t live_dispatch_count = 0;
  std::uint64_t strict_dispatch_error_count = 0;
  std::uint64_t fast_path_seed_count = 0;
  std::uint64_t fast_path_hit_count = 0;
  std::string last_dispatch_selector;
  std::uint64_t last_dispatch_selector_stable_id = 0;
  std::uint64_t last_dispatch_normalized_receiver_identity = 0;
  std::uint64_t last_category_probe_count = 0;
  std::uint64_t last_protocol_probe_count = 0;
  bool last_dispatch_used_cache = false;
  bool last_dispatch_used_fast_path = false;
  bool last_dispatch_resolved_live_method = false;
  bool last_dispatch_strict_error = false;
  bool last_dispatch_effective_direct_dispatch = false;
  bool last_dispatch_used_builtin = false;
  objc3_runtime_dispatch_status_code last_dispatch_status_code =
      OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR;
  std::string last_fast_path_reason;
  std::string last_dispatch_path;
  std::string last_dispatch_implementation_kind;
  std::string last_dispatch_return_kind;
  std::string last_dispatch_diagnostic_code;
  std::string last_dispatch_diagnostic_message;
  std::string last_dispatch_result_contract;
  std::string last_dispatch_property_name;
  std::string last_resolved_class_name;
  std::string last_resolved_owner_identity;
  std::uint64_t last_dispatch_parameter_count = 0;
  std::uint64_t last_dispatch_property_base_identity = 0;
  std::uint64_t last_dispatch_property_slot_index = 0;
  const objc3_runtime_registration_table *staged_registration_table = nullptr;
  std::uint64_t walked_image_count = 0;
  std::uint64_t last_discovery_root_entry_count = 0;
  std::uint64_t last_walked_class_descriptor_count = 0;
  std::uint64_t last_walked_protocol_descriptor_count = 0;
  std::uint64_t last_walked_category_descriptor_count = 0;
  std::uint64_t last_walked_property_descriptor_count = 0;
  std::uint64_t last_walked_ivar_descriptor_count = 0;
  std::uint64_t last_walked_selector_pool_count = 0;
  std::uint64_t last_walked_string_pool_count = 0;
  std::uint64_t last_walked_keypath_descriptor_count = 0;
  bool last_linker_anchor_matches_discovery_root = false;
  bool last_registration_used_staged_table = false;
  std::string last_walked_module_name;
  std::string last_walked_translation_unit_identity_key;
  std::uint64_t last_reset_cleared_image_local_init_state_count = 0;
  std::uint64_t last_replayed_image_count = 0;
  std::uint64_t reset_generation = 0;
  std::uint64_t replay_generation = 0;
  std::uint64_t class_graph_generation = 0;
  std::uint64_t category_attachment_generation = 0;
  std::uint64_t protocol_declaration_generation = 0;
  std::uint64_t storage_surface_generation = 0;
  std::uint64_t method_surface_generation = 0;
  int last_replay_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  std::string last_replayed_module_name;
  std::string last_replayed_translation_unit_identity_key;
  std::unordered_map<std::uint64_t, std::string>
      realized_class_name_by_base_identity;
  std::unordered_set<std::uint64_t> ambiguous_realized_base_identities;
  std::unordered_map<std::string, std::vector<std::size_t>>
      realized_class_node_indices_by_name;
  std::vector<RealizedClassNode> realized_class_nodes;
  std::uint64_t realized_root_class_count = 0;
  std::uint64_t realized_metaclass_edge_count = 0;
  std::uint64_t receiver_class_binding_count = 0;
  std::uint64_t realized_attached_category_count = 0;
  std::uint64_t realized_protocol_conformance_edge_count = 0;
  std::string last_realized_class_name;
  std::string last_realized_class_owner_identity;
  std::string last_realized_metaclass_owner_identity;
  std::string last_attached_category_owner_identity;
  std::string last_attached_category_name;
  std::string last_queried_class_name;
  std::string last_resolved_class_query_name;
  std::string last_resolved_class_query_owner_identity;
  bool last_class_query_found = false;
  std::string last_protocol_conformance_class_name;
  std::string last_protocol_conformance_protocol_name;
  std::string last_protocol_conformance_owner_identity;
  std::string last_protocol_conformance_attachment_owner_identity;
  std::string last_protocol_conformance_matched_class_name;
  std::string last_protocol_conformance_matched_class_owner_identity;
  std::uint64_t last_protocol_conformance_matched_protocol_depth = 0;
  bool last_protocol_conformance_matched_from_category = false;
  bool last_protocol_conformance_matched_from_superclass = false;
  bool last_protocol_conformance_matched_via_inherited_protocol = false;
  bool last_protocol_query_class_found = false;
  bool last_protocol_query_protocol_found = false;
  bool last_protocol_query_conforms = false;
  std::unordered_map<int, RuntimeInstanceRecord> runtime_instances_by_receiver;
  std::unordered_map<int, RuntimeBlockRecord> runtime_blocks_by_handle;
  std::unordered_map<RuntimeBlockByrefCell *,
                     std::weak_ptr<RuntimeBlockByrefCell>>
      runtime_block_byref_cells_by_heap_address;
  std::unordered_map<int, std::vector<RuntimeWeakSlotRef>>
      weak_slot_refs_by_target_receiver;
  int next_runtime_instance_receiver = 0x100000;
  int next_runtime_block_handle = 0x200000;
  std::uint64_t next_runtime_block_byref_cell_ordinal = 1;
  std::uint64_t next_runtime_instance_allocation_ordinal = 1;
  std::uint64_t live_runtime_instance_count = 0;
  std::uint64_t last_allocated_runtime_instance_receiver = 0;
  std::uint64_t last_allocated_runtime_instance_base_identity = 0;
  std::uint64_t last_allocated_runtime_instance_size_bytes = 0;
  std::uint64_t last_allocated_runtime_instance_allocation_ordinal = 0;
  std::string last_allocated_runtime_instance_class_name;
  std::string last_queried_property_class_name;
  std::string last_queried_property_name;
  std::string last_reflected_property_class_name;
  std::string last_reflected_property_owner_identity;
  bool last_property_query_found = false;
  bool last_property_query_inherited = false;
  bool last_property_query_used_cache = false;
  std::uint64_t property_lookup_cache_hit_count = 0;
  std::uint64_t property_lookup_cache_miss_count = 0;
};

}  // namespace objc3c::runtime
