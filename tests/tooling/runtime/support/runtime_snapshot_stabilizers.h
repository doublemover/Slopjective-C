#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_STABILIZERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_STABILIZERS_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe {

// Runtime snapshot string fields are runtime-owned. Probes that reset, replay,
// or otherwise mutate runtime state must copy strings before printing later.
inline void StabilizeNullableCString(const char *source, std::string &storage,
                                     const char *&field) {
  storage = source != nullptr ? source : "";
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
    std::string &attachment_owner_storage) {
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

inline void StabilizePropertyEntry(
    objc3_runtime_property_entry_snapshot &snapshot,
    std::string &queried_class_storage, std::string &resolved_class_storage,
    std::string &property_name_storage,
    std::string &declaration_owner_storage,
    std::string &export_owner_storage, std::string &getter_selector_storage,
    std::string &setter_selector_storage,
    std::string &effective_getter_selector_storage,
    std::string &effective_setter_selector_storage,
    std::string &ivar_binding_storage,
    std::string &synthesized_binding_storage,
    std::string &layout_symbol_storage, std::string &getter_owner_storage,
    std::string &setter_owner_storage) {
  StabilizeNullableCString(snapshot.queried_class_name, queried_class_storage,
                           snapshot.queried_class_name);
  StabilizeNullableCString(snapshot.resolved_class_name, resolved_class_storage,
                           snapshot.resolved_class_name);
  StabilizeNullableCString(snapshot.property_name, property_name_storage,
                           snapshot.property_name);
  StabilizeNullableCString(snapshot.declaration_owner_identity,
                           declaration_owner_storage,
                           snapshot.declaration_owner_identity);
  StabilizeNullableCString(snapshot.export_owner_identity, export_owner_storage,
                           snapshot.export_owner_identity);
  StabilizeNullableCString(snapshot.getter_selector, getter_selector_storage,
                           snapshot.getter_selector);
  StabilizeNullableCString(snapshot.setter_selector, setter_selector_storage,
                           snapshot.setter_selector);
  StabilizeNullableCString(snapshot.effective_getter_selector,
                           effective_getter_selector_storage,
                           snapshot.effective_getter_selector);
  StabilizeNullableCString(snapshot.effective_setter_selector,
                           effective_setter_selector_storage,
                           snapshot.effective_setter_selector);
  StabilizeNullableCString(snapshot.ivar_binding_symbol, ivar_binding_storage,
                           snapshot.ivar_binding_symbol);
  StabilizeNullableCString(snapshot.synthesized_binding_symbol,
                           synthesized_binding_storage,
                           snapshot.synthesized_binding_symbol);
  StabilizeNullableCString(snapshot.ivar_layout_symbol, layout_symbol_storage,
                           snapshot.ivar_layout_symbol);
  StabilizeNullableCString(snapshot.getter_owner_identity,
                           getter_owner_storage,
                           snapshot.getter_owner_identity);
  StabilizeNullableCString(snapshot.setter_owner_identity,
                           setter_owner_storage,
                           snapshot.setter_owner_identity);
}

inline void StabilizePropertyEntry(
    objc3_runtime_property_entry_snapshot &snapshot,
    std::string &queried_class_storage, std::string &resolved_class_storage,
    std::string &property_name_storage,
    std::string &declaration_owner_storage,
    std::string &getter_owner_storage, std::string &setter_owner_storage,
    std::string &lifetime_storage, std::string &hook_storage,
    std::string &accessor_storage) {
  StabilizeNullableCString(snapshot.queried_class_name, queried_class_storage,
                           snapshot.queried_class_name);
  StabilizeNullableCString(snapshot.resolved_class_name, resolved_class_storage,
                           snapshot.resolved_class_name);
  StabilizeNullableCString(snapshot.property_name, property_name_storage,
                           snapshot.property_name);
  StabilizeNullableCString(snapshot.declaration_owner_identity,
                           declaration_owner_storage,
                           snapshot.declaration_owner_identity);
  StabilizeNullableCString(snapshot.getter_owner_identity, getter_owner_storage,
                           snapshot.getter_owner_identity);
  StabilizeNullableCString(snapshot.setter_owner_identity, setter_owner_storage,
                           snapshot.setter_owner_identity);
  StabilizeNullableCString(snapshot.ownership_lifetime_profile,
                           lifetime_storage,
                           snapshot.ownership_lifetime_profile);
  StabilizeNullableCString(snapshot.ownership_runtime_hook_profile,
                           hook_storage,
                           snapshot.ownership_runtime_hook_profile);
  StabilizeNullableCString(snapshot.accessor_ownership_profile,
                           accessor_storage,
                           snapshot.accessor_ownership_profile);
}

inline void StabilizePropertyEntry(
    objc3_runtime_property_entry_snapshot &snapshot,
    std::string &queried_class_storage, std::string &resolved_class_storage,
    std::string &property_name_storage, std::string &owner_storage,
    std::string &lifetime_storage, std::string &hook_storage,
    std::string &accessor_storage) {
  StabilizeNullableCString(snapshot.queried_class_name, queried_class_storage,
                           snapshot.queried_class_name);
  StabilizeNullableCString(snapshot.resolved_class_name, resolved_class_storage,
                           snapshot.resolved_class_name);
  StabilizeNullableCString(snapshot.property_name, property_name_storage,
                           snapshot.property_name);
  StabilizeNullableCString(snapshot.declaration_owner_identity, owner_storage,
                           snapshot.declaration_owner_identity);
  StabilizeNullableCString(snapshot.ownership_lifetime_profile,
                           lifetime_storage,
                           snapshot.ownership_lifetime_profile);
  StabilizeNullableCString(snapshot.ownership_runtime_hook_profile,
                           hook_storage,
                           snapshot.ownership_runtime_hook_profile);
  StabilizeNullableCString(snapshot.accessor_ownership_profile,
                           accessor_storage,
                           snapshot.accessor_ownership_profile);
}

template <typename StablePropertyEntry>
inline void StabilizePropertyEntry(StablePropertyEntry &entry) {
  StabilizeNullableCString(entry.snapshot.queried_class_name,
                           entry.queried_class_name,
                           entry.snapshot.queried_class_name);
  StabilizeNullableCString(entry.snapshot.resolved_class_name,
                           entry.resolved_class_name,
                           entry.snapshot.resolved_class_name);
  StabilizeNullableCString(entry.snapshot.property_name, entry.property_name,
                           entry.snapshot.property_name);
  StabilizeNullableCString(entry.snapshot.declaration_owner_identity,
                           entry.declaration_owner_identity,
                           entry.snapshot.declaration_owner_identity);
  StabilizeNullableCString(entry.snapshot.export_owner_identity,
                           entry.export_owner_identity,
                           entry.snapshot.export_owner_identity);
  StabilizeNullableCString(entry.snapshot.getter_selector,
                           entry.getter_selector,
                           entry.snapshot.getter_selector);
  StabilizeNullableCString(entry.snapshot.setter_selector,
                           entry.setter_selector,
                           entry.snapshot.setter_selector);
  StabilizeNullableCString(entry.snapshot.effective_getter_selector,
                           entry.effective_getter_selector,
                           entry.snapshot.effective_getter_selector);
  StabilizeNullableCString(entry.snapshot.effective_setter_selector,
                           entry.effective_setter_selector,
                           entry.snapshot.effective_setter_selector);
  StabilizeNullableCString(entry.snapshot.ivar_binding_symbol,
                           entry.ivar_binding_symbol,
                           entry.snapshot.ivar_binding_symbol);
  StabilizeNullableCString(entry.snapshot.synthesized_binding_symbol,
                           entry.synthesized_binding_symbol,
                           entry.snapshot.synthesized_binding_symbol);
  StabilizeNullableCString(entry.snapshot.ivar_layout_symbol,
                           entry.ivar_layout_symbol,
                           entry.snapshot.ivar_layout_symbol);
  StabilizeNullableCString(entry.snapshot.property_attribute_profile,
                           entry.property_attribute_profile,
                           entry.snapshot.property_attribute_profile);
  StabilizeNullableCString(entry.snapshot.ownership_lifetime_profile,
                           entry.ownership_lifetime_profile,
                           entry.snapshot.ownership_lifetime_profile);
  StabilizeNullableCString(entry.snapshot.ownership_runtime_hook_profile,
                           entry.ownership_runtime_hook_profile,
                           entry.snapshot.ownership_runtime_hook_profile);
  StabilizeNullableCString(entry.snapshot.accessor_ownership_profile,
                           entry.accessor_ownership_profile,
                           entry.snapshot.accessor_ownership_profile);
  StabilizeNullableCString(entry.snapshot.getter_owner_identity,
                           entry.getter_owner_identity,
                           entry.snapshot.getter_owner_identity);
  StabilizeNullableCString(entry.snapshot.setter_owner_identity,
                           entry.setter_owner_identity,
                           entry.snapshot.setter_owner_identity);
}

inline void StabilizePropertyRegistryState(
    objc3_runtime_property_registry_state_snapshot &snapshot,
    std::string &queried_class_storage, std::string &queried_property_storage,
    std::string &resolved_class_storage, std::string &resolved_owner_storage) {
  StabilizeNullableCString(snapshot.last_queried_class_name,
                           queried_class_storage,
                           snapshot.last_queried_class_name);
  StabilizeNullableCString(snapshot.last_queried_property_name,
                           queried_property_storage,
                           snapshot.last_queried_property_name);
  StabilizeNullableCString(snapshot.last_resolved_class_name,
                           resolved_class_storage,
                           snapshot.last_resolved_class_name);
  StabilizeNullableCString(snapshot.last_resolved_owner_identity,
                           resolved_owner_storage,
                           snapshot.last_resolved_owner_identity);
}

inline void StabilizeRealizedClassEntry(
    objc3_runtime_realized_class_entry_snapshot &snapshot,
    std::string &module_storage, std::string &identity_storage,
    std::string &class_storage, std::string &class_owner_storage,
    std::string &metaclass_owner_storage, std::string &super_class_storage,
    std::string &super_metaclass_storage, std::string &category_owner_storage,
    std::string &category_name_storage) {
  StabilizeNullableCString(snapshot.module_name, module_storage,
                           snapshot.module_name);
  StabilizeNullableCString(snapshot.translation_unit_identity_key,
                           identity_storage,
                           snapshot.translation_unit_identity_key);
  StabilizeNullableCString(snapshot.class_name, class_storage,
                           snapshot.class_name);
  StabilizeNullableCString(snapshot.class_owner_identity, class_owner_storage,
                           snapshot.class_owner_identity);
  StabilizeNullableCString(snapshot.metaclass_owner_identity,
                           metaclass_owner_storage,
                           snapshot.metaclass_owner_identity);
  StabilizeNullableCString(snapshot.super_class_owner_identity,
                           super_class_storage,
                           snapshot.super_class_owner_identity);
  StabilizeNullableCString(snapshot.super_metaclass_owner_identity,
                           super_metaclass_storage,
                           snapshot.super_metaclass_owner_identity);
  StabilizeNullableCString(snapshot.last_attached_category_owner_identity,
                           category_owner_storage,
                           snapshot.last_attached_category_owner_identity);
  StabilizeNullableCString(snapshot.last_attached_category_name,
                           category_name_storage,
                           snapshot.last_attached_category_name);
}

template <typename StableRealizedClassEntry>
inline void StabilizeRealizedClassEntry(StableRealizedClassEntry &entry) {
  StabilizeNullableCString(entry.snapshot.module_name, entry.module_name,
                           entry.snapshot.module_name);
  StabilizeNullableCString(entry.snapshot.translation_unit_identity_key,
                           entry.translation_unit_identity_key,
                           entry.snapshot.translation_unit_identity_key);
  StabilizeNullableCString(entry.snapshot.class_name, entry.class_name,
                           entry.snapshot.class_name);
  StabilizeNullableCString(entry.snapshot.class_owner_identity,
                           entry.class_owner_identity,
                           entry.snapshot.class_owner_identity);
  StabilizeNullableCString(entry.snapshot.metaclass_owner_identity,
                           entry.metaclass_owner_identity,
                           entry.snapshot.metaclass_owner_identity);
  StabilizeNullableCString(entry.snapshot.super_class_owner_identity,
                           entry.super_class_owner_identity,
                           entry.snapshot.super_class_owner_identity);
  StabilizeNullableCString(entry.snapshot.super_metaclass_owner_identity,
                           entry.super_metaclass_owner_identity,
                           entry.snapshot.super_metaclass_owner_identity);
  StabilizeNullableCString(entry.snapshot.last_attached_category_owner_identity,
                           entry.attached_category_owner_identity,
                           entry.snapshot.last_attached_category_owner_identity);
  StabilizeNullableCString(entry.snapshot.last_attached_category_name,
                           entry.attached_category_name,
                           entry.snapshot.last_attached_category_name);
}

inline void StabilizeRealizedClassGraph(
    objc3_runtime_realized_class_graph_state_snapshot &snapshot,
    std::string &class_storage) {
  StabilizeNullableCString(snapshot.last_allocated_class_name, class_storage,
                           snapshot.last_allocated_class_name);
}

inline void StabilizeRealizedEntry(
    objc3_runtime_realized_class_entry_snapshot &snapshot,
    std::string &module_storage, std::string &identity_storage,
    std::string &class_storage, std::string &class_owner_storage,
    std::string &metaclass_owner_storage,
    std::string &super_class_owner_storage,
    std::string &super_metaclass_owner_storage) {
  StabilizeNullableCString(snapshot.module_name, module_storage,
                           snapshot.module_name);
  StabilizeNullableCString(snapshot.translation_unit_identity_key,
                           identity_storage,
                           snapshot.translation_unit_identity_key);
  StabilizeNullableCString(snapshot.class_name, class_storage,
                           snapshot.class_name);
  StabilizeNullableCString(snapshot.class_owner_identity, class_owner_storage,
                           snapshot.class_owner_identity);
  StabilizeNullableCString(snapshot.metaclass_owner_identity,
                           metaclass_owner_storage,
                           snapshot.metaclass_owner_identity);
  StabilizeNullableCString(snapshot.super_class_owner_identity,
                           super_class_owner_storage,
                           snapshot.super_class_owner_identity);
  StabilizeNullableCString(snapshot.super_metaclass_owner_identity,
                           super_metaclass_owner_storage,
                           snapshot.super_metaclass_owner_identity);
}

inline void StabilizeRealizedEntry(
    objc3_runtime_realized_class_entry_snapshot &snapshot,
    std::string &module_storage, std::string &identity_storage,
    std::string &class_storage, std::string &class_owner_storage,
    std::string &metaclass_owner_storage,
    std::string &super_class_owner_storage,
    std::string &super_metaclass_owner_storage,
    std::string &category_owner_storage, std::string &category_name_storage) {
  StabilizeRealizedEntry(snapshot, module_storage, identity_storage,
                         class_storage, class_owner_storage,
                         metaclass_owner_storage, super_class_owner_storage,
                         super_metaclass_owner_storage);
  StabilizeNullableCString(snapshot.last_attached_category_owner_identity,
                           category_owner_storage,
                           snapshot.last_attached_category_owner_identity);
  StabilizeNullableCString(snapshot.last_attached_category_name,
                           category_name_storage,
                           snapshot.last_attached_category_name);
}

inline void StabilizeRealizedGraphState(
    objc3_runtime_realized_class_graph_state_snapshot &snapshot,
    std::string &class_storage, std::string &class_owner_storage,
    std::string &metaclass_owner_storage) {
  StabilizeNullableCString(snapshot.last_realized_class_name, class_storage,
                           snapshot.last_realized_class_name);
  StabilizeNullableCString(snapshot.last_realized_class_owner_identity,
                           class_owner_storage,
                           snapshot.last_realized_class_owner_identity);
  StabilizeNullableCString(snapshot.last_realized_metaclass_owner_identity,
                           metaclass_owner_storage,
                           snapshot.last_realized_metaclass_owner_identity);
}

inline void StabilizeRealizedGraphState(
    objc3_runtime_realized_class_graph_state_snapshot &snapshot,
    std::string &class_storage, std::string &owner_storage,
    std::string &metaclass_storage, std::string &category_owner_storage,
    std::string &category_name_storage, std::string &allocated_class_storage) {
  StabilizeRealizedGraphState(snapshot, class_storage, owner_storage,
                              metaclass_storage);
  StabilizeNullableCString(snapshot.last_attached_category_owner_identity,
                           category_owner_storage,
                           snapshot.last_attached_category_owner_identity);
  StabilizeNullableCString(snapshot.last_attached_category_name,
                           category_name_storage,
                           snapshot.last_attached_category_name);
  StabilizeNullableCString(snapshot.last_allocated_class_name,
                           allocated_class_storage,
                           snapshot.last_allocated_class_name);
}

inline void StabilizeRegistrationState(
    objc3_runtime_registration_state_snapshot &snapshot,
    std::string &module_storage, std::string &identity_storage) {
  StabilizeNullableCString(snapshot.last_registered_module_name, module_storage,
                           snapshot.last_registered_module_name);
  StabilizeNullableCString(snapshot.last_registered_translation_unit_identity_key,
                           identity_storage,
                           snapshot.last_registered_translation_unit_identity_key);
}

inline void StabilizeRegistrationState(
    objc3_runtime_registration_state_snapshot &snapshot,
    std::string &module_storage, std::string &identity_storage,
    std::string &rejected_module_storage,
    std::string &rejected_identity_storage) {
  StabilizeRegistrationState(snapshot, module_storage, identity_storage);
  StabilizeNullableCString(snapshot.last_rejected_module_name,
                           rejected_module_storage,
                           snapshot.last_rejected_module_name);
  StabilizeNullableCString(snapshot.last_rejected_translation_unit_identity_key,
                           rejected_identity_storage,
                           snapshot.last_rejected_translation_unit_identity_key);
}

inline void StabilizeResetReplayState(
    objc3_runtime_reset_replay_state_snapshot &snapshot,
    std::string &module_storage, std::string &identity_storage) {
  StabilizeNullableCString(snapshot.last_replayed_module_name, module_storage,
                           snapshot.last_replayed_module_name);
  StabilizeNullableCString(snapshot.last_replayed_translation_unit_identity_key,
                           identity_storage,
                           snapshot.last_replayed_translation_unit_identity_key);
}

inline void StabilizeSelectorEntry(
    objc3_runtime_selector_lookup_entry_snapshot &snapshot,
    std::string &selector_storage) {
  StabilizeNullableCString(snapshot.canonical_selector, selector_storage,
                           snapshot.canonical_selector);
}

inline void StabilizeSelectorTableState(
    objc3_runtime_selector_lookup_table_state_snapshot &snapshot,
    std::string &selector_storage) {
  StabilizeNullableCString(snapshot.last_materialized_selector,
                           selector_storage,
                           snapshot.last_materialized_selector);
}

}  // namespace objc3c::runtime::probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_STABILIZERS_H_
