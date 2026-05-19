#pragma once

#include "pipeline/objc3_frontend_types.h"

namespace objc3c::pipeline::orchestration {

Objc3RuntimeMetadataSourceOwnershipBoundary
BuildRuntimeMetadataSourceOwnershipBoundary(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff);

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
    const Objc3SemaParityContractSurface &sema_parity_surface);

}  // namespace objc3c::pipeline::orchestration
