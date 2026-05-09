#pragma once

#include <algorithm>
#include <cstddef>

#include "pipeline/objc3_frontend_types.h"

namespace objc3c::pipeline::orchestration {

inline Objc3RuntimeMetadataSourceOwnershipBoundary
BuildRuntimeMetadataSourceOwnershipBoundary(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff) {
  Objc3RuntimeMetadataSourceOwnershipBoundary boundary;
  const std::size_t sema_interface_implementation_record_count =
      type_metadata_handoff.interfaces_lexicographic.size() +
      type_metadata_handoff.implementations_lexicographic.size();
  const bool sema_interface_implementation_record_count_present =
      sema_interface_implementation_record_count > 0u ||
      type_metadata_handoff.interface_implementation_summary.declared_interfaces > 0u ||
      type_metadata_handoff.interface_implementation_summary.declared_implementations > 0u;

  boundary.frontend_owns_runtime_metadata_source_records = true;
  boundary.runtime_metadata_source_records_ready_for_lowering = false;
  boundary.native_runtime_library_present = false;
  boundary.runtime_link_test_only = true;
  boundary.class_record_count = records.classes_lexicographic.size();
  boundary.protocol_record_count = records.protocols_lexicographic.size();
  boundary.category_interface_record_count =
      static_cast<std::size_t>(std::count_if(
          records.categories_lexicographic.begin(),
          records.categories_lexicographic.end(),
          [](const Objc3RuntimeMetadataCategorySourceRecord &record) {
            return record.record_kind == "interface";
          }));
  boundary.category_implementation_record_count =
      static_cast<std::size_t>(std::count_if(
          records.categories_lexicographic.begin(),
          records.categories_lexicographic.end(),
          [](const Objc3RuntimeMetadataCategorySourceRecord &record) {
            return record.record_kind == "implementation";
          }));
  boundary.property_record_count = records.properties_lexicographic.size();
  boundary.method_record_count = records.methods_lexicographic.size();
  boundary.ivar_record_count = records.ivars_lexicographic.size();

  // Diagnostic precision anchor: category containers are separate
  // runtime-metadata records, so the sema handoff is checked against class
  // source records only until category-specific sema ownership is wired.
  const std::size_t source_interface_implementation_record_count =
      boundary.class_record_count;
  const bool class_alignment_consistent =
      !sema_interface_implementation_record_count_present ||
      sema_interface_implementation_record_count ==
          source_interface_implementation_record_count;
  boundary.deterministic_source_schema =
      IsReadyObjc3RuntimeMetadataSourceRecordSet(records) &&
      class_alignment_consistent &&
      boundary.ivar_record_count <= boundary.property_record_count &&
      !boundary.contract_id.empty() &&
      !boundary.canonical_source_schema.empty() &&
      !boundary.class_record_ast_anchor.empty() &&
      !boundary.protocol_record_ast_anchor.empty() &&
      !boundary.category_record_ast_anchor.empty() &&
      !boundary.property_record_ast_anchor.empty() &&
      !boundary.method_record_ast_anchor.empty() &&
      !boundary.ivar_record_ast_anchor.empty() &&
      !boundary.ivar_record_source_model.empty();
  boundary.fail_closed =
      boundary.frontend_owns_runtime_metadata_source_records &&
      !boundary.runtime_metadata_source_records_ready_for_lowering &&
      !boundary.native_runtime_library_present &&
      boundary.runtime_link_test_only;

  if (!class_alignment_consistent) {
    boundary.failure_reason = "AST/sema class metadata source counts diverged";
  } else if (boundary.ivar_record_count > boundary.property_record_count) {
    boundary.failure_reason = "ivar source records exceed property source records";
  } else if (!boundary.deterministic_source_schema) {
    boundary.failure_reason = "runtime metadata source schema anchors are incomplete";
  } else if (!boundary.fail_closed) {
    boundary.failure_reason =
        "runtime metadata source ownership boundary is not fail-closed";
  }

  return boundary;
}

inline Objc3RuntimeExportLegalityBoundary BuildRuntimeExportLegalityBoundary(
    const Objc3RuntimeMetadataSourceOwnershipBoundary
        &runtime_metadata_source_ownership,
    const Objc3TypedSemaToLoweringContractSurface &typed_surface,
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3FrontendProtocolCategorySummary &protocol_category_summary,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary,
    const Objc3FrontendSelectorNormalizationSummary
        &selector_normalization_summary,
    const Objc3FrontendPropertyAttributeSummary &property_attribute_summary,
    const Objc3FrontendObjectPointerNullabilityGenericsSummary
        &object_pointer_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary
        &symbol_graph_scope_resolution_summary,
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3RuntimeExportLegalityBoundary boundary;
  boundary.semantic_integration_surface_built = integration_surface.built;
  boundary.sema_type_metadata_handoff_deterministic =
      typed_surface.semantic_type_metadata_handoff_deterministic &&
      sema_parity_surface.deterministic_type_metadata_handoff;
  boundary.typed_sema_surface_ready =
      typed_surface.semantic_integration_surface_built;
  boundary.typed_sema_surface_deterministic =
      typed_surface.semantic_type_metadata_handoff_deterministic &&
      typed_surface.protocol_category_handoff_deterministic &&
      typed_surface.class_protocol_category_linking_handoff_deterministic &&
      typed_surface.selector_normalization_handoff_deterministic &&
      typed_surface.property_attribute_handoff_deterministic &&
      typed_surface.object_pointer_type_handoff_deterministic &&
      typed_surface.symbol_graph_handoff_deterministic &&
      typed_surface.scope_resolution_handoff_deterministic;
  boundary.runtime_metadata_source_boundary_ready =
      IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(
          runtime_metadata_source_ownership);
  boundary.protocol_category_deterministic =
      protocol_category_summary.deterministic_protocol_category_handoff;
  boundary.class_protocol_category_linking_deterministic =
      class_protocol_category_linking_summary
          .deterministic_class_protocol_category_linking_handoff;
  boundary.selector_normalization_deterministic =
      selector_normalization_summary.deterministic_selector_normalization_handoff;
  boundary.property_attribute_deterministic =
      property_attribute_summary.deterministic_property_attribute_handoff;
  boundary.object_pointer_surface_deterministic =
      object_pointer_summary
          .deterministic_object_pointer_nullability_generics_handoff;
  boundary.symbol_graph_scope_resolution_deterministic =
      symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff &&
      symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff;
  boundary.property_synthesis_ivar_binding_deterministic =
      sema_parity_surface.property_synthesis_ivar_binding_summary.deterministic &&
      sema_parity_surface.deterministic_property_synthesis_ivar_binding_handoff;

  boundary.class_record_count =
      runtime_metadata_source_ownership.class_record_count;
  boundary.protocol_record_count =
      runtime_metadata_source_ownership.protocol_record_count;
  boundary.category_record_count =
      runtime_metadata_source_ownership.category_record_count();
  boundary.property_record_count =
      runtime_metadata_source_ownership.property_record_count;
  boundary.method_record_count =
      runtime_metadata_source_ownership.method_record_count;
  boundary.ivar_record_count =
      runtime_metadata_source_ownership.ivar_record_count;
  boundary.invalid_protocol_composition_sites =
      class_protocol_category_linking_summary.invalid_protocol_composition_sites;
  boundary.property_attribute_invalid_entries =
      sema_parity_surface.property_attribute_invalid_attribute_entries_total;
  boundary.property_attribute_contract_violations =
      sema_parity_surface.property_attribute_contract_violations_total;
  boundary.invalid_type_annotation_sites =
      sema_parity_surface.type_annotation_invalid_generic_suffix_sites_total +
      sema_parity_surface.type_annotation_invalid_pointer_declarator_sites_total +
      sema_parity_surface.type_annotation_invalid_nullability_suffix_sites_total +
      sema_parity_surface.type_annotation_invalid_ownership_qualifier_sites_total;
  boundary.property_ivar_binding_missing =
      sema_parity_surface.property_synthesis_ivar_binding_summary
          .ivar_binding_missing;
  boundary.property_ivar_binding_conflicts =
      sema_parity_surface.property_synthesis_ivar_binding_summary
          .ivar_binding_conflicts;
  boundary.implementation_resolution_misses =
      symbol_graph_scope_resolution_summary
          .implementation_interface_resolution_misses;
  boundary.method_resolution_misses =
      symbol_graph_scope_resolution_summary.method_resolution_misses;

  if (boundary.contract_id.empty()) {
    boundary.failure_reason = "runtime export legality contract id is empty";
  } else if (!boundary.sema_type_metadata_handoff_deterministic) {
    boundary.failure_reason =
        typed_surface.failure_reason.empty()
            ? "semantic type-metadata handoff is not deterministic"
            : typed_surface.failure_reason;
  } else if (!boundary.typed_sema_surface_ready) {
    boundary.failure_reason =
        "typed sema runtime-export handoff is not ready";
  } else if (!boundary.typed_sema_surface_deterministic) {
    boundary.failure_reason =
        "typed sema runtime-export handoff is not deterministic";
  } else if (!boundary.runtime_metadata_source_boundary_ready) {
    boundary.failure_reason =
        "runtime metadata source ownership boundary is not ready";
  } else if (!boundary.protocol_category_deterministic) {
    boundary.failure_reason =
        "protocol/category semantic handoff is not deterministic";
  } else if (!boundary.class_protocol_category_linking_deterministic) {
    boundary.failure_reason =
        "class/protocol/category linking handoff is not deterministic";
  } else if (!boundary.selector_normalization_deterministic) {
    boundary.failure_reason =
        "selector normalization handoff is not deterministic";
  } else if (!boundary.property_attribute_deterministic) {
    boundary.failure_reason = "property attribute handoff is not deterministic";
  } else if (!boundary.object_pointer_surface_deterministic) {
    boundary.failure_reason =
        "object-pointer/nullability/generics handoff is not deterministic";
  } else if (!boundary.symbol_graph_scope_resolution_deterministic) {
    boundary.failure_reason =
        "symbol-graph/scope-resolution handoff is not deterministic";
  } else if (!boundary.property_synthesis_ivar_binding_deterministic) {
    boundary.failure_reason =
        "property synthesis/ivar binding handoff is not deterministic";
  } else if (boundary.invalid_protocol_composition_sites >
             boundary.protocol_record_count + boundary.category_record_count) {
    boundary.failure_reason =
        "invalid protocol composition sites exceed export-bearing records";
  } else if (boundary.ivar_record_count > boundary.property_record_count) {
    boundary.failure_reason =
        "ivar export records exceed property export records";
  }

  boundary.semantic_boundary_frozen = boundary.failure_reason.empty();
  boundary.metadata_export_enforcement_ready = false;
  boundary.fail_closed =
      boundary.semantic_boundary_frozen &&
      !boundary.metadata_export_enforcement_ready &&
      boundary.duplicate_runtime_identity_enforcement_pending &&
      boundary.incomplete_declaration_export_blocking_pending &&
      boundary.illegal_redeclaration_mix_export_blocking_pending;
  if (boundary.failure_reason.empty() && !boundary.fail_closed) {
    boundary.failure_reason =
        "runtime export legality freeze is not fail-closed";
  }
  return boundary;
}

}  // namespace objc3c::pipeline::orchestration
