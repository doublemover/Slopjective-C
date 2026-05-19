#include "pipeline/frontend_executable_metadata_handoff.h"

#include "pipeline/frontend_executable_metadata_handoff_owners.h"

Objc3ExecutableMetadataLoweringHandoffSurface
BuildExecutableMetadataLoweringHandoffSurface(
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3ExecutableMetadataSemanticConsistencyBoundary
        &semantic_consistency_boundary,
    const Objc3ExecutableMetadataSemanticValidationSurface
        &semantic_validation_surface,
    const Objc3SemanticTypeMetadataHandoff &sema_type_metadata_handoff,
    const Objc3FrontendProtocolCategorySummary &protocol_category_summary,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary,
    const Objc3FrontendSelectorNormalizationSummary
        &selector_normalization_summary,
    const Objc3FrontendPropertyAttributeSummary &property_attribute_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary
        &symbol_graph_scope_resolution_summary,
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ExecutableMetadataLoweringHandoffSurface surface;
  objc3_frontend_executable_metadata_handoff::
      PopulateExecutableMetadataLoweringHandoffSurfaceState(
          surface,
          graph,
          semantic_consistency_boundary,
          semantic_validation_surface,
          sema_type_metadata_handoff,
          protocol_category_summary,
          class_protocol_category_linking_summary,
          selector_normalization_summary,
          property_attribute_summary,
          symbol_graph_scope_resolution_summary,
          sema_parity_surface);
  objc3_frontend_executable_metadata_handoff::
      PublishExecutableMetadataLoweringHandoffSurfaceReadiness(surface);
  return surface;
}

Objc3ExecutableMetadataTypedLoweringHandoff
BuildExecutableMetadataTypedLoweringHandoff(
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3ExecutableMetadataSemanticConsistencyBoundary
        &semantic_consistency_boundary,
    const Objc3ExecutableMetadataSemanticValidationSurface
        &semantic_validation_surface,
    const Objc3ExecutableMetadataLoweringHandoffSurface
        &lowering_handoff_surface) {
  Objc3ExecutableMetadataTypedLoweringHandoff surface;
  objc3_frontend_executable_metadata_handoff::
      PopulateExecutableMetadataTypedLoweringHandoffState(
          surface,
          graph,
          semantic_consistency_boundary,
          semantic_validation_surface,
          lowering_handoff_surface);
  objc3_frontend_executable_metadata_handoff::
      PublishExecutableMetadataTypedLoweringHandoffReadiness(
          surface,
          graph,
          lowering_handoff_surface);
  return surface;
}
