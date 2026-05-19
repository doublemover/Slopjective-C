#include "pipeline/frontend_runtime_metadata_boundary_helpers.h"

#include "pipeline/frontend_runtime_metadata_boundary_helpers_owners.h"

namespace objc3c::pipeline::orchestration {

Objc3RuntimeExportLegalityBoundary BuildRuntimeExportLegalityBoundary(
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
  PopulateRuntimeExportLegalitySemanticState(
      boundary,
      runtime_metadata_source_ownership,
      typed_surface,
      integration_surface,
      protocol_category_summary,
      class_protocol_category_linking_summary,
      selector_normalization_summary,
      property_attribute_summary,
      object_pointer_summary,
      symbol_graph_scope_resolution_summary,
      sema_parity_surface);
  PopulateRuntimeExportLegalityRecordState(
      boundary,
      runtime_metadata_source_ownership,
      class_protocol_category_linking_summary,
      symbol_graph_scope_resolution_summary,
      sema_parity_surface);
  PublishRuntimeExportLegalityFailureReason(boundary, typed_surface);
  PublishRuntimeExportLegalityFailClosed(boundary);
  return boundary;
}

}  // namespace objc3c::pipeline::orchestration
