#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_REALIZED_GRAPH_JSON_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_REALIZED_GRAPH_JSON_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/json_probe_writer.h"

#include <cstdio>

namespace objc3c::runtime::probe {

inline void PrintRealizedGraphStateAllocation(
    const objc3_runtime_realized_class_graph_state_snapshot &snapshot) {
  std::printf("{");
  PrintUint64Field("realized_class_count", static_cast<unsigned long long>(
                                               snapshot.realized_class_count));
  PrintUint64Field("root_class_count",
                   static_cast<unsigned long long>(snapshot.root_class_count));
  PrintUint64Field("metaclass_edge_count", static_cast<unsigned long long>(
                                               snapshot.metaclass_edge_count));
  PrintUint64Field(
      "receiver_class_binding_count",
      static_cast<unsigned long long>(snapshot.receiver_class_binding_count));
  PrintUint64Field(
      "attached_category_count",
      static_cast<unsigned long long>(snapshot.attached_category_count));
  PrintUint64Field("protocol_conformance_edge_count",
                   static_cast<unsigned long long>(
                       snapshot.protocol_conformance_edge_count));
  PrintUint64Field("live_instance_count", static_cast<unsigned long long>(
                                              snapshot.live_instance_count));
  PrintUint64Field("last_allocated_receiver_identity",
                   static_cast<unsigned long long>(
                       snapshot.last_allocated_receiver_identity));
  PrintUint64Field(
      "last_allocated_base_identity",
      static_cast<unsigned long long>(snapshot.last_allocated_base_identity));
  PrintUint64Field("last_allocated_instance_size_bytes",
                   static_cast<unsigned long long>(
                       snapshot.last_allocated_instance_size_bytes));
  PrintUint64Field(
      "last_allocated_allocation_ordinal",
      static_cast<unsigned long long>(
          snapshot.last_allocated_allocation_ordinal));
  PrintUint64Field(
      "last_initialized_receiver_identity",
      static_cast<unsigned long long>(
          snapshot.last_initialized_receiver_identity));
  PrintUint64Field(
      "last_initialized_initialization_ordinal",
      static_cast<unsigned long long>(
          snapshot.last_initialized_initialization_ordinal));
  PrintStringField("last_realized_class_name",
                   snapshot.last_realized_class_name);
  PrintStringField("last_realized_class_owner_identity",
                   snapshot.last_realized_class_owner_identity);
  PrintStringField("last_realized_metaclass_owner_identity",
                   snapshot.last_realized_metaclass_owner_identity);
  PrintStringField("last_allocated_class_name",
                   snapshot.last_allocated_class_name);
  PrintStringField("last_instance_lifecycle_failure_reason",
                   snapshot.last_instance_lifecycle_failure_reason, false);
  std::printf("}");
}

inline void PrintRealizedClassEntryAllocation(
    const objc3_runtime_realized_class_entry_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("found", snapshot.found);
  PrintUint64Field("base_identity",
                   static_cast<unsigned long long>(snapshot.base_identity));
  PrintUint64Field("instance_receiver_identity",
                   static_cast<unsigned long long>(
                       snapshot.instance_receiver_identity));
  PrintUint64Field("class_receiver_identity",
                   static_cast<unsigned long long>(
                       snapshot.class_receiver_identity));
  PrintUint64Field(
      "registration_order_ordinal",
      static_cast<unsigned long long>(snapshot.registration_order_ordinal));
  PrintIntField("is_root_class", snapshot.is_root_class);
  PrintIntField("has_super_node", snapshot.has_super_node);
  PrintIntField("implementation_backed", snapshot.implementation_backed);
  PrintUint64Field(
      "attached_category_count",
      static_cast<unsigned long long>(snapshot.attached_category_count));
  PrintUint64Field(
      "direct_protocol_count",
      static_cast<unsigned long long>(snapshot.direct_protocol_count));
  PrintUint64Field(
      "attached_protocol_count",
      static_cast<unsigned long long>(snapshot.attached_protocol_count));
  PrintUint64Field("runtime_property_accessor_count",
                   static_cast<unsigned long long>(
                       snapshot.runtime_property_accessor_count));
  PrintUint64Field(
      "runtime_instance_size_bytes",
      static_cast<unsigned long long>(snapshot.runtime_instance_size_bytes));
  PrintUint64Field(
      "super_base_identity",
      static_cast<unsigned long long>(snapshot.super_base_identity));
  PrintStringField("class_name", snapshot.class_name);
  PrintStringField("class_owner_identity", snapshot.class_owner_identity);
  PrintStringField("metaclass_owner_identity",
                   snapshot.metaclass_owner_identity, false);
  std::printf("}");
}

inline void PrintRealizedGraphStateMetaclass(
    const objc3_runtime_realized_class_graph_state_snapshot &snapshot) {
  std::printf("{");
  PrintUint64Field("realized_class_count", static_cast<unsigned long long>(
                                               snapshot.realized_class_count));
  PrintUint64Field("root_class_count",
                   static_cast<unsigned long long>(snapshot.root_class_count));
  PrintUint64Field("metaclass_edge_count", static_cast<unsigned long long>(
                                               snapshot.metaclass_edge_count));
  PrintUint64Field(
      "receiver_class_binding_count",
      static_cast<unsigned long long>(snapshot.receiver_class_binding_count));
  PrintStringField("last_realized_class_name",
                   snapshot.last_realized_class_name);
  PrintStringField("last_realized_class_owner_identity",
                   snapshot.last_realized_class_owner_identity);
  PrintStringField("last_realized_metaclass_owner_identity",
                   snapshot.last_realized_metaclass_owner_identity, false);
  std::printf("}");
}

inline void PrintRealizedEntryMetaclass(
    const objc3_runtime_realized_class_entry_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("found", snapshot.found);
  PrintUint64Field("base_identity",
                   static_cast<unsigned long long>(snapshot.base_identity));
  PrintUint64Field(
      "registration_order_ordinal",
      static_cast<unsigned long long>(snapshot.registration_order_ordinal));
  PrintIntField("is_root_class", snapshot.is_root_class);
  PrintIntField("implementation_backed", snapshot.implementation_backed);
  PrintStringField("module_name", snapshot.module_name);
  PrintStringField("translation_unit_identity_key",
                   snapshot.translation_unit_identity_key);
  PrintStringField("class_name", snapshot.class_name);
  PrintStringField("class_owner_identity", snapshot.class_owner_identity);
  PrintStringField("metaclass_owner_identity",
                   snapshot.metaclass_owner_identity);
  PrintStringField("super_class_owner_identity",
                   snapshot.super_class_owner_identity);
  PrintStringField("super_metaclass_owner_identity",
                   snapshot.super_metaclass_owner_identity, false);
  std::printf("}");
}

inline void PrintGraphStateProtocolCategory(
    const objc3_runtime_realized_class_graph_state_snapshot &snapshot) {
  std::printf("{");
  PrintUint64Field("realized_class_count", static_cast<unsigned long long>(
                                               snapshot.realized_class_count));
  PrintUint64Field("root_class_count",
                   static_cast<unsigned long long>(snapshot.root_class_count));
  PrintUint64Field("metaclass_edge_count", static_cast<unsigned long long>(
                                               snapshot.metaclass_edge_count));
  PrintUint64Field(
      "receiver_class_binding_count",
      static_cast<unsigned long long>(snapshot.receiver_class_binding_count));
  PrintUint64Field(
      "attached_category_count",
      static_cast<unsigned long long>(snapshot.attached_category_count));
  PrintUint64Field("protocol_conformance_edge_count",
                   static_cast<unsigned long long>(
                       snapshot.protocol_conformance_edge_count));
  PrintStringField("last_realized_class_name",
                   snapshot.last_realized_class_name);
  PrintStringField("last_realized_class_owner_identity",
                   snapshot.last_realized_class_owner_identity);
  PrintStringField("last_realized_metaclass_owner_identity",
                   snapshot.last_realized_metaclass_owner_identity);
  PrintStringField("last_attached_category_owner_identity",
                   snapshot.last_attached_category_owner_identity);
  PrintStringField("last_attached_category_name",
                   snapshot.last_attached_category_name);
  PrintStringField("last_malformed_class_graph_diagnostic_code",
                   snapshot.last_malformed_class_graph_diagnostic_code);
  PrintStringField("last_malformed_class_graph_diagnostic_class",
                   snapshot.last_malformed_class_graph_diagnostic_class,
                   false);
  std::printf("}");
}

inline void PrintRealizedEntryProtocolCategory(
    const objc3_runtime_realized_class_entry_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("found", snapshot.found);
  PrintUint64Field("base_identity",
                   static_cast<unsigned long long>(snapshot.base_identity));
  PrintUint64Field(
      "registration_order_ordinal",
      static_cast<unsigned long long>(snapshot.registration_order_ordinal));
  PrintIntField("is_root_class", snapshot.is_root_class);
  PrintIntField("implementation_backed", snapshot.implementation_backed);
  PrintUint64Field(
      "attached_category_count",
      static_cast<unsigned long long>(snapshot.attached_category_count));
  PrintUint64Field(
      "direct_protocol_count",
      static_cast<unsigned long long>(snapshot.direct_protocol_count));
  PrintUint64Field(
      "attached_protocol_count",
      static_cast<unsigned long long>(snapshot.attached_protocol_count));
  PrintStringField("module_name", snapshot.module_name);
  PrintStringField("translation_unit_identity_key",
                   snapshot.translation_unit_identity_key);
  PrintStringField("class_name", snapshot.class_name);
  PrintStringField("class_owner_identity", snapshot.class_owner_identity);
  PrintStringField("metaclass_owner_identity",
                   snapshot.metaclass_owner_identity);
  PrintStringField("super_class_owner_identity",
                   snapshot.super_class_owner_identity);
  PrintStringField("super_metaclass_owner_identity",
                   snapshot.super_metaclass_owner_identity);
  PrintStringField("last_attached_category_owner_identity",
                   snapshot.last_attached_category_owner_identity);
  PrintStringField("last_attached_category_name",
                   snapshot.last_attached_category_name, false);
  std::printf("}");
}

inline void PrintConformanceQueryProtocolCategory(
    const objc3_runtime_protocol_conformance_query_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("class_found", snapshot.class_found);
  PrintIntField("protocol_found", snapshot.protocol_found);
  PrintIntField("conforms", snapshot.conforms);
  PrintUint64Field(
      "visited_protocol_count",
      static_cast<unsigned long long>(snapshot.visited_protocol_count));
  PrintUint64Field(
      "attached_category_count",
      static_cast<unsigned long long>(snapshot.attached_category_count));
  PrintUint64Field(
      "matched_protocol_depth",
      static_cast<unsigned long long>(snapshot.matched_protocol_depth));
  PrintIntField("matched_from_category", snapshot.matched_from_category);
  PrintIntField("matched_from_superclass", snapshot.matched_from_superclass);
  PrintIntField("matched_via_inherited_protocol",
                snapshot.matched_via_inherited_protocol);
  PrintIntField("malformed_metadata", snapshot.malformed_metadata);
  PrintIntField("witness_metadata_materialized",
                snapshot.witness_metadata_materialized);
  PrintIntField("witness_metadata_supported",
                snapshot.witness_metadata_supported);
  PrintIntField("conformance_edge_materializable",
                snapshot.conformance_edge_materializable);
  PrintIntField("associated_types_supported",
                snapshot.associated_types_supported);
  PrintIntField("dynamic_existential_dispatch_supported",
                snapshot.dynamic_existential_dispatch_supported);
  PrintIntField("fail_closed_for_unsupported_semantics",
                snapshot.fail_closed_for_unsupported_semantics);
  PrintStringField("class_name", snapshot.class_name);
  PrintStringField("protocol_name", snapshot.protocol_name);
  PrintStringField("matched_protocol_owner_identity",
                   snapshot.matched_protocol_owner_identity);
  PrintStringField("matched_attachment_owner_identity",
                   snapshot.matched_attachment_owner_identity);
  PrintStringField("matched_class_name", snapshot.matched_class_name);
  PrintStringField("matched_class_owner_identity",
                   snapshot.matched_class_owner_identity);
  PrintStringField("existential_canonical_spelling",
                   snapshot.existential_canonical_spelling);
  PrintStringField("object_representation", snapshot.object_representation);
  PrintStringField("conformance_owner_identity",
                   snapshot.conformance_owner_identity);
  PrintStringField("runtime_lookup_anchor", snapshot.runtime_lookup_anchor);
  PrintStringField("witness_metadata_key", snapshot.witness_metadata_key);
  PrintStringField("requirement_resolution_policy",
                   snapshot.requirement_resolution_policy);
  PrintStringField("unsupported_associated_type_diagnostic",
                   snapshot.unsupported_associated_type_diagnostic);
  PrintStringField("unsupported_dynamic_dispatch_diagnostic",
                   snapshot.unsupported_dynamic_dispatch_diagnostic);
  PrintStringField("failure_reason", snapshot.failure_reason, false);
  std::printf("}");
}

inline void PrintRealizedClassEntryCanonicalSummary(
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
  PrintStringField("class_owner_identity", snapshot.class_owner_identity);
  PrintStringField("last_attached_category_owner_identity",
                   snapshot.last_attached_category_owner_identity, false);
  std::printf("}");
}

inline void PrintConformanceQueryCanonicalSummary(
    const objc3_runtime_protocol_conformance_query_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("conforms", snapshot.conforms);
  PrintIntField("malformed_metadata", snapshot.malformed_metadata);
  PrintStringField("class_name", snapshot.class_name);
  PrintStringField("protocol_name", snapshot.protocol_name);
  PrintStringField("matched_protocol_owner_identity",
                   snapshot.matched_protocol_owner_identity);
  PrintStringField("matched_attachment_owner_identity",
                   snapshot.matched_attachment_owner_identity);
  PrintIntField("witness_metadata_materialized",
                snapshot.witness_metadata_materialized);
  PrintStringField("existential_canonical_spelling",
                   snapshot.existential_canonical_spelling);
  PrintStringField("conformance_owner_identity",
                   snapshot.conformance_owner_identity);
  PrintStringField("failure_reason", snapshot.failure_reason, false);
  std::printf("}");
}

inline void PrintConformanceQueryProtocolInheritance(
    const objc3_runtime_protocol_conformance_query_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("class_found", snapshot.class_found);
  PrintIntField("protocol_found", snapshot.protocol_found);
  PrintIntField("conforms", snapshot.conforms);
  PrintUint64Field(
      "visited_protocol_count",
      static_cast<unsigned long long>(snapshot.visited_protocol_count));
  PrintUint64Field(
      "attached_category_count",
      static_cast<unsigned long long>(snapshot.attached_category_count));
  PrintUint64Field(
      "matched_protocol_depth",
      static_cast<unsigned long long>(snapshot.matched_protocol_depth));
  PrintIntField("matched_from_category", snapshot.matched_from_category);
  PrintIntField("matched_from_superclass", snapshot.matched_from_superclass);
  PrintIntField("matched_via_inherited_protocol",
                snapshot.matched_via_inherited_protocol);
  PrintIntField("malformed_metadata", snapshot.malformed_metadata);
  PrintIntField("witness_metadata_materialized",
                snapshot.witness_metadata_materialized);
  PrintIntField("witness_metadata_supported",
                snapshot.witness_metadata_supported);
  PrintIntField("conformance_edge_materializable",
                snapshot.conformance_edge_materializable);
  PrintIntField("associated_types_supported",
                snapshot.associated_types_supported);
  PrintIntField("dynamic_existential_dispatch_supported",
                snapshot.dynamic_existential_dispatch_supported);
  PrintIntField("fail_closed_for_unsupported_semantics",
                snapshot.fail_closed_for_unsupported_semantics);
  PrintStringField("class_name", snapshot.class_name);
  PrintStringField("protocol_name", snapshot.protocol_name);
  PrintStringField("matched_protocol_owner_identity",
                   snapshot.matched_protocol_owner_identity);
  PrintStringField("matched_attachment_owner_identity",
                   snapshot.matched_attachment_owner_identity);
  PrintStringField("matched_class_name", snapshot.matched_class_name);
  PrintStringField("matched_class_owner_identity",
                   snapshot.matched_class_owner_identity);
  PrintStringField("existential_canonical_spelling",
                   snapshot.existential_canonical_spelling);
  PrintStringField("object_representation", snapshot.object_representation);
  PrintStringField("conformance_owner_identity",
                   snapshot.conformance_owner_identity);
  PrintStringField("runtime_lookup_anchor", snapshot.runtime_lookup_anchor);
  PrintStringField("witness_metadata_key", snapshot.witness_metadata_key);
  PrintStringField("requirement_resolution_policy",
                   snapshot.requirement_resolution_policy);
  PrintStringField("unsupported_associated_type_diagnostic",
                   snapshot.unsupported_associated_type_diagnostic);
  PrintStringField("unsupported_dynamic_dispatch_diagnostic",
                   snapshot.unsupported_dynamic_dispatch_diagnostic);
  PrintStringField("failure_reason", snapshot.failure_reason, false);
  std::printf("}");
}

inline void PrintPropertyEntryCanonicalSummary(
    const objc3_runtime_property_entry_snapshot &snapshot) {
  std::printf("{");
  PrintIntField("found", snapshot.found);
  PrintIntField("setter_available", snapshot.setter_available);
  PrintIntField("has_runtime_getter", snapshot.has_runtime_getter);
  PrintIntField("has_runtime_setter", snapshot.has_runtime_setter);
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
  PrintStringField("property_name", snapshot.property_name);
  PrintStringField("effective_getter_selector",
                   snapshot.effective_getter_selector);
  PrintStringField("effective_setter_selector",
                   snapshot.effective_setter_selector);
  PrintStringField("getter_owner_identity", snapshot.getter_owner_identity);
  PrintStringField("setter_owner_identity", snapshot.setter_owner_identity,
                   false);
  std::printf("}");
}


} // namespace objc3c::runtime::probe

#endif // OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_REALIZED_GRAPH_JSON_H_
