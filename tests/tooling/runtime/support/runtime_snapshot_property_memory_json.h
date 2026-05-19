#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_PROPERTY_MEMORY_JSON_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_PROPERTY_MEMORY_JSON_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/json_probe_writer.h"

#include <cstdio>

namespace objc3c::runtime::probe {

inline void PrintPropertyRegistryStateFull(
    const objc3_runtime_property_registry_state_snapshot &snapshot) {
  std::printf("{");
  PrintUint64Field(
      "layout_ready_class_count",
      static_cast<unsigned long long>(snapshot.layout_ready_class_count));
  PrintUint64Field(
      "reflectable_property_count",
      static_cast<unsigned long long>(snapshot.reflectable_property_count));
  PrintUint64Field(
      "writable_property_count",
      static_cast<unsigned long long>(snapshot.writable_property_count));
  PrintUint64Field(
      "slot_backed_property_count",
      static_cast<unsigned long long>(snapshot.slot_backed_property_count));
  PrintIntField("last_query_found", snapshot.last_query_found);
  PrintIntField("last_query_inherited", snapshot.last_query_inherited);
  PrintStringField("last_queried_class_name", snapshot.last_queried_class_name);
  PrintStringField("last_queried_property_name",
                   snapshot.last_queried_property_name);
  PrintStringField("last_resolved_class_name",
                   snapshot.last_resolved_class_name);
  PrintStringField("last_resolved_owner_identity",
                   snapshot.last_resolved_owner_identity, false);
  std::printf("}");
}

inline void
PrintPropertyEntryFull(const objc3_runtime_property_entry_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("found", snapshot.found);
  PrintIntField("inherited", snapshot.inherited);
  PrintIntField("setter_available", snapshot.setter_available);
  PrintIntField("has_runtime_getter", snapshot.has_runtime_getter);
  PrintIntField("has_runtime_setter", snapshot.has_runtime_setter);
  PrintUint64Field("attribute_count",
                   static_cast<unsigned long long>(snapshot.attribute_count));
  PrintIntField("is_readonly", snapshot.is_readonly);
  PrintIntField("is_nonatomic", snapshot.is_nonatomic);
  PrintIntField("is_strong", snapshot.is_strong);
  PrintIntField("is_assign", snapshot.is_assign);
  PrintIntField("is_weak", snapshot.is_weak);
  PrintIntField("is_copy", snapshot.is_copy);
  PrintIntField("has_custom_getter", snapshot.has_custom_getter);
  PrintIntField("has_custom_setter", snapshot.has_custom_setter);
  PrintUint64Field("base_identity",
                   static_cast<unsigned long long>(snapshot.base_identity));
  PrintUint64Field("slot_index",
                   static_cast<unsigned long long>(snapshot.slot_index));
  PrintUint64Field("offset_bytes",
                   static_cast<unsigned long long>(snapshot.offset_bytes));
  PrintUint64Field("size_bytes",
                   static_cast<unsigned long long>(snapshot.size_bytes));
  PrintUint64Field("alignment_bytes",
                   static_cast<unsigned long long>(snapshot.alignment_bytes));
  PrintUint64Field("padding_bytes",
                   static_cast<unsigned long long>(snapshot.padding_bytes));
  PrintUint64Field("inherited_slot_count",
                   static_cast<unsigned long long>(
                       snapshot.inherited_slot_count));
  PrintUint64Field("inherited_size_bytes",
                   static_cast<unsigned long long>(
                       snapshot.inherited_size_bytes));
  PrintUint64Field("owner_size_bytes",
                   static_cast<unsigned long long>(snapshot.owner_size_bytes));
  PrintUint64Field("init_order_index",
                   static_cast<unsigned long long>(snapshot.init_order_index));
  PrintUint64Field("destroy_order_index",
                   static_cast<unsigned long long>(
                       snapshot.destroy_order_index));
  PrintIntField("layout_valid", snapshot.layout_valid);
  PrintUint64Field("instance_size_bytes", static_cast<unsigned long long>(
                                              snapshot.instance_size_bytes));
  PrintStringField("queried_class_name", snapshot.queried_class_name);
  PrintStringField("resolved_class_name", snapshot.resolved_class_name);
  PrintStringField("property_name", snapshot.property_name);
  PrintStringField("declaration_owner_identity",
                   snapshot.declaration_owner_identity);
  PrintStringField("export_owner_identity", snapshot.export_owner_identity);
  PrintStringField("getter_selector", snapshot.getter_selector);
  PrintStringField("setter_selector", snapshot.setter_selector);
  PrintStringField("effective_getter_selector",
                   snapshot.effective_getter_selector);
  PrintStringField("effective_setter_selector",
                   snapshot.effective_setter_selector);
  PrintStringField("ivar_binding_symbol", snapshot.ivar_binding_symbol);
  PrintStringField("synthesized_binding_symbol",
                   snapshot.synthesized_binding_symbol);
  PrintStringField("ivar_layout_symbol", snapshot.ivar_layout_symbol);
  PrintStringField("ivar_layout_replay_key", snapshot.ivar_layout_replay_key);
  PrintStringField("property_attribute_profile",
                   snapshot.property_attribute_profile);
  PrintStringField("ownership_lifetime_profile",
                   snapshot.ownership_lifetime_profile);
  PrintStringField("ownership_runtime_hook_profile",
                   snapshot.ownership_runtime_hook_profile);
  PrintStringField("accessor_ownership_profile",
                   snapshot.accessor_ownership_profile);
  PrintStringField("getter_owner_identity", snapshot.getter_owner_identity);
  PrintStringField("setter_owner_identity", snapshot.setter_owner_identity,
                   false);
  std::printf("}");
}

inline void PrintRealizedClassEntryPropertySummary(
    const objc3_runtime_realized_class_entry_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("found", snapshot.found);
  PrintUint64Field("base_identity",
                   static_cast<unsigned long long>(snapshot.base_identity));
  PrintUint64Field("runtime_property_accessor_count",
                   static_cast<unsigned long long>(
                       snapshot.runtime_property_accessor_count));
  PrintUint64Field(
      "runtime_instance_size_bytes",
      static_cast<unsigned long long>(snapshot.runtime_instance_size_bytes));
  PrintStringField("class_name", snapshot.class_name);
  PrintStringField("class_owner_identity", snapshot.class_owner_identity,
                   false);
  std::printf("}");
}

inline void PrintDispatchStatePropertyExecution(
    const objc3_runtime_dispatch_state_snapshot &snapshot) {
  std::printf("{");
  PrintUint64Field("cache_entry_count",
                   static_cast<unsigned long long>(snapshot.cache_entry_count));
  PrintUint64Field("fast_path_seed_count", static_cast<unsigned long long>(
                                               snapshot.fast_path_seed_count));
  PrintUint64Field("fast_path_hit_count", static_cast<unsigned long long>(
                                              snapshot.fast_path_hit_count));
  PrintUint64Field("live_dispatch_count", static_cast<unsigned long long>(
                                              snapshot.live_dispatch_count));
  PrintUint64Field(
      "strict_dispatch_error_count",
      static_cast<unsigned long long>(snapshot.strict_dispatch_error_count));
  PrintUint64Field(
      "last_resolved_parameter_count",
      static_cast<unsigned long long>(snapshot.last_resolved_parameter_count));
  PrintUint64Field(
      "last_property_base_identity",
      static_cast<unsigned long long>(snapshot.last_property_base_identity));
  PrintUint64Field(
      "last_property_slot_index",
      static_cast<unsigned long long>(snapshot.last_property_slot_index));
  PrintIntField("last_dispatch_used_cache", snapshot.last_dispatch_used_cache);
  PrintIntField("last_dispatch_used_fast_path",
                snapshot.last_dispatch_used_fast_path);
  PrintIntField("last_dispatch_resolved_live_method",
                snapshot.last_dispatch_resolved_live_method);
  PrintIntField("last_dispatch_strict_error",
                snapshot.last_dispatch_strict_error);
  PrintIntField("last_effective_direct_dispatch",
                snapshot.last_effective_direct_dispatch);
  PrintIntField("last_used_builtin", snapshot.last_used_builtin);
  PrintStringField("last_selector", snapshot.last_selector);
  PrintStringField("last_fast_path_reason", snapshot.last_fast_path_reason);
  PrintStringField("last_dispatch_path", snapshot.last_dispatch_path);
  PrintStringField("last_implementation_kind",
                   snapshot.last_implementation_kind);
  PrintStringField("last_property_name", snapshot.last_property_name);
  PrintStringField("last_resolved_class_name",
                   snapshot.last_resolved_class_name);
  PrintStringField("last_resolved_owner_identity",
                   snapshot.last_resolved_owner_identity, false);
  std::printf("}");
}

inline void PrintAllocationGraph(
    const objc3_runtime_realized_class_graph_state_snapshot &snapshot) {
  std::printf("{");
  PrintUint64Field("live_instance_count", static_cast<unsigned long long>(
                                              snapshot.live_instance_count));
  PrintStringField("last_allocated_class_name",
                   snapshot.last_allocated_class_name, false);
  std::printf("}");
}

inline void PrintPropertyEntryOwnershipHook(
    const objc3_runtime_property_entry_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("found", snapshot.found);
  PrintIntField("inherited", snapshot.inherited);
  PrintIntField("setter_available", snapshot.setter_available);
  PrintIntField("has_runtime_getter", snapshot.has_runtime_getter);
  PrintIntField("has_runtime_setter", snapshot.has_runtime_setter);
  PrintUint64Field("slot_index",
                   static_cast<unsigned long long>(snapshot.slot_index));
  PrintUint64Field("offset_bytes",
                   static_cast<unsigned long long>(snapshot.offset_bytes));
  PrintStringField("ownership_lifetime_profile",
                   snapshot.ownership_lifetime_profile);
  PrintStringField("ownership_runtime_hook_profile",
                   snapshot.ownership_runtime_hook_profile);
  PrintStringField("accessor_ownership_profile",
                   snapshot.accessor_ownership_profile);
  PrintStringField("getter_owner_identity", snapshot.getter_owner_identity);
  PrintStringField("setter_owner_identity", snapshot.setter_owner_identity,
                   false);
  std::printf("}");
}

inline void PrintMemoryManagementState(
    const objc3_runtime_memory_management_state_snapshot &snapshot) {
  std::printf("{");
  PrintUint64Field(
      "live_runtime_instance_count",
      static_cast<unsigned long long>(snapshot.live_runtime_instance_count));
  PrintUint64Field("weak_target_count",
                   static_cast<unsigned long long>(snapshot.weak_target_count));
  PrintUint64Field("weak_slot_ref_count", static_cast<unsigned long long>(
                                              snapshot.weak_slot_ref_count));
  PrintUint64Field(
      "autoreleasepool_depth",
      static_cast<unsigned long long>(snapshot.autoreleasepool_depth));
  PrintUint64Field(
      "autoreleasepool_max_depth",
      static_cast<unsigned long long>(snapshot.autoreleasepool_max_depth));
  PrintUint64Field(
      "queued_autorelease_value_count",
      static_cast<unsigned long long>(snapshot.queued_autorelease_value_count));
  PrintUint64Field("drained_autorelease_value_count",
                   static_cast<unsigned long long>(
                       snapshot.drained_autorelease_value_count));
  PrintIntField("last_autoreleased_value", snapshot.last_autoreleased_value);
  PrintIntField("last_drained_autorelease_value",
                snapshot.last_drained_autorelease_value, false);
  std::printf("}");
}

inline void PrintPropertyEntryWeakAutoreleasepool(
    const objc3_runtime_property_entry_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("found", snapshot.found);
  PrintIntField("setter_available", snapshot.setter_available);
  PrintIntField("has_runtime_getter", snapshot.has_runtime_getter);
  PrintIntField("has_runtime_setter", snapshot.has_runtime_setter);
  PrintStringField("ownership_lifetime_profile",
                   snapshot.ownership_lifetime_profile);
  PrintStringField("ownership_runtime_hook_profile",
                   snapshot.ownership_runtime_hook_profile);
  PrintStringField("accessor_ownership_profile",
                   snapshot.accessor_ownership_profile, false);
  std::printf("}");
}

inline void PrintPropertyEntryStorageOwnership(
    const objc3_runtime_property_entry_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("found", snapshot.found);
  PrintIntField("setter_available", snapshot.setter_available);
  PrintIntField("has_runtime_getter", snapshot.has_runtime_getter);
  PrintIntField("has_runtime_setter", snapshot.has_runtime_setter);
  PrintUint64Field("base_identity",
                   static_cast<unsigned long long>(snapshot.base_identity));
  PrintUint64Field("slot_index",
                   static_cast<unsigned long long>(snapshot.slot_index));
  PrintUint64Field("offset_bytes",
                   static_cast<unsigned long long>(snapshot.offset_bytes));
  PrintUint64Field("size_bytes",
                   static_cast<unsigned long long>(snapshot.size_bytes));
  PrintUint64Field("alignment_bytes",
                   static_cast<unsigned long long>(snapshot.alignment_bytes));
  PrintUint64Field("instance_size_bytes", static_cast<unsigned long long>(
                                              snapshot.instance_size_bytes));
  PrintStringField("queried_class_name", snapshot.queried_class_name);
  PrintStringField("resolved_class_name", snapshot.resolved_class_name);
  PrintStringField("property_name", snapshot.property_name);
  PrintStringField("effective_getter_selector",
                   snapshot.effective_getter_selector);
  PrintStringField("effective_setter_selector",
                   snapshot.effective_setter_selector);
  PrintStringField("property_attribute_profile",
                   snapshot.property_attribute_profile);
  PrintStringField("ownership_lifetime_profile",
                   snapshot.ownership_lifetime_profile);
  PrintStringField("ownership_runtime_hook_profile",
                   snapshot.ownership_runtime_hook_profile);
  PrintStringField("accessor_ownership_profile",
                   snapshot.accessor_ownership_profile);
  PrintStringField("getter_owner_identity", snapshot.getter_owner_identity);
  PrintStringField("setter_owner_identity", snapshot.setter_owner_identity,
                   false);
  std::printf("}");
}


} // namespace objc3c::runtime::probe

#endif // OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_PROPERTY_MEMORY_JSON_H_
