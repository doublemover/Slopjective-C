#include "pipeline/frontend_runtime_metadata_boundary_helpers_owners.h"

namespace objc3c::pipeline::orchestration {

void PopulateRuntimeExportLegalitySemanticState(
    Objc3RuntimeExportLegalityBoundary &boundary,
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
}

}  // namespace objc3c::pipeline::orchestration
