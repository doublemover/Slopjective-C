#include "runtime/images/registration.h"

#include "runtime/classes/class_graph.h"
#include "runtime/classes/protocol_conformance.h"
#include "runtime/dispatch/method_fast_path_seed.h"
#include "runtime/images/image_descriptor.h"
#include "runtime/images/registration_snapshots.h"
#include "runtime/images/registration_state_publish.h"
#include "runtime/images/registration_table_walk.h"
#include "runtime/metadata/runtime_ownership_contracts.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/state/runtime_cache_invalidation.h"
#include "runtime/state/runtime_state_clear.h"
#include "runtime/state/runtime_state_records.h"

#include <cstdint>
#include <deque>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace objc3c::runtime {

namespace {

bool RuntimeCStringMatchesString(const char *value, const std::string &text) {
  return value != nullptr && text == value;
}

void RebindRestoredRealizedClassNodeImagesUnlocked(RuntimeState &state) {
  for (RealizedClassNode &node : state.realized_class_nodes) {
    const auto image_it = state.registered_image_metadata_by_identity_key.find(
        node.translation_unit_identity_key);
    node.image = image_it != state.registered_image_metadata_by_identity_key.end()
                     ? &image_it->second
                     : nullptr;
  }
}

const RealizedPropertyAccessor *FindRestoredRuntimePropertyAccessorUnlocked(
    const RuntimeState &state, const MethodCacheEntry &entry) {
  for (const RealizedClassNode &node : state.realized_class_nodes) {
    if (node.class_name != entry.class_name) {
      continue;
    }
    for (const RealizedPropertyAccessor &accessor :
         node.runtime_property_accessors) {
      if (accessor.property_descriptor == nullptr) {
        continue;
      }
      const EmittedPropertyDescriptor &descriptor =
          *accessor.property_descriptor;
      if (entry.parameter_count == 0 &&
          entry.return_kind == accessor.getter_return_kind &&
          entry.owner_identity == accessor.getter_owner_identity &&
          RuntimeCStringMatchesString(descriptor.effective_getter_selector,
                                      entry.selector_storage)) {
        return &accessor;
      }
      if (entry.parameter_count == 1 &&
          entry.return_kind == RuntimeMethodReturnKind::Void &&
          descriptor.effective_setter_available &&
          entry.owner_identity == accessor.setter_owner_identity &&
          RuntimeCStringMatchesString(descriptor.effective_setter_selector,
                                      entry.selector_storage)) {
        return &accessor;
      }
    }
  }
  return nullptr;
}

void RebindRestoredMethodCacheAccessorsUnlocked(RuntimeState &state) {
  for (auto cache_it = state.method_cache.begin();
       cache_it != state.method_cache.end();) {
    MethodCacheEntry &entry = cache_it->second;
    if (entry.runtime_property_accessor == nullptr) {
      ++cache_it;
      continue;
    }
    const RealizedPropertyAccessor *accessor =
        FindRestoredRuntimePropertyAccessorUnlocked(state, entry);
    if (accessor == nullptr) {
      cache_it = state.method_cache.erase(cache_it);
      state.last_method_cache_invalidation_reason =
          OBJC3_RUNTIME_METHOD_CACHE_INVALIDATION_PROPERTY_ACCESSOR_REBIND;
      continue;
    }
    entry.runtime_property_accessor = accessor;
    ++cache_it;
  }
}

void RebindRestoredRuntimeRegistrationPointersUnlocked(RuntimeState &state) {
  RebindRestoredRealizedClassNodeImagesUnlocked(state);
  RebindRestoredMethodCacheAccessorsUnlocked(state);
}

struct RuntimeRegistrationMutationCheckpoint {
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
  std::uint64_t malformed_class_metadata_rejection_count = 0;
  std::string last_malformed_class_graph_reason;
  std::string last_malformed_class_graph_metadata_surface;
  std::string last_malformed_class_graph_target_kind;
  std::string last_malformed_class_graph_visibility_state;
  std::string last_malformed_class_graph_availability_state;
  std::unordered_map<MethodCacheKey, MethodCacheEntry, MethodCacheKeyHash>
      method_cache;
  std::uint64_t method_cache_hit_count = 0;
  std::uint64_t method_cache_miss_count = 0;
  std::uint64_t slow_path_lookup_count = 0;
  std::uint64_t stale_method_cache_entry_count = 0;
  std::uint64_t next_method_cache_entry_generation = 1;
  int last_method_cache_invalidation_reason =
      OBJC3_RUNTIME_METHOD_CACHE_INVALIDATION_NONE;
  std::uint64_t fast_path_seed_count = 0;
  std::uint64_t fast_path_hit_count = 0;
  std::uint64_t class_graph_generation = 0;
  std::uint64_t category_attachment_generation = 0;
  std::uint64_t protocol_declaration_generation = 0;
  std::uint64_t storage_surface_generation = 0;
  std::uint64_t method_surface_generation = 0;
  std::unordered_map<PropertyLookupCacheKey, PropertyLookupCacheEntry,
                     PropertyLookupCacheKeyHash>
      property_lookup_cache;
  std::uint64_t property_lookup_cache_hit_count = 0;
  std::uint64_t property_lookup_cache_miss_count = 0;
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

  explicit RuntimeRegistrationMutationCheckpoint(const RuntimeState &state)
      : registered_image_count(state.registered_image_count),
        registered_descriptor_total(state.registered_descriptor_total),
        next_expected_registration_order_ordinal(
            state.next_expected_registration_order_ordinal),
        last_successful_registration_order_ordinal(
            state.last_successful_registration_order_ordinal),
        last_registration_status(state.last_registration_status),
        last_registered_module_name(state.last_registered_module_name),
        last_registered_translation_unit_identity_key(
            state.last_registered_translation_unit_identity_key),
        last_rejected_module_name(state.last_rejected_module_name),
        last_rejected_translation_unit_identity_key(
            state.last_rejected_translation_unit_identity_key),
        last_rejected_registration_order_ordinal(
            state.last_rejected_registration_order_ordinal),
        registration_order_by_identity_key(
            state.registration_order_by_identity_key),
        registered_image_metadata_by_identity_key(
            state.registered_image_metadata_by_identity_key),
        retained_bootstrap_identity_order(
            state.retained_bootstrap_identity_order),
        retained_bootstrap_metadata_by_identity_key(
            state.retained_bootstrap_metadata_by_identity_key),
        selector_index_by_name(state.selector_index_by_name),
        selector_slots(state.selector_slots),
        keypath_slots(state.keypath_slots),
        metadata_backed_selector_count(state.metadata_backed_selector_count),
        dynamic_selector_count(state.dynamic_selector_count),
        metadata_provider_edge_count(state.metadata_provider_edge_count),
        image_backed_keypath_count(state.image_backed_keypath_count),
        ambiguous_keypath_handle_count(state.ambiguous_keypath_handle_count),
        last_materialized_selector(state.last_materialized_selector),
        last_materialized_stable_id(state.last_materialized_stable_id),
        last_materialized_registration_order_ordinal(
            state.last_materialized_registration_order_ordinal),
        last_materialized_selector_pool_index(
            state.last_materialized_selector_pool_index),
        last_materialized_from_metadata(state.last_materialized_from_metadata),
        last_materialized_keypath_handle(
            state.last_materialized_keypath_handle),
        last_materialized_keypath_registration_order_ordinal(
            state.last_materialized_keypath_registration_order_ordinal),
        last_materialized_keypath_profile(
            state.last_materialized_keypath_profile),
        malformed_class_metadata_rejection_count(
            state.malformed_class_metadata_rejection_count),
        last_malformed_class_graph_reason(
            state.last_malformed_class_graph_reason),
        last_malformed_class_graph_metadata_surface(
            state.last_malformed_class_graph_metadata_surface),
        last_malformed_class_graph_target_kind(
            state.last_malformed_class_graph_target_kind),
        last_malformed_class_graph_visibility_state(
            state.last_malformed_class_graph_visibility_state),
        last_malformed_class_graph_availability_state(
            state.last_malformed_class_graph_availability_state),
        method_cache(state.method_cache),
        method_cache_hit_count(state.method_cache_hit_count),
        method_cache_miss_count(state.method_cache_miss_count),
        slow_path_lookup_count(state.slow_path_lookup_count),
        stale_method_cache_entry_count(state.stale_method_cache_entry_count),
        next_method_cache_entry_generation(
            state.next_method_cache_entry_generation),
        last_method_cache_invalidation_reason(
            state.last_method_cache_invalidation_reason),
        fast_path_seed_count(state.fast_path_seed_count),
        fast_path_hit_count(state.fast_path_hit_count),
        class_graph_generation(state.class_graph_generation),
        category_attachment_generation(state.category_attachment_generation),
        protocol_declaration_generation(state.protocol_declaration_generation),
        storage_surface_generation(state.storage_surface_generation),
        method_surface_generation(state.method_surface_generation),
        property_lookup_cache(state.property_lookup_cache),
        property_lookup_cache_hit_count(state.property_lookup_cache_hit_count),
        property_lookup_cache_miss_count(
            state.property_lookup_cache_miss_count),
        staged_registration_table(state.staged_registration_table),
        walked_image_count(state.walked_image_count),
        last_discovery_root_entry_count(state.last_discovery_root_entry_count),
        last_walked_class_descriptor_count(
            state.last_walked_class_descriptor_count),
        last_walked_protocol_descriptor_count(
            state.last_walked_protocol_descriptor_count),
        last_walked_category_descriptor_count(
            state.last_walked_category_descriptor_count),
        last_walked_property_descriptor_count(
            state.last_walked_property_descriptor_count),
        last_walked_ivar_descriptor_count(
            state.last_walked_ivar_descriptor_count),
        last_walked_selector_pool_count(state.last_walked_selector_pool_count),
        last_walked_string_pool_count(state.last_walked_string_pool_count),
        last_walked_keypath_descriptor_count(
            state.last_walked_keypath_descriptor_count),
        last_linker_anchor_matches_discovery_root(
            state.last_linker_anchor_matches_discovery_root),
        last_registration_used_staged_table(
            state.last_registration_used_staged_table),
        last_walked_module_name(state.last_walked_module_name),
        last_walked_translation_unit_identity_key(
            state.last_walked_translation_unit_identity_key),
        realized_class_name_by_base_identity(
            state.realized_class_name_by_base_identity),
        ambiguous_realized_base_identities(
            state.ambiguous_realized_base_identities),
        realized_class_node_indices_by_name(
            state.realized_class_node_indices_by_name),
        realized_class_nodes(state.realized_class_nodes),
        realized_root_class_count(state.realized_root_class_count),
        realized_metaclass_edge_count(state.realized_metaclass_edge_count),
        receiver_class_binding_count(state.receiver_class_binding_count),
        realized_attached_category_count(
            state.realized_attached_category_count),
        realized_protocol_conformance_edge_count(
            state.realized_protocol_conformance_edge_count),
        last_realized_class_name(state.last_realized_class_name),
        last_realized_class_owner_identity(
            state.last_realized_class_owner_identity),
        last_realized_metaclass_owner_identity(
            state.last_realized_metaclass_owner_identity),
        last_attached_category_owner_identity(
            state.last_attached_category_owner_identity),
        last_attached_category_name(state.last_attached_category_name) {}

  void Restore(RuntimeState &state) const {
    state.registered_image_count = registered_image_count;
    state.registered_descriptor_total = registered_descriptor_total;
    state.next_expected_registration_order_ordinal =
        next_expected_registration_order_ordinal;
    state.last_successful_registration_order_ordinal =
        last_successful_registration_order_ordinal;
    state.last_registration_status = last_registration_status;
    state.last_registered_module_name = last_registered_module_name;
    state.last_registered_translation_unit_identity_key =
        last_registered_translation_unit_identity_key;
    state.last_rejected_module_name = last_rejected_module_name;
    state.last_rejected_translation_unit_identity_key =
        last_rejected_translation_unit_identity_key;
    state.last_rejected_registration_order_ordinal =
        last_rejected_registration_order_ordinal;
    state.registration_order_by_identity_key =
        registration_order_by_identity_key;
    state.registered_image_metadata_by_identity_key =
        registered_image_metadata_by_identity_key;
    state.retained_bootstrap_identity_order = retained_bootstrap_identity_order;
    state.retained_bootstrap_metadata_by_identity_key =
        retained_bootstrap_metadata_by_identity_key;
    state.selector_index_by_name = selector_index_by_name;
    state.selector_slots = selector_slots;
    for (SelectorSlot &slot : state.selector_slots) {
      slot.handle.selector = slot.spelling_storage.c_str();
    }
    state.keypath_slots = keypath_slots;
    state.metadata_backed_selector_count = metadata_backed_selector_count;
    state.dynamic_selector_count = dynamic_selector_count;
    state.metadata_provider_edge_count = metadata_provider_edge_count;
    state.image_backed_keypath_count = image_backed_keypath_count;
    state.ambiguous_keypath_handle_count = ambiguous_keypath_handle_count;
    state.last_materialized_selector = last_materialized_selector;
    state.last_materialized_stable_id = last_materialized_stable_id;
    state.last_materialized_registration_order_ordinal =
        last_materialized_registration_order_ordinal;
    state.last_materialized_selector_pool_index =
        last_materialized_selector_pool_index;
    state.last_materialized_from_metadata = last_materialized_from_metadata;
    state.last_materialized_keypath_handle = last_materialized_keypath_handle;
    state.last_materialized_keypath_registration_order_ordinal =
        last_materialized_keypath_registration_order_ordinal;
    state.last_materialized_keypath_profile = last_materialized_keypath_profile;
    state.malformed_class_metadata_rejection_count =
        malformed_class_metadata_rejection_count;
    state.last_malformed_class_graph_reason = last_malformed_class_graph_reason;
    state.last_malformed_class_graph_metadata_surface =
        last_malformed_class_graph_metadata_surface;
    state.last_malformed_class_graph_target_kind =
        last_malformed_class_graph_target_kind;
    state.last_malformed_class_graph_visibility_state =
        last_malformed_class_graph_visibility_state;
    state.last_malformed_class_graph_availability_state =
        last_malformed_class_graph_availability_state;
    state.method_cache = method_cache;
    state.method_cache_hit_count = method_cache_hit_count;
    state.method_cache_miss_count = method_cache_miss_count;
    state.slow_path_lookup_count = slow_path_lookup_count;
    state.stale_method_cache_entry_count = stale_method_cache_entry_count;
    state.next_method_cache_entry_generation =
        next_method_cache_entry_generation;
    state.last_method_cache_invalidation_reason =
        last_method_cache_invalidation_reason;
    state.fast_path_seed_count = fast_path_seed_count;
    state.fast_path_hit_count = fast_path_hit_count;
    state.class_graph_generation = class_graph_generation;
    state.category_attachment_generation = category_attachment_generation;
    state.protocol_declaration_generation = protocol_declaration_generation;
    state.storage_surface_generation = storage_surface_generation;
    state.method_surface_generation = method_surface_generation;
    state.property_lookup_cache = property_lookup_cache;
    state.property_lookup_cache_hit_count = property_lookup_cache_hit_count;
    state.property_lookup_cache_miss_count = property_lookup_cache_miss_count;
    state.staged_registration_table = staged_registration_table;
    state.walked_image_count = walked_image_count;
    state.last_discovery_root_entry_count = last_discovery_root_entry_count;
    state.last_walked_class_descriptor_count =
        last_walked_class_descriptor_count;
    state.last_walked_protocol_descriptor_count =
        last_walked_protocol_descriptor_count;
    state.last_walked_category_descriptor_count =
        last_walked_category_descriptor_count;
    state.last_walked_property_descriptor_count =
        last_walked_property_descriptor_count;
    state.last_walked_ivar_descriptor_count = last_walked_ivar_descriptor_count;
    state.last_walked_selector_pool_count = last_walked_selector_pool_count;
    state.last_walked_string_pool_count = last_walked_string_pool_count;
    state.last_walked_keypath_descriptor_count =
        last_walked_keypath_descriptor_count;
    state.last_linker_anchor_matches_discovery_root =
        last_linker_anchor_matches_discovery_root;
    state.last_registration_used_staged_table =
        last_registration_used_staged_table;
    state.last_walked_module_name = last_walked_module_name;
    state.last_walked_translation_unit_identity_key =
        last_walked_translation_unit_identity_key;
    state.realized_class_name_by_base_identity =
        realized_class_name_by_base_identity;
    state.ambiguous_realized_base_identities =
        ambiguous_realized_base_identities;
    state.realized_class_node_indices_by_name =
        realized_class_node_indices_by_name;
    state.realized_class_nodes = realized_class_nodes;
    state.realized_root_class_count = realized_root_class_count;
    state.realized_metaclass_edge_count = realized_metaclass_edge_count;
    state.receiver_class_binding_count = receiver_class_binding_count;
    state.realized_attached_category_count = realized_attached_category_count;
    state.realized_protocol_conformance_edge_count =
        realized_protocol_conformance_edge_count;
    state.last_realized_class_name = last_realized_class_name;
    state.last_realized_class_owner_identity =
        last_realized_class_owner_identity;
    state.last_realized_metaclass_owner_identity =
        last_realized_metaclass_owner_identity;
    state.last_attached_category_owner_identity =
        last_attached_category_owner_identity;
    state.last_attached_category_name = last_attached_category_name;
    RebindRestoredRuntimeRegistrationPointersUnlocked(state);
  }
};

bool RuntimeRegistrationPublishFailedUnlocked(
    const RuntimeState &state,
    const RuntimeRegistrationMutationCheckpoint &checkpoint) {
  return state.malformed_class_metadata_rejection_count >
             checkpoint.malformed_class_metadata_rejection_count &&
         !state.last_malformed_class_graph_reason.empty();
}

}  // namespace

int RegisterImageUnlocked(
    RuntimeState &state, const objc3_runtime_image_descriptor *image,
    const objc3_runtime_registration_table *staged_registration_table,
    bool retain_bootstrap_record, bool mark_image_local_init_state) {
  // runtime-bootstrap-table-consumption anchor: duplicate identity rejection
  // and out-of-order rejection happen before live counters advance, while
  // successful staged-table consumption is the only path allowed to publish
  // bootstrap-visible image-walk state.
  (void)RuntimeImageDescriptorOwnershipModel();
  state.runtime_owner_split_explicit = RuntimeOwnerSplitContractIsReady();
  if (!RuntimeImageDescriptorHasRequiredIdentity(image)) {
    MarkRejectedRegistrationUnlocked(
        state, image, OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR);
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  if (state.registration_order_by_identity_key.find(
          image->translation_unit_identity_key) !=
      state.registration_order_by_identity_key.end()) {
    MarkRejectedRegistrationUnlocked(
        state, image,
        OBJC3_RUNTIME_REGISTRATION_STATUS_DUPLICATE_TRANSLATION_UNIT_IDENTITY_KEY);
    return OBJC3_RUNTIME_REGISTRATION_STATUS_DUPLICATE_TRANSLATION_UNIT_IDENTITY_KEY;
  }

  if (image->registration_order_ordinal !=
      state.next_expected_registration_order_ordinal) {
    MarkRejectedRegistrationUnlocked(
        state, image,
        OBJC3_RUNTIME_REGISTRATION_STATUS_OUT_OF_ORDER_REGISTRATION);
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OUT_OF_ORDER_REGISTRATION;
  }

  const RuntimeRegistrationMutationCheckpoint mutation_checkpoint(state);
  std::uint64_t descriptor_total = RuntimeDescriptorTotal(image);
  unsigned char *image_local_init_state_to_restore = nullptr;
  unsigned char previous_image_local_init_state = 0;
  if (staged_registration_table != nullptr) {
    state.registration_order_by_identity_key.reserve(
        state.registration_order_by_identity_key.size() + 1u);
    state.registered_image_metadata_by_identity_key.reserve(
        state.registered_image_metadata_by_identity_key.size() + 1u);
    RegisteredImageMetadata record;
    if (!TryWalkRegistrationTableUnlocked(state, staged_registration_table,
                                          image, record)) {
      const bool has_malformed_metadata_rejection =
          state.malformed_class_metadata_rejection_count >
              mutation_checkpoint.malformed_class_metadata_rejection_count &&
          !state.last_malformed_class_graph_reason.empty();
      const std::string malformed_reason =
          has_malformed_metadata_rejection
              ? state.last_malformed_class_graph_reason
              : "";
      mutation_checkpoint.Restore(state);
      if (has_malformed_metadata_rejection) {
        ++state.malformed_class_metadata_rejection_count;
        state.last_malformed_class_graph_reason = malformed_reason;
        RecordRuntimeProtocolCategoryDiagnosticFieldsUnlocked(
            state, malformed_reason);
      }
      ClearImageWalkSnapshotUnlocked(state);
      MarkRejectedRegistrationUnlocked(
          state, image,
          OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_REGISTRATION_ROOTS);
      return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_REGISTRATION_ROOTS;
    }
    descriptor_total =
        record.class_descriptor_count + record.protocol_descriptor_count +
        record.category_descriptor_count + record.property_descriptor_count +
        record.ivar_descriptor_count;
    if (retain_bootstrap_record) {
      RetainBootstrapRecordUnlocked(state, record);
    }
    state.registered_image_metadata_by_identity_key
        [record.translation_unit_identity_key] = record;
    ApplyImageWalkRecordUnlocked(
        state, state.registered_image_metadata_by_identity_key.at(
                   record.translation_unit_identity_key));
    if (mark_image_local_init_state &&
        staged_registration_table->image_local_init_state != nullptr) {
      image_local_init_state_to_restore =
          staged_registration_table->image_local_init_state;
      previous_image_local_init_state = *image_local_init_state_to_restore;
      *staged_registration_table->image_local_init_state = 1;
    }
  } else {
    ClearImageWalkSnapshotUnlocked(state);
  }

  ++state.registered_image_count;
  state.registered_descriptor_total += descriptor_total;
  state.next_expected_registration_order_ordinal =
      image->registration_order_ordinal + 1;
  state.last_successful_registration_order_ordinal =
      image->registration_order_ordinal;
  state.last_registration_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  state.last_malformed_class_graph_reason.clear();
  ClearRuntimeProtocolCategoryDiagnosticFieldsUnlocked(state);
  state.last_registered_module_name = image->module_name;
  state.last_registered_translation_unit_identity_key =
      image->translation_unit_identity_key;
  state.registration_order_by_identity_key.emplace(
      image->translation_unit_identity_key, image->registration_order_ordinal);
  if (staged_registration_table != nullptr) {
    const RegisteredImageMetadata &record =
        state.registered_image_metadata_by_identity_key.at(
            image->translation_unit_identity_key);
    if (record.protocol_descriptor_count != 0) {
      BumpRuntimeProtocolDeclarationGenerationUnlocked(state);
    }
  }
  RebuildRealizedClassGraphUnlocked(state);
  if (RuntimeRegistrationPublishFailedUnlocked(state, mutation_checkpoint)) {
    const std::string malformed_reason =
        state.last_malformed_class_graph_reason;
    if (image_local_init_state_to_restore != nullptr) {
      *image_local_init_state_to_restore = previous_image_local_init_state;
    }
    mutation_checkpoint.Restore(state);
    ++state.malformed_class_metadata_rejection_count;
    state.last_malformed_class_graph_reason = malformed_reason;
    RecordRuntimeProtocolCategoryDiagnosticFieldsUnlocked(state,
                                                          malformed_reason);
    MarkRejectedRegistrationUnlocked(
        state, image,
        OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_REGISTRATION_ROOTS);
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_REGISTRATION_ROOTS;
  }
  ClearRejectedRegistrationUnlocked(state);
  SeedDispatchIntentFastPathCacheUnlocked(state);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

}  // namespace objc3c::runtime
