#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_STABILIZER_COMMON_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_STABILIZER_COMMON_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe {

// Runtime snapshot string fields are runtime-owned. Probes that reset, replay,
// or otherwise mutate runtime state must copy strings before printing later.
inline void StabilizeNullableCString(const char *source, std::string &storage,
                                     const char *&field) {
  const std::string stable_source = source != nullptr ? source : "";
  storage = stable_source;
  field = storage.empty() ? nullptr : storage.c_str();
}

inline void StabilizeArcDebugSnapshot(
    objc3_runtime_arc_debug_state_snapshot &snapshot,
    std::string &property_name_storage, std::string &owner_storage) {
  StabilizeNullableCString(snapshot.last_property_name, property_name_storage,
                           snapshot.last_property_name);
  StabilizeNullableCString(snapshot.last_property_owner_identity, owner_storage,
                           snapshot.last_property_owner_identity);
}

inline void StabilizeAggregateSnapshot(
    objc3_runtime_object_model_query_state_snapshot &snapshot,
    std::string &queried_class_storage, std::string &resolved_class_storage,
    std::string &resolved_class_owner_storage,
    std::string &queried_property_storage,
    std::string &resolved_property_class_storage,
    std::string &resolved_property_owner_storage,
    std::string &queried_protocol_class_storage,
    std::string &queried_protocol_storage,
    std::string &matched_protocol_owner_storage,
    std::string &matched_attachment_owner_storage) {
  StabilizeNullableCString(snapshot.last_queried_class_name,
                           queried_class_storage,
                           snapshot.last_queried_class_name);
  StabilizeNullableCString(snapshot.last_resolved_class_name,
                           resolved_class_storage,
                           snapshot.last_resolved_class_name);
  StabilizeNullableCString(snapshot.last_resolved_class_owner_identity,
                           resolved_class_owner_storage,
                           snapshot.last_resolved_class_owner_identity);
  StabilizeNullableCString(snapshot.last_queried_property_name,
                           queried_property_storage,
                           snapshot.last_queried_property_name);
  StabilizeNullableCString(snapshot.last_resolved_property_class_name,
                           resolved_property_class_storage,
                           snapshot.last_resolved_property_class_name);
  StabilizeNullableCString(snapshot.last_resolved_property_owner_identity,
                           resolved_property_owner_storage,
                           snapshot.last_resolved_property_owner_identity);
  StabilizeNullableCString(snapshot.last_queried_protocol_class_name,
                           queried_protocol_class_storage,
                           snapshot.last_queried_protocol_class_name);
  StabilizeNullableCString(snapshot.last_queried_protocol_name,
                           queried_protocol_storage,
                           snapshot.last_queried_protocol_name);
  StabilizeNullableCString(snapshot.last_matched_protocol_owner_identity,
                           matched_protocol_owner_storage,
                           snapshot.last_matched_protocol_owner_identity);
  StabilizeNullableCString(snapshot.last_matched_attachment_owner_identity,
                           matched_attachment_owner_storage,
                           snapshot.last_matched_attachment_owner_identity);
}

inline void StabilizeConformanceQuery(
    objc3_runtime_protocol_conformance_query_snapshot &snapshot,
    std::string &class_storage, std::string &protocol_storage,
    std::string &protocol_owner_storage,
    std::string &attachment_owner_storage,
    std::string *matched_class_storage = nullptr,
    std::string *matched_class_owner_storage = nullptr,
    std::string *failure_reason_storage = nullptr,
    std::string *existential_canonical_spelling_storage = nullptr,
    std::string *object_representation_storage = nullptr,
    std::string *conformance_owner_identity_storage = nullptr,
    std::string *runtime_lookup_anchor_storage = nullptr,
    std::string *witness_metadata_key_storage = nullptr,
    std::string *requirement_resolution_policy_storage = nullptr,
    std::string *associated_type_diagnostic_storage = nullptr,
    std::string *dynamic_dispatch_diagnostic_storage = nullptr) {
  StabilizeNullableCString(snapshot.class_name, class_storage,
                           snapshot.class_name);
  StabilizeNullableCString(snapshot.protocol_name, protocol_storage,
                           snapshot.protocol_name);
  StabilizeNullableCString(snapshot.matched_protocol_owner_identity,
                           protocol_owner_storage,
                           snapshot.matched_protocol_owner_identity);
  StabilizeNullableCString(snapshot.matched_attachment_owner_identity,
                           attachment_owner_storage,
                           snapshot.matched_attachment_owner_identity);
  if (matched_class_storage != nullptr) {
    StabilizeNullableCString(snapshot.matched_class_name,
                             *matched_class_storage,
                             snapshot.matched_class_name);
  }
  if (matched_class_owner_storage != nullptr) {
    StabilizeNullableCString(snapshot.matched_class_owner_identity,
                             *matched_class_owner_storage,
                             snapshot.matched_class_owner_identity);
  }
  if (failure_reason_storage != nullptr) {
    StabilizeNullableCString(snapshot.failure_reason,
                             *failure_reason_storage,
                             snapshot.failure_reason);
  }
  if (existential_canonical_spelling_storage != nullptr) {
    StabilizeNullableCString(snapshot.existential_canonical_spelling,
                             *existential_canonical_spelling_storage,
                             snapshot.existential_canonical_spelling);
  }
  if (object_representation_storage != nullptr) {
    StabilizeNullableCString(snapshot.object_representation,
                             *object_representation_storage,
                             snapshot.object_representation);
  }
  if (conformance_owner_identity_storage != nullptr) {
    StabilizeNullableCString(snapshot.conformance_owner_identity,
                             *conformance_owner_identity_storage,
                             snapshot.conformance_owner_identity);
  }
  if (runtime_lookup_anchor_storage != nullptr) {
    StabilizeNullableCString(snapshot.runtime_lookup_anchor,
                             *runtime_lookup_anchor_storage,
                             snapshot.runtime_lookup_anchor);
  }
  if (witness_metadata_key_storage != nullptr) {
    StabilizeNullableCString(snapshot.witness_metadata_key,
                             *witness_metadata_key_storage,
                             snapshot.witness_metadata_key);
  }
  if (requirement_resolution_policy_storage != nullptr) {
    StabilizeNullableCString(snapshot.requirement_resolution_policy,
                             *requirement_resolution_policy_storage,
                             snapshot.requirement_resolution_policy);
  }
  if (associated_type_diagnostic_storage != nullptr) {
    StabilizeNullableCString(snapshot.unsupported_associated_type_diagnostic,
                             *associated_type_diagnostic_storage,
                             snapshot.unsupported_associated_type_diagnostic);
  }
  if (dynamic_dispatch_diagnostic_storage != nullptr) {
    StabilizeNullableCString(snapshot.unsupported_dynamic_dispatch_diagnostic,
                             *dynamic_dispatch_diagnostic_storage,
                             snapshot.unsupported_dynamic_dispatch_diagnostic);
  }
}

inline void StabilizeDispatchState(
    objc3_runtime_dispatch_state_snapshot &snapshot,
    std::string &selector_storage, std::string &fast_path_reason_storage,
    std::string &dispatch_path_storage,
    std::string &implementation_kind_storage,
    std::string &property_name_storage, std::string &resolved_class_storage,
    std::string &resolved_owner_storage) {
  StabilizeNullableCString(snapshot.last_selector, selector_storage,
                           snapshot.last_selector);
  StabilizeNullableCString(snapshot.last_fast_path_reason,
                           fast_path_reason_storage,
                           snapshot.last_fast_path_reason);
  StabilizeNullableCString(snapshot.last_dispatch_path, dispatch_path_storage,
                           snapshot.last_dispatch_path);
  StabilizeNullableCString(snapshot.last_implementation_kind,
                           implementation_kind_storage,
                           snapshot.last_implementation_kind);
  StabilizeNullableCString(snapshot.last_property_name, property_name_storage,
                           snapshot.last_property_name);
  StabilizeNullableCString(snapshot.last_resolved_class_name,
                           resolved_class_storage,
                           snapshot.last_resolved_class_name);
  StabilizeNullableCString(snapshot.last_resolved_owner_identity,
                           resolved_owner_storage,
                           snapshot.last_resolved_owner_identity);
}

inline void StabilizeGraphState(
    objc3_runtime_realized_class_graph_state_snapshot &snapshot,
    std::string &class_storage, std::string &class_owner_storage,
    std::string &metaclass_owner_storage, std::string &category_owner_storage,
    std::string &category_name_storage) {
  StabilizeNullableCString(snapshot.last_realized_class_name, class_storage,
                           snapshot.last_realized_class_name);
  StabilizeNullableCString(snapshot.last_realized_class_owner_identity,
                           class_owner_storage,
                           snapshot.last_realized_class_owner_identity);
  StabilizeNullableCString(snapshot.last_realized_metaclass_owner_identity,
                           metaclass_owner_storage,
                           snapshot.last_realized_metaclass_owner_identity);
  StabilizeNullableCString(snapshot.last_attached_category_owner_identity,
                           category_owner_storage,
                           snapshot.last_attached_category_owner_identity);
  StabilizeNullableCString(snapshot.last_attached_category_name,
                           category_name_storage,
                           snapshot.last_attached_category_name);
}

inline void StabilizeGraph(
    objc3_runtime_realized_class_graph_state_snapshot &snapshot,
    std::string &class_storage) {
  StabilizeNullableCString(snapshot.last_allocated_class_name, class_storage,
                           snapshot.last_allocated_class_name);
}

inline void StabilizeImageWalkState(
    objc3_runtime_image_walk_state_snapshot &snapshot,
    std::string &module_storage, std::string &identity_storage) {
  StabilizeNullableCString(snapshot.last_walked_module_name, module_storage,
                           snapshot.last_walked_module_name);
  StabilizeNullableCString(snapshot.last_walked_translation_unit_identity_key,
                           identity_storage,
                           snapshot.last_walked_translation_unit_identity_key);
}

inline void StabilizeMethodCacheEntry(
    objc3_runtime_method_cache_entry_snapshot &snapshot,
    std::string &selector_storage, std::string &class_storage,
    std::string &owner_storage) {
  StabilizeNullableCString(snapshot.selector, selector_storage,
                           snapshot.selector);
  StabilizeNullableCString(snapshot.resolved_class_name, class_storage,
                           snapshot.resolved_class_name);
  StabilizeNullableCString(snapshot.resolved_owner_identity, owner_storage,
                           snapshot.resolved_owner_identity);
}

inline void StabilizeMethodCacheState(
    objc3_runtime_method_cache_state_snapshot &snapshot,
    std::string &selector_storage, std::string &class_storage,
    std::string &owner_storage) {
  StabilizeNullableCString(snapshot.last_selector, selector_storage,
                           snapshot.last_selector);
  StabilizeNullableCString(snapshot.last_resolved_class_name, class_storage,
                           snapshot.last_resolved_class_name);
  StabilizeNullableCString(snapshot.last_resolved_owner_identity, owner_storage,
                           snapshot.last_resolved_owner_identity);
}


}  // namespace objc3c::runtime::probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_STABILIZER_COMMON_H_
