#pragma once

#include "pipeline/frontend_executable_metadata_handoff.h"

namespace objc3_frontend_executable_metadata_handoff {

void PopulateExecutableMetadataLoweringHandoffSurfaceState(
    Objc3ExecutableMetadataLoweringHandoffSurface &surface,
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
    const Objc3SemaParityContractSurface &sema_parity_surface);

void PublishExecutableMetadataLoweringHandoffSurfaceReadiness(
    Objc3ExecutableMetadataLoweringHandoffSurface &surface);

void PopulateExecutableMetadataTypedLoweringHandoffState(
    Objc3ExecutableMetadataTypedLoweringHandoff &surface,
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3ExecutableMetadataSemanticConsistencyBoundary
        &semantic_consistency_boundary,
    const Objc3ExecutableMetadataSemanticValidationSurface
        &semantic_validation_surface,
    const Objc3ExecutableMetadataLoweringHandoffSurface
        &lowering_handoff_surface);

void PublishExecutableMetadataTypedLoweringHandoffReadiness(
    Objc3ExecutableMetadataTypedLoweringHandoff &surface,
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3ExecutableMetadataLoweringHandoffSurface
        &lowering_handoff_surface);

}  // namespace objc3_frontend_executable_metadata_handoff
