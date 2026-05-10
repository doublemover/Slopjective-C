#pragma once

#include "pipeline/frontend_runtime_metadata_boundary_helpers.h"

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
    const Objc3SemaParityContractSurface &sema_parity_surface);

void PopulateRuntimeExportLegalityRecordState(
    Objc3RuntimeExportLegalityBoundary &boundary,
    const Objc3RuntimeMetadataSourceOwnershipBoundary
        &runtime_metadata_source_ownership,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary
        &symbol_graph_scope_resolution_summary,
    const Objc3SemaParityContractSurface &sema_parity_surface);

void PublishRuntimeExportLegalityFailureReason(
    Objc3RuntimeExportLegalityBoundary &boundary,
    const Objc3TypedSemaToLoweringContractSurface &typed_surface);

void PublishRuntimeExportLegalityFailClosed(
    Objc3RuntimeExportLegalityBoundary &boundary);

}  // namespace objc3c::pipeline::orchestration
