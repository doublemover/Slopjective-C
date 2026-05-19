#include "pipeline/frontend_runtime_metadata_boundary_helpers_owners.h"

namespace objc3c::pipeline::orchestration {

void PopulateRuntimeExportLegalityRecordState(
    Objc3RuntimeExportLegalityBoundary &boundary,
    const Objc3RuntimeMetadataSourceOwnershipBoundary
        &runtime_metadata_source_ownership,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary
        &symbol_graph_scope_resolution_summary,
    const Objc3SemaParityContractSurface &sema_parity_surface) {
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
}

}  // namespace objc3c::pipeline::orchestration
